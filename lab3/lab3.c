#include <lcom/lcf.h>
#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include "keyboard.h"
#include "i8042.h"

#include "../lab2/my_timer.h"

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


int (kbd_test_scan)() {
  // variables for interrupt handling
  uint8_t bit_no;
  uint32_t irq_set;
  int ipc_status, r;
  message msg;

  // subscribe keyboard interrupts
  if (keyboard_subscribe_int(&bit_no) != 0) return 1;
  irq_set = BIT(bit_no);

  // loop until ESC breakcode is received
  while (scancode_byte != ESC_BREAKCODE) {
    // receive messages from the system
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d\n", r);
      continue;
    }

    // check if it is a notification
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          // check if it is keyboard interrupt
          if (msg.m_notify.interrupts & irq_set) {
            kbc_ih(); // read scancode from keyboard

            // ignore if error occurred and reset size to drop bad bytes
            if (kbc_error) {
              scancode_size = 0;
              continue;
            }

            // check if the scancode is fully assembled
            if (kbd_process_scancode(&scancode_size, scancode_bytes)) {
              bool make = !(scancode_bytes[scancode_size - 1] & BIT(7));
              
              if (kbd_print_scancode(make, scancode_size, scancode_bytes) != 0) {
                keyboard_unsubscribe_int();
                return 1;
              }
              
              // reset size for the next key press
              scancode_size = 0;
            }
          }
          break;
        default:
          break;
      }
    }
  }

  // unsubscribe interrupts
  if (keyboard_unsubscribe_int() != 0) return 1;

  if (kbd_print_no_sysinb(kbd_get_sys_inb_count()) != 0) return 1;
  return 0;
}

int (kbd_test_poll)() {
  // buffer to store scancode bytes
  uint8_t bytes[2];
  uint8_t size = 0;

  // reset sys_inb counter
  kbd_reset_sys_inb_count();

  // initialize scancode
  scancode_byte = 0;

  // loop until ESC breakcode
  while (scancode_byte != ESC_BREAKCODE) {
    // read keyboard using polling
    if (kbd_poll() != 0) continue;

    // check if the scancode is fully assembled
    if (kbd_process_scancode(&size, bytes)) {
      // check if make or break
      bool make = !(bytes[size - 1] & BIT(7));

      // print scancode
      if (kbd_print_scancode(make, size, bytes) != 0) return 1;

      // reset buffer
      size = 0;
  }
}
  // re-enable interrupts and print sys_inb count
  if (kbd_enable_interrupts() != 0) return 1;
  if (kbd_print_no_sysinb(kbd_get_sys_inb_count()) != 0) return 1;

  return 0;

}

int (kbd_test_timed_scan)(uint8_t n) {
  // variables for keyboard and timer interrupts
  uint8_t kbd_bit_no, timer_bit_no;
  int ipc_status, r;
  message msg;

  // subscribe keyboard and timer interrupts
  if (keyboard_subscribe_int(&kbd_bit_no) != 0) return 1;
  if (timer_subscribe_int(&timer_bit_no)  != 0) { keyboard_unsubscribe_int(); return 1; }

  // create interrupt masks
  uint32_t irq_set_kbd   = BIT(kbd_bit_no);
  uint32_t irq_set_timer = BIT(timer_bit_no);

  // buffer for scancodes
  int tick_count = 0;
  int idle_seconds = 0;

  scancode_byte = 0;

  // loop until ESC or timeout
  while (scancode_byte != ESC_BREAKCODE && idle_seconds < n) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) continue;

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:

          // keyboard interrupt
          if (msg.m_notify.interrupts & irq_set_kbd) {
            kbc_ih();

            // ignore errors and reset size to drop bad bytes
            if (kbc_error) {
              scancode_size = 0;
              continue;
            }

            // check if the scancode is fully assembled
            if (kbd_process_scancode(&scancode_size, scancode_bytes)) {
              bool make = !(scancode_bytes[scancode_size - 1] & BIT(7));
              
              if (kbd_print_scancode(make, scancode_size, scancode_bytes) != 0) {
                timer_unsubscribe_int();
                keyboard_unsubscribe_int();
                return 1;
              }
              
              // reset size for the next key press
              scancode_size = 0;
            }

            // reset idle timer on valid key press
            tick_count   = 0;
            idle_seconds = 0;
          }

          // timer interrupt
          if (msg.m_notify.interrupts & irq_set_timer) {
            timer_int_handler();
            tick_count++;

            // 60 ticks = 1 second
            if (tick_count % 60 == 0) idle_seconds++;
          }

          break;
        default:
          break;
      }
    }
  }

  // unsubscribe interrupts
  if (timer_unsubscribe_int()    != 0) return 1;
  if (keyboard_unsubscribe_int() != 0) return 1;
  if (kbd_print_no_sysinb(kbd_get_sys_inb_count()) != 0) return 1;
  return 0;
}
