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

//interrupt handler
void (kbc_ih)() {
    uint8_t status = 0;
    //we assume that data is always invalid at first
    ih_error = true; //reset the error flag every time we get a new interrupt

    //first let's read the status register - port Ox64 - to see what's up
    if (util_sys_inb(KBC_CMD_REG, &status) != 0) {
        ih_error = true;
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
