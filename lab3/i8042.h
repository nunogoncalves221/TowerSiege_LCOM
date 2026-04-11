#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

#define IRQ_KEYBOARD 1 // hhe keyboard's IRQ line is always 1

/* FOR kbd_test_scan and kbc_ih */

#define KBC_OUT_BUF   0x60    // port of Output Buffer
#define KBC_CMD_REG   0x64    // port of Status/Command Register

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

//KBC command byte
#define ENABLE_INTERRUPTS_KEYBOARD BIT(0)

//polling constants
#define DELAY 20000
#define MAX_RETRIES 10
#define KBC_IBF BIT(1)


#endif /* _LCOM_I8042_H_ */
