#include <lcom/lcf.h>
#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include "keyboard.h"
#include "i8042.h"

#include "../lab2/my_timer.h"

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");
  lcf_log_output("/home/lcom/labs/lab3/output.txt");
  if (lcf_start(argc, argv)) return 1;
  lcf_cleanup();
  return 0;
}

int (kbd_test_scan)() {
  uint8_t bit_no;
  uint32_t irq_set;
  int ipc_status, r;
  message msg;

  uint8_t bytes[2];
  uint8_t size = 0;

  if (keyboard_subscribe_int(&bit_no) != 0) return 1;
  irq_set = BIT(bit_no);

  while (scancode_byte != 0x81) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d\n", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            kbc_ih();

            if (kbc_error) { size = 0; continue; }

            bytes[size] = scancode_byte;
            size++;

            /* wait for the second byte of a two-byte scancode */
            if (scancode_byte == 0xE0 && size == 1) continue;

            bool make = !(bytes[size - 1] & BIT(7));
            if (kbd_print_scancode(make, size, bytes) != 0) {
              keyboard_unsubscribe_int();
              return 1;
            }
            size = 0;
          }
          break;
        default:
          break;
      }
    }
  }

  if (keyboard_unsubscribe_int() != 0) return 1;
  return 0;
}

int (kbd_test_poll)() {
  uint8_t bytes[2];
  uint8_t size = 0;

  kbd_reset_sys_inb_count();
  scancode_byte = 0;

  while (scancode_byte != 0x81) {
    if (kbd_poll() != 0) continue;

    bytes[size] = scancode_byte;
    size++;

    /* wait for the second byte of a two-byte scancode */
    if (scancode_byte == 0xE0 && size == 1) continue;

    bool make = !(bytes[size - 1] & BIT(7));
    if (kbd_print_scancode(make, size, bytes) != 0) return 1;
    size = 0;
  }

  /* re-enable interrupts and print the total number of sys_inb calls */
  if (kbd_enable_interrupts() != 0) return 1;
  if (kbd_print_no_sysinb(kbd_get_sys_inb_count()) != 0) return 1;

  return 0;
}

int (kbd_test_timed_scan)(uint8_t n) {
  uint8_t kbd_bit_no, timer_bit_no;
  int ipc_status, r;
  message msg;

  if (keyboard_subscribe_int(&kbd_bit_no) != 0) return 1;
  if (timer_subscribe_int(&timer_bit_no)  != 0) { keyboard_unsubscribe_int(); return 1; }

  uint32_t irq_set_kbd   = BIT(kbd_bit_no);
  uint32_t irq_set_timer = BIT(timer_bit_no);

  uint8_t bytes[2];
  uint8_t size         = 0;
  int     tick_count   = 0;
  int     idle_seconds = 0;

  scancode_byte = 0;

  while (scancode_byte != 0x81 && idle_seconds < n) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) continue;

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:

          if (msg.m_notify.interrupts & irq_set_kbd) {
            kbc_ih();
            if (kbc_error) { size = 0; continue; }

            bytes[size] = scancode_byte;
            size++;

            if (scancode_byte == 0xE0 && size == 1) continue;

            bool make = !(bytes[size - 1] & BIT(7));
            if (kbd_print_scancode(make, size, bytes) != 0) {
              timer_unsubscribe_int();
              keyboard_unsubscribe_int();
              return 1;
            }
            size = 0;

            /* any keypress resets the idle timer */
            tick_count   = 0;
            idle_seconds = 0;
          }

          if (msg.m_notify.interrupts & irq_set_timer) {
            timer_int_handler();
            tick_count++;
            /* 60 ticks = 1 second */
            if (tick_count % 60 == 0) idle_seconds++;
          }

          break;
        default:
          break;
      }
    }
  }

  if (timer_unsubscribe_int()    != 0) return 1;
  if (keyboard_unsubscribe_int() != 0) return 1;
  return 0;
}
