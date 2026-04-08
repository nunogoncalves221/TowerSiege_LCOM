#include <lcom/lcf.h>

#include <lcom/lab3.h>
#include "kbc.h"


#include <stdbool.h>
#include <stdint.h>

extern uint8_t scancode;
extern bool ih_error;

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
  int ipc_status, r;
  message msg;
  uint8_t bit_no;

  // subscribe interruptions (IRQ 1)
  // kbd_subscribe_int uses kbd_hook_id defined by 1 in kbc.c
  if (kbd_subscribe_int(&bit_no) != 0) return 1;

  // create the bit mask in order to identify keyboard's interruption
  uint32_t irq_set = BIT(bit_no);

  uint8_t scancodes[2];
  uint8_t size = 0;

  // loop runs until we read breakcode of ESC (0x81)
  while (scancode != 0x81) {
    
    /* get a request message */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */
                if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
                    
                    //call our interrupt handler (implementation in kbc.c)
                    kbc_ih();

                    //if exists parity or timeout errors (ih_error), we ignore the byte
                    if (ih_error) {
                        size = 0;
                        continue;
                    }

                    // logic to process scancodes of 1 or 2 bytes
                    scancodes[size] = scancode;
                    size++;

                    // if the prefix is 0xE0, we dont print it now, we have to wait for the next byte
                    if (scancode == 0xE0 && size == 1) {
                        continue; 
                    }

                    // if we are here, the scancode is complete (of 1 or 2 bytes)
                    bool is_make = !(scancodes[size - 1] & BIT(7));
                    kbd_print_scancode(is_make, size, scancodes);

                    //reset of the size for the next scancode
                    size = 0;
                }
                break;
            default:
                break;
        }
    }
  }

  // unsubscribe before leave
  if (kbd_unsubscribe_int() != 0) return 1;

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
