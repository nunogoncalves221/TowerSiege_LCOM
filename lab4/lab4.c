// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// any header files included below this line should have been created by you
#include "i8042.h"

// from lab2 for mouse_test_async
extern int timer_counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
    uint8_t bit_no = IRQ_MOUSE;
    // mouse_subscribe_int() must return the bit mask!
    uint32_t irq_set = BIT(bit_no);

    // enable the mouse (using the lcf function)
    if (mouse_enable_data_reporting() != 0) return 1;

    // subscribe to mouse interrupts (don't forget: irq_reenable | irq_exclusive)
    if (mouse_subscribe_int(&bit_no) != 0) return 1;

    int ipc_status, r;
    message msg;
    struct packet mouse_packet;

    // the loop continues until 'cnt' packets have been read
    while (cnt > 0) {
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) continue;

        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:
                    if (msg.m_notify.interrupts & irq_set) { 
                        mouse_ih();

                        if (valid_byte) {
                            // synchronization: if it's the first byte, bit 3 must be 1
                            if (byte_idx == 0 && !(current_byte & BIT(3))) {
                                continue; // ignore and wait for the actual first byte
                            }

                            packet_bytes[byte_idx] = current_byte;
                            byte_idx++;

                            if (byte_idx == 3) {
                                parse_mouse_packet(&mouse_packet); 
                                mouse_print_packet(&mouse_packet);
                                byte_idx = 0;
                                cnt--;        // one less packet!
                            }
                        }
                    }
                    break;
            }
        }
    }

    // unsubscribe from mouse interrupts
    if (mouse_unsubscribe_int() != 0) return 1;

    // disable data reporting manually (using your function!)
    if (mouse_write_command(DISABLE_DATA_REPORTING) != 0) return 1;

    return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
    uint8_t mouse_bit_no = IRQ_MOUSE; 
    uint8_t timer_bit_no = 0; // timer irq is 0

    uint32_t mouse_irq_set = BIT(mouse_bit_no);
    uint32_t timer_irq_set = BIT(timer_bit_no);

    // enable data reporting manually
    if (mouse_write_command(ENABLE_DATA_REPORTING) != 0) return 1;

    // subscribe to mouse and timer interrupts
    if (mouse_subscribe_int(&mouse_bit_no) != 0) return 1;
    if (timer_subscribe_int(&timer_bit_no) != 0) return 1;

    int ipc_status, r;
    message msg;
    struct packet mouse_packet;

    // timer frequency (60hz) * time in seconds
    int max_ticks = idle_time * 60;
    timer_counter = 0;

    // exit the loop if timer_counter exceeds the limit
    while (timer_counter < max_ticks) {
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) continue;

        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:
                    
                    // --- mouse interrupt ---
                    if (msg.m_notify.interrupts & mouse_irq_set) {
                        mouse_ih();
                        if (valid_byte) {
                            if (byte_idx == 0 && !(current_byte & BIT(3))) continue;

                            packet_bytes[byte_idx] = current_byte;
                            byte_idx++;

                            if (byte_idx == 3) {
                                parse_mouse_packet(&mouse_packet);
                                mouse_print_packet(&mouse_packet);
                                byte_idx = 0;
                                
                                // the mouse moved! reset the timer!
                                timer_counter = 0;
                            }
                        }
                    }

                    // --- timer interrupt ---
                    if (msg.m_notify.interrupts & timer_irq_set) {
                        timer_int_handler(); // increments timer_counter
                    }
                    break;
            }
        }
    }

    // unsubscribe from everything
    if (timer_unsubscribe_int() != 0) return 1;
    if (mouse_unsubscribe_int() != 0) return 1;

    // disable data reporting manually
    if (mouse_write_command(DISABLE_DATA_REPORTING) != 0) return 1;

    return 0;
}