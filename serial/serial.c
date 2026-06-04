#include <lcom/lcf.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "uart_reg.h"
#include "uart.h"

// forward declarations
int (ser_test_conf)(unsigned short com);
int (ser_test_set)(unsigned short com, unsigned long bits, unsigned short stop, unsigned short parity, unsigned long rate);
int (ser_test_poll)(unsigned short com, bool rx, unsigned char *s, int len);
int (ser_test_int)(unsigned short com, bool rx, unsigned char *s, int len);
int (ser_test_fifo)(unsigned short com, bool rx, unsigned char *s, int len);

int (proj_main_loop)(int argc, char *argv[]) {
  if (argc < 1) { printf("Usage: conf|set|poll|int|fifo ...\n"); return 1; }

  if (strcmp(argv[0], "conf") == 0 && argc >= 2)
    return ser_test_conf((unsigned short)atoi(argv[1]));

  if (strcmp(argv[0], "set") == 0 && argc >= 6)
    return ser_test_set((unsigned short)atoi(argv[1]), (unsigned long)atol(argv[2]),
                        (unsigned short)atoi(argv[3]), (unsigned short)atoi(argv[4]),
                        (unsigned long)atol(argv[5]));

  if (strcmp(argv[0], "poll") == 0 && argc >= 5)
    return ser_test_poll((unsigned short)atoi(argv[1]), (bool)atoi(argv[2]),
                         (unsigned char *)argv[3], atoi(argv[4]));

  if (strcmp(argv[0], "int") == 0 && argc >= 5)
    return ser_test_int((unsigned short)atoi(argv[1]), (bool)atoi(argv[2]),
                        (unsigned char *)argv[3], atoi(argv[4]));

  if (strcmp(argv[0], "fifo") == 0 && argc >= 5)
    return ser_test_fifo((unsigned short)atoi(argv[1]), (bool)atoi(argv[2]),
                         (unsigned char *)argv[3], atoi(argv[4]));

  printf("Unknown command: %s\n", argv[0]);
  return 1;
}

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");
if (lcf_start(argc, argv))
    return 1;

  lcf_cleanup();
  return 0;
}

// ser_test_conf

int (ser_test_conf)(unsigned short com) {
  if (com < 1 || com > 2) return 1;

  uart_config_t cfg;
  if (uart_get_config((uint8_t)com, &cfg) != 0) {
    printf("ser_test_conf: failed to read UART config\n");
    return 1;
  }

  printf("COM%u configuration:\n", com);
  uart_print_config(&cfg);
  return 0;
}

// ser_test_set

// parity: 0=none, 1=odd, 2=even
int (ser_test_set)(unsigned short com, unsigned long bits, unsigned short stop,
                   unsigned short parity, unsigned long rate) {
  if (com < 1 || com > 2) return 1;
  if (bits < 5 || bits > 8) return 1;
  if (stop < 1 || stop > 2) return 1;
  if (parity > 2) return 1;

  if (uart_set_config((uint8_t)com, (uint8_t)bits, (uint8_t)stop,
                      (uint8_t)parity, (uint32_t)rate) != 0) {
    printf("ser_test_set: failed to configure UART\n");
    return 1;
  }

  printf("COM%u configured.\n", com);
  return ser_test_conf(com);
}

// ser_test_poll

// Polled mode send/receive.
//  rx=true  -> receive 'len' characters and print them
//  rx=false -> transmit the string 's' (first 'len' bytes)
//
// Minix has a default IH for the serial port; disabling UART interrupts
// (IER=0) before polling prevents interference.
int (ser_test_poll)(unsigned short com, bool rx, unsigned char *s, int len) {
  if (com < 1 || com > 2) return 1;
  uint16_t base = (com == 1) ? COM1_BASE_ADDR : COM2_BASE_ADDR;

  // disable UART interrupts for polled operation
  uint8_t saved_ier;
  if (util_sys_inb(base + UART_IER, &saved_ier) != 0) return 1;
  if (sys_outb(base + UART_IER, 0x00) != 0) return 1;

  int ret = 0;

  if (rx) {
    printf("Polling COM%u for %d character(s)...\n", com, len);
    int received = 0;
    while (received < len) {
      uint8_t c;
      if (uart_poll_recv((uint8_t)com, &c) == 0) {
        printf("rx[%d]: 0x%02X ('%c')\n", received, c, (c >= 32 && c < 127) ? c : '.');
        received++;
      } else {
        tickdelay(micros_to_ticks(1000));
      }
    }
  } else {
    if (s == NULL) { ret = 1; goto restore; }
    printf("Sending %d byte(s) via COM%u (polled)...\n", len, com);
    for (int i = 0; i < len; i++) {
      if (uart_poll_send((uint8_t)com, s[i]) != 0) {
        printf("ser_test_poll: send failed at byte %d\n", i);
        ret = 1;
        break;
      }
    }
    if (ret == 0) printf("Done.\n");
  }

restore:
  sys_outb(base + UART_IER, saved_ier);
  return ret;
}

// ser_test_int

int (ser_test_int)(unsigned short com, bool rx, unsigned char *s, int len) {
  (void)com; (void)rx; (void)s; (void)len;
  return 0;
}

// ser_test_fifo

int (ser_test_fifo)(unsigned short com, bool rx, unsigned char *s, int len) {
  (void)com; (void)rx; (void)s; (void)len;
  return 0;
}
