#include <lcom/lcf.h>

#include "video.h"
#include "my_timer.h"
#include "i8042.h"
#include "keyboard.h"
#include "mouse.h"
#include "rtc.h"

#include "states.h"
#include "menu.h"
#include "difficulty.h"     
#include "game.h"
#include "instructions.h"
#include "pause.h"
#include "gameovermenu.h"
#include "gameovermenu_hs.h"
#include "nameentry.h"
#include "high_scores.h"

#define ESC_BREAKCODE 0x81

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

  hs_load();

  uint8_t timer_bit, kbd_bit, mouse_bit;

  // subscribe to all the interrupts we need
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

          // --- timer interrupts ---
          if (msg.m_notify.interrupts & timer_irq) {
            timer_int_handler();

            // update the screen at the requested fps
            if (get_timer_counter() % (TIMER_FREQ / FPS) == 0) {
              vg_clear_buffer();

              switch (state) {
                case STATE_MENU:
                  menu_draw();
                  break;
                case STATE_DIFFICULTY:
                  diff_draw();
                  break;
                case STATE_GAME:
                  game_tick();
                  // check if we died this tick
                  if (game_is_over()) {
                    if (hs_qualifies(game_get_score())) {
                      state = STATE_GAMEOVER_HS;
                      gameover_hs_reset();
                    } else {
                      state = STATE_GAMEOVER;
                      gameover_reset();
                    }
                  } else {
                    game_draw();
                  }
                  break;
                case STATE_GAMEOVER:
                  gameover_draw(game_get_score());
                  break;
                case STATE_GAMEOVER_HS:
                  gameover_hs_draw(game_get_score());
                  break;
                case STATE_INSTRUCTIONS:
                  instructions_draw();
                  break;
                case STATE_HIGH_SCORES:
                  hs_draw();
                  break;
                case STATE_PAUSE:
                  game_draw(); // draw game in background
                  pause_draw(); // overlay pause menu
                  break;
              }

              vg_refresh();
            }
          }

          // --- keyboard interrupts ---
          if (msg.m_notify.interrupts & kbd_irq) {
            kbc_ih();

            if (!kbc_error) {
              switch (state) {
                case STATE_MENU:
                  if (menu_handle_key(scancode_byte)) {
                    switch (menu_get_selected()) {
                      case MENU_PLAY:
                        state = STATE_DIFFICULTY;
                        diff_reset();
                        break;
                      case MENU_QUIT:
                        running = false;
                        break;
                      case MENU_INSTRUCTIONS:
                        state = STATE_INSTRUCTIONS;
                        break;
                      case MENU_HIGH_SCORES:
                        state = STATE_HIGH_SCORES;
                        break;
                      default: break;
                    }
                  }
                  break;

                case STATE_DIFFICULTY:
                  if (scancode_byte == ESC_BREAKCODE) {
                    state = STATE_MENU;
                    menu_reset();
                  } else if (diff_handle_key(scancode_byte)) {
                    game_reset();
                    state = STATE_GAME;
                  }
                  break;

                case STATE_GAME:
                  game_handle_key(scancode_byte);
                  // hit esc to pause the game
                  if (scancode_byte == ESC_BREAKCODE) {
                    state = STATE_PAUSE;
                    pause_reset();
                  }
                  break;

                case STATE_PAUSE:
                  if (pause_handle_key(scancode_byte)) {
                    switch (pause_get_selected()) {
                      case PAUSE_CONTINUE:
                        state = STATE_GAME;
                        break;
                      case PAUSE_RETRY:
                        game_reset();
                        state = STATE_GAME;
                        break;
                      case PAUSE_QUIT:
                        state = STATE_MENU;
                        menu_reset();
                        break;
                      default: break;
                    }
                    pause_reset(); // clean up pause state
                  }
                  break;

                case STATE_HIGH_SCORES:
                case STATE_INSTRUCTIONS:
                  // both just need esc to go back to main menu
                  if (scancode_byte == ESC_BREAKCODE) {
                    state = STATE_MENU;
                    menu_reset();
                  }
                  break;

                case STATE_GAMEOVER_HS:
                  if (scancode_byte == ESC_BREAKCODE) {
                    state = STATE_MENU;
                    menu_reset();
                  } else if (gameover_hs_handle_key(scancode_byte)) {
                    // user submitted their name, save it and jump to high scores
                    hs_insert(nameentry_get_name(), game_get_score(), game_get_start_time());
                    state = STATE_HIGH_SCORES;
                  }
                  break;

                case STATE_GAMEOVER:
                  if (scancode_byte == ESC_BREAKCODE) {
                    state = STATE_MENU;
                    menu_reset();
                  } else if (gameover_handle_key(scancode_byte)) {
                    if (gameover_get_selected() == GAMEOVER_RETRY) {
                      game_reset();
                      state = STATE_GAME;
                    } else {
                      state = STATE_MENU;
                      menu_reset();
                    }
                  }
                  break;
              }
            }
          }

          // --- mouse interrupts ---
          if (msg.m_notify.interrupts & mouse_irq) {
            mouse_ih();
            if (valid_byte && mouse_process_packet_byte()) {
              struct packet pp;
              parse_mouse_packet(&pp);
              
              static bool lb_prev = false;
              // check for a fresh left click
              if (pp.lb && !lb_prev) {
                if (state == STATE_GAME) {
                  game_handle_click();
                }
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

  // cleanup everything before exiting
  vg_free();
  mouse_unsubscribe_int();
  mouse_write_command(DISABLE_DATA_REPORTING);
  keyboard_unsubscribe_int();
  timer_unsubscribe_int();

  return vg_exit();
}
