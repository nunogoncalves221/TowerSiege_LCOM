#include <lcom/lcf.h>

#include "video.h"
#include "my_timer.h"
#include "i8042.h"
#include "keyboard.h"
#include "mouse.h"
#include "rtc.h"

#include "states.h"
#include "menu.h"
#include "game.h"
#include "instructions.h"
#include "pause.h"

#define KEY_P_MAKE 0x19

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/grupo_2leic16_2/proj/trace.txt");
  lcf_log_output("/home/lcom/labs/grupo_2leic16_2/proj/output.txt");
  if (lcf_start(argc, argv))
    return 1;
  lcf_cleanup();
  return 0;
}

int(proj_main_loop)(int argc, char* argv[]) {
  if (vg_init(0x14C) == NULL) return 1;

  uint8_t timer_bit, kbd_bit, mouse_bit;

  if (timer_subscribe_int(&timer_bit) != 0) {
    vg_exit();
    return 1;
  }

  if (keyboard_subscribe_int(&kbd_bit) != 0) {
    timer_unsubscribe_int();
    vg_exit();
    return 1;
  }

  if (mouse_write_command(ENABLE_DATA_REPORTING) != 0) {
    keyboard_unsubscribe_int();
    timer_unsubscribe_int();
    vg_exit();
    return 1;
  }

  if (mouse_subscribe_int(&mouse_bit) != 0) {
    mouse_write_command(DISABLE_DATA_REPORTING);
    keyboard_unsubscribe_int();
    timer_unsubscribe_int();
    vg_exit();
    return 1;
  }

  uint32_t timer_irq = BIT(timer_bit);
  uint32_t kbd_irq   = BIT(kbd_bit);
  uint32_t mouse_irq = BIT(mouse_bit);

  app_state_t state = STATE_MENU;
  menu_reset();

  int ipc_status;
  message msg;
  bool running = true;

  while (running) {
    if (driver_receive(ANY, &msg, &ipc_status) != 0) continue;

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:

          // Timer
          if (msg.m_notify.interrupts & timer_irq) {
            timer_int_handler();

            if (get_timer_counter() % (TIMER_FREQ / FPS) == 0) {
              vg_clear_buffer();

              if (state == STATE_MENU) {
                menu_draw();
              } else if (state == STATE_GAME) {
                game_tick();
                game_draw();
              } else if (state == STATE_INSTRUCTIONS) {
                instructions_draw();
              } else if (state == STATE_PAUSE) {
                  game_draw();
                  pause_draw();
              }

              vg_refresh();
            }
          }

          // Keyboard
          if (msg.m_notify.interrupts & kbd_irq) {
            kbc_ih();

            if (!kbc_error) {
              if (state == STATE_MENU) {
                if (scancode_byte == ESC_BREAKCODE) {
                  running = false;
                }
                else if (menu_handle_key(scancode_byte)) {
                  switch (menu_get_selected()) {
                    case MENU_PLAY:
                      state = STATE_GAME;
                      game_reset();
                      break;
                    case MENU_QUIT:
                      running = false;
                      break;
                    case MENU_INSTRUCTIONS:
                      state = STATE_INSTRUCTIONS;
                      break;
                    default:
                      break;
                  }
                }

              } else if (state == STATE_GAME) {
                  game_handle_key(scancode_byte);

                  if (scancode_byte == ESC_BREAKCODE) {
                      state = STATE_PAUSE;
                      pause_reset();
                  }

              } else if (state == STATE_PAUSE) {
                  if (pause_handle_key(scancode_byte)) {
                      switch (pause_get_selected()) {
                          case PAUSE_CONTINUE:
                              state = STATE_GAME;
                              break;
                          case PAUSE_QUIT:
                              state = STATE_MENU;
                              menu_reset();
                              break;
                          default:
                              break;
                      }
                      pause_reset();
                  }
              } else if (state == STATE_INSTRUCTIONS) {
                  if (scancode_byte == ESC_BREAKCODE) {
                    state = STATE_MENU;
                    menu_reset();
                  }
                }
              }
            }

          // Mouse
          if (msg.m_notify.interrupts & mouse_irq) {
            mouse_ih();
            if (valid_byte && mouse_process_packet_byte()) {
              struct packet pp;
              parse_mouse_packet(&pp);
              static bool lb_prev = false;
              if (pp.lb && !lb_prev) {
                game_handle_click();
              }
              lb_prev = pp.lb;
              game_update_cursor(pp.delta_x, pp.delta_y);
            }
          }
          break;

        default:
          break;
      }
    }
  }

  // Cleanup
  vg_free();
  mouse_unsubscribe_int();
  mouse_write_command(DISABLE_DATA_REPORTING);
  keyboard_unsubscribe_int();
  timer_unsubscribe_int();

  return vg_exit();
}
