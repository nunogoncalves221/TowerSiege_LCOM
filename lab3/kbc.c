#include <lcom/lcf.h>
#include "kbc.h"
#include "i8042.h"

//will be shared with lab3.c using 'extern'
uint8_t scancode = 0; 
bool ih_error = false;

int kbd_hook_id = 1; //define an id

int (kbd_subscribe_int)(uint8_t *bit_no) {
    if (bit_no == NULL) return 1;

    // now we give the caller the bit mask they'll need for the interrupt loop
    *bit_no = kbd_hook_id;

    //using IRQ_REENABLE and IRQ_EXCLUSIVE so minix doesn't steal our keystrokes
    if (sys_irqsetpolicy(IRQ_KEYBOARD, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id) != 0) {
        return 1;
    }

    return 0;
}

int (kbd_unsubscribe_int)() {
    //simple unsubscribe using our global hook id
    if (sys_irqrmpolicy(&kbd_hook_id) != 0) {
        return 1;
    }
    return 0;
}

/* FOR kbd_test_scan() */
//interrupt handler
void (kbc_ih)() {
    uint8_t status = 0;
    //we assume that data is always invalid at first
    ih_error = true; //reset the error flag every time we get a new interrupt

    //first let's read the status register - port Ox64 - to see what's up
    if (util_sys_inb(KBC_CMD_REG, &status) != 0) {
        return;
    }

    //if the output buffer is full, it means we have a keystroke waiting for us
    if (status & KBC_OBF) {
        
        //so we grab the scancode from the output buffer
        if (util_sys_inb(KBC_OUT_BUF, &scancode) != 0) {
            return;
        }

        // now let's check for parity or timeout errors - bits 7 ad 6
        // if any of these bits are set to 1, the data is trash
        if ((status & KBC_PAR_ERR) || (status & KBC_TO_ERR)) {
            return;
        }

        //only here 
        ih_error = false;
    }
}

/* FOR kdb_test_poll */
int (kbd_poll)() {
    uint8_t status = 0;

    //loop until MAX_RETRIES defined
    for (int i = 0; i < MAX_RETRIES; i++){

        //read the status register - port 0X64
        if (util_sys_inb(KBC_CMD_REG, &status) != 0) return 1;

        //see if the output buffer is full
        if (status & KBC_OBF){

            //reading the scancode from the output buffer
            if (util_sys_inb(KBC_OUT_BUF, &scancode) != 0) return 1;
               
            //check if exists parity or timeout errors - bits 7 and 6
            if ((status & KBC_PAR_ERR) || (status & KBC_TO_ERR)) return 1; 

            return 0; //sucess
        }

        tickdelay(micros_to_ticks(DELAY)); //if buffer was empty, wait DELAY time and try sgain
    }

    return 1; //failed after MAX_RETRIES
}

/* AUXILIAR FUNCTIONS FOR kbd_restore_interrupts() */

// to write a cmd or an argument
int (kbc_write_cmd) (int port, uint8_t cmd){
    uint8_t status;

    for (int i = 0; i < MAX_RETRIES; i++){
        if (util_sys_inb(KBC_CMD_REG, &status) != 0) return 1;

        // we write when the input buffer is empty
        if ((status & KBC_IBF) == 0) return sys_outb(port, cmd);

        tickdelay(micros_to_ticks(DELAY)); //wait DELAY time and repeat
    }

    return 1; //failed after MAX_RETRIES
}

// to read the output of a cmd
int (kbc_read_ret) (uint8_t *ret) {
    uint8_t status;

    for (int i = 0; i < MAX_RETRIES; i++){
        if (util_sys_inb(KBC_CMD_REG, &status) != 0) return 1;

        // we read when the output buffer is full
        if (status && KBC_OBF){
            if (util_sys_inb(KBC_OUT_BUF, ret) != 0) return 1;

            // exists parity or timeout error? invalid read
            if ((status & (KBC_PAR_ERR | KBC_TO_ERR)) != 0) return 1;

            return 0;
        }
        tickdelay(micros_to_ticks(DELAY));
    }
    return 1;
}


/* FUNCTION TO ENABLE KBC INTERRUPTS */
int (kbd_restore_interrupts)() {
    uint8_t command_byte;

    // tell the kbc we want to read the command byte
    if (kbc_write_cmd(KBC_CMD_REG, READ_COMMAND_BYTE) != 0) return 1;

    // now we read the current command byte from the output buffer
    if (kbc_read_ret(&command_byte) != 0) return 1;

    // set the bit to enable keyboard interrupts (bit 0)
    command_byte = command_byte | ENABLE_INTERRUPTS_KEYBOARD;

    // tell the kbc we are about to write a new command byte
    if (kbc_write_cmd(KBC_CMD_REG, WRITE_COMMAND_BYTE) != 0) return 1;

    // finally we send our updated command byte
    if (kbc_write_cmd(KBC_OUT_BUF, command_byte) != 0) return 1;

    return 0;
}
