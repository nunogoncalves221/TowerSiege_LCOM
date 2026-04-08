#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

#define IRQ_KEYBOARD 1 // hhe keyboard's IRQ line is always 1

/* for kbd_test_scan and kbc_ih */
#define KBC_OUT_BUF   0x60    // port of Output Buffer
#define KBC_CMD_REG   0x64    // port of Status/Command Register

#define KBC_OBF       BIT(0)  // Bit 0: Output Buffer Full
#define KBC_PAR_ERR   BIT(7)  // Bit 7: Parity Error
#define KBC_TO_ERR    BIT(6)  // Bit 6: Timeout Error

#endif /* _LCOM_I8042_H_ */
