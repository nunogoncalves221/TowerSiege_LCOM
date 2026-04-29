#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "i8042.h"

/* for mouse_ih() */
uint8_t packet_bytes[3]; // stores the 3 bytes of the current packet
int byte_idx = 0;        // keeps track of which byte we are reading (0, 1, or 2)
uint8_t current_byte;    // stores the most recently read byte from the KBC
bool valid_byte = false; // extra flag to know if the read was valid

int hook_id_mouse = IRQ_MOUSE; // hook id for mouse interrupts

// subscribes to mouse interrupts
int mouse_subscribe_int(uint8_t *bit_no) {
    *bit_no = hook_id_mouse;
    return sys_irqsetpolicy(IRQ_MOUSE, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_mouse);
}

// unsubscribes from mouse interrupts
int mouse_unsubscribe_int() {
    return sys_irqrmpolicy(&hook_id_mouse);
}

// helper function to write a byte to a kbc port, with retries
static int kbc_write(uint8_t port, uint8_t byte) {
    uint8_t status;
    int retries = MAX_RETRIES;
    while (retries > 0) {
        if (util_sys_inb(KBC_CMD_REG, &status) != 0) return 1;

        // check if input buffer is clear
        if ((status & KBC_IBF) == 0) {
            if (sys_outb(port, byte) == 0) return 0; // success
            return 1; // sys_outb failed
        }

        tickdelay(micros_to_ticks(DELAY));
        retries--;
    }
    return 1; // timeout
}

int mouse_write_command(uint8_t cmd) {
    uint8_t ack_byte;
    uint8_t status;
    int retries = MAX_RETRIES;

    // flush the output buffer to remove any garbage left from previous tests
    do {
        util_sys_inb(KBC_CMD_REG, &status);
        if (status & KBC_OBF) {
            uint8_t trash;
            util_sys_inb(KBC_OUT_BUF, &trash);
        }
    } while (status & KBC_OBF);

    while (retries > 0) {
        // tell the kbc we are sending a mouse command
        if (kbc_write(KBC_CMD_REG, WRITE_TO_MOUSE) != 0) return 1;

        // send the actual command
        if (kbc_write(KBC_OUT_BUF, cmd) != 0) return 1;

        // wait and read the acknowledgment byte
        for (int i = 0; i < MAX_RETRIES; i++) {
            if (util_sys_inb(KBC_CMD_REG, &status) != 0) {
                break; // reading status failed, retry command
            }

            if (status & KBC_OBF) {
                // if there are communication errors, the byte is invalid
                if (status & (KBC_PAR_ERR | KBC_TO_ERR)) {
                    uint8_t trash;
                    util_sys_inb(KBC_OUT_BUF, &trash); // flush garbage and retry
                    break;
                }

                if (util_sys_inb(KBC_OUT_BUF, &ack_byte) != 0) {
                    break; // reading byte failed, retry command
                }

                if (ack_byte == MOUSE_ACK) return 0; // success
                if (ack_byte == MOUSE_NACK) break;   // nack, retry command
                
                // any other byte is considered an error, so retry command
                break;
            }
            tickdelay(micros_to_ticks(DELAY));
        }
        retries--;
    }
    return 1;
}

void (mouse_ih)() {
    uint8_t status;
    valid_byte = false; // reset flag at the beginning of each interrupt
    
    // read the status register (port 0x64)
    if (util_sys_inb(KBC_CMD_REG, &status) != 0) return;
    
    // check for errors and if there is data in the output buffer
    if (status & KBC_OBF) {
        // read the byte from the output buffer (port 0x60)
        if (util_sys_inb(KBC_OUT_BUF, &current_byte) != 0) return;
        
        // check that there are no parity or timeout errors, and that the data is from the mouse (kbc_aux)
        if (!(status & (KBC_PAR_ERR | KBC_TO_ERR)) && (status & KBC_AUX)) {
            valid_byte = true; // the byte is valid and belongs to the mouse
        }
    }
}

void parse_mouse_packet(struct packet *pp) {
    // copy the raw bytes to the struct
    for (int i = 0; i < 3; i++) {
        pp->bytes[i] = packet_bytes[i];
    }

    // extract buttons (bits 0, 1, and 2 of the first byte)
    pp->lb = pp->bytes[0] & BIT(0);
    pp->rb = pp->bytes[0] & BIT(1);
    pp->mb = pp->bytes[0] & BIT(2);

    // extract overflows (bits 6 and 7)
    pp->x_ov = pp->bytes[0] & BIT(6);
    pp->y_ov = pp->bytes[0] & BIT(7);

    // process delta x with sign extension
    if (pp->bytes[0] & BIT(4)) { // x sign bit is 1 (negative)
        pp->delta_x = pp->bytes[1] | 0xFF00; // fill upper 8 bits with 1s
    } else {
        pp->delta_x = pp->bytes[1]; // positive
    }

    // process delta y with sign extension
    if (pp->bytes[0] & BIT(5)) { // y sign bit is 1 (negative)
        pp->delta_y = pp->bytes[2] | 0xFF00;
    } else {
        pp->delta_y = pp->bytes[2];
    }
}

bool mouse_process_packet_byte() {
    // synchronization -> if it's the first byte, bit 3 must be 1
    if (byte_idx == 0 && !(current_byte & MOUSE_SYNC_BIT)) {
        return false; // desynchronized, ignore byte
    }

    packet_bytes[byte_idx] = current_byte;
    byte_idx++;

    if (byte_idx == MOUSE_PACKET_SIZE) {
        byte_idx = 0; // reset for the next packet
        return true;  // packet is complete
    }

    return false;
}
