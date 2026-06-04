#ifndef _UART_REG_H_
#define _UART_REG_H_

#include <lcom/lcf.h>

/* COM port base I/O addresses and IRQ lines */
#define COM1_BASE_ADDR  0x3F8
#define COM2_BASE_ADDR  0x2F8
#define COM1_IRQ        4
#define COM2_IRQ        3

/* Register offsets from the COM base address */
#define UART_RBR  0  /* Receiver Buffer Register       (read,  DLAB=0) */
#define UART_THR  0  /* Transmitter Holding Register   (write, DLAB=0) */
#define UART_IER  1  /* Interrupt Enable Register      (r/w,   DLAB=0) */
#define UART_DLL  0  /* Divisor Latch LSB              (r/w,   DLAB=1) */
#define UART_DLM  1  /* Divisor Latch MSB              (r/w,   DLAB=1) */
#define UART_IIR  2  /* Interrupt Identification Reg.  (read)          */
#define UART_FCR  2  /* FIFO Control Register          (write)         */
#define UART_LCR  3  /* Line Control Register          (r/w)           */
#define UART_MCR  4  /* Modem Control Register         (r/w)           */
#define UART_LSR  5  /* Line Status Register           (read)          */
#define UART_MSR  6  /* Modem Status Register          (read)          */
#define UART_SR   7  /* Scratchpad Register            (r/w)           */

/* LCR - bits per character (bits 1:0) */
#define UART_LCR_BITS_5       0x00
#define UART_LCR_BITS_6       0x01
#define UART_LCR_BITS_7       0x02
#define UART_LCR_BITS_8       0x03
#define UART_LCR_BITS_MASK    0x03

/* LCR - stop bits (bit 2) */
#define UART_LCR_STOP_1       0x00
#define UART_LCR_STOP_2       BIT(2)

/* LCR - parity (bits 5:3) */
#define UART_LCR_PAR_MASK     (BIT(3) | BIT(4) | BIT(5))
#define UART_LCR_NO_PARITY    0x00
#define UART_LCR_ODD_PARITY   BIT(3)
#define UART_LCR_EVEN_PARITY  (BIT(3) | BIT(4))

/* LCR - Divisor Latch Access Bit (bit 7) */
#define UART_LCR_DLAB         BIT(7)

/* LSR - Line Status Register bits */
#define UART_LSR_DR           BIT(0)  /* Data Ready - char available in RBR  */
#define UART_LSR_OE           BIT(1)  /* Overrun Error                        */
#define UART_LSR_PE           BIT(2)  /* Parity Error                         */
#define UART_LSR_FE           BIT(3)  /* Framing Error                        */
#define UART_LSR_BI           BIT(4)  /* Break Interrupt                      */
#define UART_LSR_THRE         BIT(5)  /* THR Empty - ready to transmit        */
#define UART_LSR_TEMT         BIT(6)  /* Transmitter Empty (THR + shift reg)  */
#define UART_LSR_FIFOE        BIT(7)  /* FIFO Error                           */
#define UART_LSR_ERR_MASK     (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE)

/* IER - Interrupt Enable Register bits */
#define UART_IER_RDA          BIT(0)  /* Received Data Available interrupt    */
#define UART_IER_THRE         BIT(1)  /* THR Empty interrupt                  */
#define UART_IER_RLS          BIT(2)  /* Receiver Line Status interrupt       */
#define UART_IER_MS           BIT(3)  /* Modem Status interrupt               */

/* IIR - Interrupt Identification Register bits */
#define UART_IIR_NO_INT       BIT(0)  /* 1 = no interrupt pending             */
#define UART_IIR_ID_MASK      0x0E    /* interrupt ID in bits 3:1             */
#define UART_IIR_RLS          0x06    /* Receiver Line Status (highest prio)  */
#define UART_IIR_RDA          0x04    /* Received Data Available              */
#define UART_IIR_CTO          0x0C    /* Character Timeout (FIFO mode)        */
#define UART_IIR_THRE         0x02    /* THR Empty                            */
#define UART_IIR_MS           0x00    /* Modem Status (lowest prio)           */

/* FCR - FIFO Control Register bits */
#define UART_FCR_ENABLE       BIT(0)  /* Enable both FIFOs                    */
#define UART_FCR_CLR_RX       BIT(1)  /* Clear Receiver FIFO (self-clearing)  */
#define UART_FCR_CLR_TX       BIT(2)  /* Clear Transmitter FIFO (self-clear.) */
#define UART_FCR_TRIG_1       0x00    /* RX FIFO trigger at 1 byte            */
#define UART_FCR_TRIG_4       BIT(6)
#define UART_FCR_TRIG_8       BIT(7)
#define UART_FCR_TRIG_14      (BIT(6) | BIT(7))

/* Baud rate: divisor = UART_CLOCK / baud_rate; DLL = lo byte, DLM = hi byte */
#define UART_CLOCK            115200

#endif /* _UART_REG_H_ */
