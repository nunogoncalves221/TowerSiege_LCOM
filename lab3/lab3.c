#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include "kbc.h"

#include <stdbool.h>
#include <stdint.h>

extern uint8_t scancode;
extern bool ih_error;

// acess the timer counter from lab2
extern int timer_counter;

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
                    if (scancode == 0xE0 && size == 1) continue; 
                    

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
  uint8_t scancodes[2];
  uint8_t size = 0;

   // loop runs until we read breakcode of ESC (0x81)
  while (scancode != 0x81){

    //call kbd_poll function in kbc.c for  polling
    if (kbd_poll() == 0){

      // logic to process scancodes of 1 or 2 bytes
      scancodes[size] = scancode;
      size++;

      // if the prefix is 0xE0, we dont print it now, we have to wait for the next byte
      if (scancode == 0XE0 && size == 1) continue;

      // if we are here, the scancode is complete (of 1 or 2 bytes)
      bool is_make = !(scancodes[size - 1] & BIT(7));
      kbd_print_scancode(is_make, size, scancodes);

      //reset of the size for the next scancode
      size = 0;
    }
  }

  //reactivate interrupts before leave
  return kbd_restore_interrupts();
}

int(kbd_test_timed_scan)(uint8_t n) {
  int ipc_status, r;
  message msg;
  uint8_t kbd_bit_no, timer_bit_no;

  // subscribe both hardware devices
  if (kbd_subscribe_int(&kbd_bit_no) != 0) return 1;
  if (timer_subscribe_int(&timer_bit_no) != 0) return 1;

  // create the bit masks for both interruptions
  uint32_t irq_set_kbd = BIT(kbd_bit_no);
  uint32_t irq_set_timer = BIT(timer_bit_no);

  uint8_t scancodes[2];
  uint8_t size = 0;

  // we need to track how many seconds have passed without any key presses
  int idle_seconds = 0; 
  timer_counter = 0; // reset the global counter from lab2 

  // loop runs until ESC is pressed OR we reach the idle time limit 'n'
    while (scancode != 0x81 && idle_seconds < n) {
        
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) continue;
        
        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: 
                    
                    // keyboard interrupt zone
                    if (msg.m_notify.interrupts & irq_set_kbd) {
                        
                        kbc_ih();

                        if (ih_error) {
                            size = 0;
                            continue;
                        }

                        scancodes[size] = scancode;
                        size++;

                        if (scancode == 0xE0 && size == 1) continue; 

                        bool is_make = !(scancodes[size - 1] & BIT(7));
                        kbd_print_scancode(is_make, size, scancodes);
                        size = 0;

                        // the user pressed a key,
                        // so we reset our idle timers back to zero
                        timer_counter = 0;
                        idle_seconds = 0;
                    }

                    // timer interrupt zone
                    if (msg.m_notify.interrupts & irq_set_timer) {
                        
                        // call the handler from lab2 to increment the global counter
                        timer_int_handler();

                        // since the timer runs at 60Hz, 60 ticks = 1 second
                        if (timer_counter % 60 == 0) {
                            idle_seconds++;
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    // unsubscribe from everything before we leave
    if (timer_unsubscribe_int() != 0) return 1;
    if (kbd_unsubscribe_int() != 0) return 1;

    return 0;
}
