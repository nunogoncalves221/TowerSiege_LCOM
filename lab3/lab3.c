#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include "../lab2/utils.c"
#include "keyboard.h"

#include <stdbool.h>
#include <stdint.h>

static uint8_t scancode_byte = 0;
static bool kbc_error = false;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

void (kbc_ih)(void) {
  uint8_t status;
  uint8_t data;

  kbc_error = false;

  if (util_sys_inb(KBC_ST_REG, &status) != 0) {
    kbc_error = true;
    return;
  }

  if (status & OBF) {
    if (util_sys_inb(KBC_OUT_BUF, &data) != 0) {
      kbc_error = true;
      return;
    }

    if ((status & (PAR_ERR | TO_ERR)) || (status & AUX)) {
      kbc_error = true;
      return;
    }

    scancode_byte = data;
  }
  else {
    kbc_error = true;
  }
}

int(kbd_test_scan)() {
  uint8_t bit_no;
  uint32_t irq_set;
  int ipc_status, r;
  message msg;

  uint8_t bytes[2];
  uint8_t size = 0;
  bool two_bytes = false;
  bool make;
  bool done = false;

  if (keyboard_subscribe_int(&bit_no) != 0) {
    return 1;
  }

  irq_set = BIT(bit_no);

  while (!done) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d\n", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            kbc_ih();

            if (kbc_error) continue;

            if (scancode_byte == 0xE0) {
              bytes[0] = scancode_byte;
              two_bytes = true;
              continue;
            }

            if (two_bytes) {
              bytes[1] = scancode_byte;
              size = 2;
              make = !(bytes[1] & BIT(7));
              two_bytes = false;
            }
            else {
              bytes[0] = scancode_byte;
              size = 1;
              make = !(bytes[0] & BIT(7));
            }

            if (kbd_print_scancode(make, size, bytes) != 0) {
              keyboard_unsubscribe_int();
              return 1;
            }

            if (size == 1 && bytes[0] == 0x81) {
              done = true;
            }
          }
          break;
        default:
          break;
      }
    }
  }

  if (keyboard_unsubscribe_int() != 0) {
    return 1;
  }

  return 0;
}


int(kbd_test_poll)() {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int(kbd_test_timed_scan)(uint8_t n) {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}
