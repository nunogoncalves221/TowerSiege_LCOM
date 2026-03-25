#include <lcom/lcf.h>
#include <lcom/lab2.h>
#include "my_timer.h"

#include <stdbool.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
  // set language for LCF messages
  lcf_set_language("EN-US");

  // enable function call tracing (for debugging)
  lcf_trace_calls("/home/lcom/labs/grupo_2leic16_2/lab2/trace.txt");

  // redirect printf output to a file
  lcf_log_output("/home/lcom/labs/grupo_2leic16_2/lab2/output.txt");

  // start LCF (handles arguments and calls the correct function)
  if (lcf_start(argc, argv))
    return 1;

  // cleanup before exiting
  lcf_cleanup();

  return 0;
}

int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {

  uint8_t st; // variable to store timer status byte

  // read timer configuration
  if (timer_get_conf(timer, &st) != 0) {
    printf("timer_get_conf error.\n");
    return 1;
  }

  // display selected configuration field
  if (timer_display_conf(timer, st, field) != 0) {
    printf("timer_display_conf error.\n");
    return 1;
  }

  return 0;
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {

  // set timer frequency
  if (timer_set_frequency(timer, freq) != 0) {
    printf("error calling timer_set_frequency.\n");
    return 1;
  }

  return 0;
}

int(timer_test_int)(uint8_t time) {
  int ipc_status, r;
  message msg;
  uint8_t bit_no;

  // subscribe timer interrupts
  if (timer_subscribe_int(&bit_no) != 0) {
    printf("timer_subscribe_int error.\n");
    return 1;
  }

  // bit mask for this interrupt
  uint32_t irq_set = BIT(bit_no);

  // reset internal interrupt counter
  reset_timer_counter();

  // loop until desired time (seconds * 60 interrupts per second)
  while (get_timer_counter() < time * 60) {

    // receive message from kernel
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d\n", r);
      continue;
    }

    // check if message is a notification
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {

        case HARDWARE: // hardware interrupt

          // check if it's the timer interrupt
          if (msg.m_notify.interrupts & irq_set) {

            // handle interrupt (increment counter)
            timer_int_handler();

            // every 60 interrupts = 1 second
            if (get_timer_counter() % 60 == 0) {
              timer_print_elapsed_time();
            }
          }
          break;
        default:
          // ignore other notifications
          break;
      }
    }
  }

  // unsubscribe interrupts
  if (timer_unsubscribe_int() != 0) {
    printf("timer_unsubscribe_int error.\n");
    return 1;
  }

  return 0;
}
