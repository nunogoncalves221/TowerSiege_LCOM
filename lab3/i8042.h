#ifndef I8042_H
#define I8042_H

#define KBD_IRQ 1

#define KBC_ST_REG   0x64
#define KBC_CMD_REG  0x64
#define KBC_OUT_BUF  0x60
#define KBC_IN_BUF   0x60

#define OBF     BIT(0)
#define IBF     BIT(1)
#define AUX     BIT(5)
#define TO_ERR  BIT(6)
#define PAR_ERR BIT(7)

#define READ_CMD_BYTE   0x20
#define WRITE_CMD_BYTE  0x60

#define KBD_INT BIT(0)

#define DELAY_US 20000
#define RETRIES  10

#endif
