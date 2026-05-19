#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

#define IRQ_KEYBOARD 1 // hhe keyboard's IRQ line is always 1
#define IRQ_MOUSE 12   // the mouse's irq line is always 12

/* FOR kbd_test_scan and kbc_ih */

#define KBC_OUT_BUF   0x60    // port of Output Buffer
#define KBC_IN_BUF    0x60 // port to write arguments to the KBC
#define KBC_CMD_REG   0x64    // port of Status/Command Register

#define KBC_AUX       BIT(5)  // bit 5: mouse data
#define KBC_OBF       BIT(0)  // Bit 0: Output Buffer Full
#define KBC_PAR_ERR   BIT(7)  // Bit 7: Parity Error
#define KBC_TO_ERR    BIT(6)  // Bit 6: Timeout Error

/* FOR kbd_test_poll and kbd_poll */

//KBC commands
#define READ_COMMAND_BYTE 0x20
#define WRITE_COMMAND_BYTE 0x60
#define CHECK_KBC 0xAA 
#define CHECK_KEYBOARD_INTERFACE 0xAB
#define DISABLE_KBD_INTERFACE 0xAD
#define ENABLE_KDB_INTERFACE 0xAE

//mouse commands
#define WRITE_TO_MOUSE 0xD4 // write byte to mouse
#define ENABLE_DATA_REPORTING 0xF4 // enable data reporting
#define DISABLE_DATA_REPORTING 0xF5 // disable data reporting

//mouse acknowledgments
#define MOUSE_ACK 0xFA // acknowledged
#define MOUSE_NACK 0xFE // not acknowledged
#define MOUSE_ERROR 0xFC // error

// mouse packet byte 1 bits
#define MOUSE_LB        BIT(0) // left button
#define MOUSE_RB        BIT(1) // right button
#define MOUSE_MB        BIT(2) // middle button
#define MOUSE_SYNC_BIT  BIT(3) // always 1 on first byte
#define MOUSE_X_SIGN    BIT(4) // X delta sign
#define MOUSE_Y_SIGN    BIT(5) // Y delta sign
#define MOUSE_X_OVF     BIT(6) // X delta overflow
#define MOUSE_Y_OVF     BIT(7) // Y delta overflow

// scancodes
#define ESC_BREAKCODE 0x81

//KBC command byte
#define ENABLE_INTERRUPTS_KEYBOARD BIT(0)

//polling constants
#define DELAY 20000
#define MAX_RETRIES 10
#define KBC_IBF BIT(1)
#define MOUSE_PACKET_SIZE 3

// timer constants
#define TIMER_FREQ 60 // default timer frequency

#endif /* _LCOM_I8042_H_ */
