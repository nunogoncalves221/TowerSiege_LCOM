#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "i8042.h"


int mouse_write_command(uint8_t cmd) {
    uint8_t status;
    uint8_t ack_byte;
    int retries;

    // tell the KBC that we want to send a byte to the mouse
    // try this for a limited number of times before giving up
    retries = MAX_RETRIES;
    while (retries > 0) {
        // read the KBC status register
        if (util_sys_inb(KBC_CMD_REG, &status) != 0) return 1; 
        

        // only write if the KBC's input buffer is not full
        if ((status & KBC_IBF) == 0) {
            // buffer is clear, send the command
            if (sys_outb(KBC_CMD_REG, WRITE_TO_MOUSE) == 0) {
                break; // command sent successfully, exit the loop
            } else {
                return 1; // error handling
            }
        }

        // buffer was full, wait DELAY timw and try again.
        tickdelay(micros_to_ticks(DELAY));
        retries--;
    }
    if (retries == 0) return 1; // timed out

    // now, send the actual command byte to the mouse
    retries = MAX_RETRIES;
    while (retries > 0) {
        // need to check the status again.
        if (util_sys_inb(KBC_CMD_REG, &status) != 0) {
            return 1;
        }

        // wait again for the input buffer to be clear
        if ((status & KBC_IBF) == 0) {
            // buffer is clear, send our command argument to the data port
            if (sys_outb(KBC_OUT_BUF, cmd) == 0) {
                break; // argument sent, exit loop
            } else {
                return 1;
            }
        }
        tickdelay(micros_to_ticks(DELAY));
        retries--;
    }
    if (retries == 0) return 1; // timed out again

    // finally, let's get the acknowledgement from the mouse
    // don't need a retry loop here because the KBC will wait for the mouse
    // however, reading the response from the mouse might require a retry
    if (util_sys_inb(KBC_OUT_BUF, &ack_byte) != 0) return 1;

    if (ack_byte == MOUSE_ACK) return 0; // all good
    
    return 1; // NACK, ERROR or timeout
}
