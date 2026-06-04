#ifndef _UART_H_
#define _UART_H_

#include <lcom/lcf.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
  uint8_t  bits;
  uint8_t  stop;
  uint8_t  parity;    // 0=none, 1=odd, 2=even
  uint32_t baud_rate;
  uint8_t  ier;
  uint8_t  lcr;
} uart_config_t;

int  uart_get_config(uint8_t com, uart_config_t *cfg);
int  uart_set_config(uint8_t com, uint8_t bits, uint8_t stop, uint8_t parity, uint32_t rate);
void uart_print_config(const uart_config_t *cfg);

int  uart_poll_recv(uint8_t com, uint8_t *c);
int  uart_poll_send(uint8_t com, uint8_t c);

#endif
