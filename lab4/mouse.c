#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "i8042.h"

/* for mouse_ih() */
static uint8_t packet_bytes[3]; // stores the 3 bytes of the current packet
static int byte_idx = 0;        // keeps track of which byte we are reading (0, 1, or 2)
static uint8_t current_byte;    // stores the most recently read byte from the KBC
static bool valid_byte = false; // extra flag to know if the read was valid

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
    uint8_t ack_byte = 0;
    int retries = MAX_RETRIES;

    while (retries > 0) {
        // tell kbc we're writing to the mouse
        if (kbc_write(KBC_CMD_REG, WRITE_TO_MOUSE) != 0) return 1;

        // write the command itself
        if (kbc_write(KBC_OUT_BUF, cmd) != 0) return 1;

        // wait for and read acknowledgement from the mouse
        int ack_retries = MAX_RETRIES;
        bool ack_read = false;
        while (ack_retries > 0) {
            uint8_t status;
            if (util_sys_inb(KBC_CMD_REG, &status) != 0) return 1;

            // if there's data in the output buffer, read it
            if (status & KBC_OBF) {
                // check for communication errors before reading the byte
                if (status & (KBC_PAR_ERR | KBC_TO_ERR)) return 1;

                if (util_sys_inb(KBC_OUT_BUF, &ack_byte) != 0) return 1;
                ack_read = true;
                break; // exit the wait loop
            }
            tickdelay(micros_to_ticks(DELAY));
            ack_retries--;
        }

        if (!ack_read) return 1; // timed out waiting for acknowledgement

        if (ack_byte == MOUSE_ACK) {
            return 0; // success
        }
        
        if (ack_byte == MOUSE_NACK || ack_byte == MOUSE_ERROR) {
            retries--;
            continue; // retry command on nack
        }

        return 1; // mouse error or unexpected byte
    }
    return 1; // failed after all retries
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
