#include <lcom/lcf.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "uart_reg.h"
#include "uart.h"
#include "../lab2/utils.h"

// Configuration

static uint16_t com_base(uint8_t com) {
  return (com == 1) ? COM1_BASE_ADDR : COM2_BASE_ADDR;
}

int uart_get_config(uint8_t com, uart_config_t *cfg) {
  if (cfg == NULL) return 1;
  uint16_t base = com_base(com);
  uint8_t lcr, ier, dll, dlm;

  if (util_sys_inb(base + UART_LCR, &lcr) != 0) return 1;
  if (util_sys_inb(base + UART_IER, &ier) != 0) return 1;

  // bits per char
  switch (lcr & UART_LCR_BITS_MASK) {
    case UART_LCR_BITS_5: cfg->bits = 5; break;
    case UART_LCR_BITS_6: cfg->bits = 6; break;
    case UART_LCR_BITS_7: cfg->bits = 7; break;
    default:              cfg->bits = 8; break;
  }

  // stop bits
  cfg->stop = (lcr & UART_LCR_STOP_2) ? 2 : 1;

  // parity
  if (!(lcr & BIT(3))) {
    cfg->parity = 0; // none
  } else if (lcr & BIT(4)) {
    cfg->parity = 2; // even
  } else {
    cfg->parity = 1; // odd
  }

  // baud rate via divisor latches
  if (sys_outb(base + UART_LCR, lcr | UART_LCR_DLAB) != 0) return 1;
  if (util_sys_inb(base + UART_DLL, &dll) != 0) { sys_outb(base + UART_LCR, lcr); return 1; }
  if (util_sys_inb(base + UART_DLM, &dlm) != 0) { sys_outb(base + UART_LCR, lcr); return 1; }
  if (sys_outb(base + UART_LCR, lcr) != 0) return 1; // restore DLAB=0

  uint16_t divisor = ((uint16_t)dlm << 8) | dll;
  cfg->baud_rate = (divisor == 0) ? 0 : UART_CLOCK / divisor;

  cfg->lcr = lcr;
  cfg->ier = ier;
  return 0;
}

int uart_set_config(uint8_t com, uint8_t bits, uint8_t stop, uint8_t parity, uint32_t rate) {
  uint16_t base = com_base(com);
  uint8_t lcr = 0;

  // bits per char
  switch (bits) {
    case 5: lcr |= UART_LCR_BITS_5; break;
    case 6: lcr |= UART_LCR_BITS_6; break;
    case 7: lcr |= UART_LCR_BITS_7; break;
    case 8: lcr |= UART_LCR_BITS_8; break;
    default: return 1;
  }

  // stop bits
  if (stop == 2) lcr |= UART_LCR_STOP_2;
  else if (stop != 1) return 1;

  // parity
  switch (parity) {
    case 0: break;                             // none
    case 1: lcr |= UART_LCR_ODD_PARITY;  break;
    case 2: lcr |= UART_LCR_EVEN_PARITY; break;
    default: return 1;
  }

  // set baud rate via divisor latches
  if (rate == 0) return 1;
  uint16_t divisor = UART_CLOCK / rate;
  uint8_t dll = divisor & 0xFF;
  uint8_t dlm = (divisor >> 8) & 0xFF;

  if (sys_outb(base + UART_LCR, lcr | UART_LCR_DLAB) != 0) return 1;
  if (sys_outb(base + UART_DLL, dll) != 0) { sys_outb(base + UART_LCR, lcr); return 1; }
  if (sys_outb(base + UART_DLM, dlm) != 0) { sys_outb(base + UART_LCR, lcr); return 1; }

  // write LCR with DLAB cleared
  if (sys_outb(base + UART_LCR, lcr) != 0) return 1;
  return 0;
}

void uart_print_config(const uart_config_t *cfg) {
  static const char *par_names[] = { "none", "odd", "even" };
  printf("  Bits/char : %u\n", cfg->bits);
  printf("  Stop bits : %u\n", cfg->stop);
  printf("  Parity    : %s\n", (cfg->parity <= 2) ? par_names[cfg->parity] : "unknown");
  printf("  Baud rate : %lu bps\n", (unsigned long)cfg->baud_rate);
  printf("  IER raw   : 0x%02X", cfg->ier);
  if (cfg->ier & UART_IER_RDA)  printf(" [RDA]");
  if (cfg->ier & UART_IER_THRE) printf(" [THRE]");
  if (cfg->ier & UART_IER_RLS)  printf(" [RLS]");
  if (cfg->ier & UART_IER_MS)   printf(" [MS]");
  printf("\n");
}

// Polled I/O

int uart_poll_recv(uint8_t com, uint8_t *c) {
  uint16_t base = com_base(com);
  uint8_t lsr;
  if (util_sys_inb(base + UART_LSR, &lsr) != 0) return 1;
  if (!(lsr & UART_LSR_DR)) return 1; // no data yet
  return util_sys_inb(base + UART_RBR, c);
}

int uart_poll_send(uint8_t com, uint8_t c) {
  uint16_t base = com_base(com);
  uint8_t lsr;
  int retries = 10000;

  do {
    if (util_sys_inb(base + UART_LSR, &lsr) != 0) return 1;
    if (lsr & UART_LSR_THRE) break;
    tickdelay(micros_to_ticks(10));
  } while (--retries > 0);

  if (!(lsr & UART_LSR_THRE)) return 1; // timeout
  return sys_outb(base + UART_THR, c);
}
