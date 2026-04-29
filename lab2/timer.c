#include <lcom/lcf.h>
#include "my_timer.h"

#include <stdint.h>

#include "i8254.h"

int hook_id = 0;                     // identifier for interrupt subscription
static uint32_t timer_counter = 0;  // internal counter of timer interrupts

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {

  // check if timer number and frequency are valid
  if (timer > 2 || freq < 19 || freq > TIMER_FREQ) {
    printf("Timer or frequency invalid");
    return 1;
  };

  uint8_t st;

  // read current timer configuration
  if (timer_get_conf(timer, &st) != 0) {
    printf("timer_get_conf error.\n");
    return 1;
  }

  // preserve lower 4 bits (mode and BCD)
  uint8_t control_word = st & 0x0F;

  // set initialization mode to LSB followed by MSB
  control_word |= TIMER_LSB_MSB;

  // select which timer to configure
  switch (timer) {
    case 0:
      control_word |= TIMER_SEL0;
      break;
    case 1:
      control_word |= TIMER_SEL1;
      break;
    case 2:
      control_word |= TIMER_SEL2;
      break;
    default:
      return 1;
  }

  // write control word to control register
  if (sys_outb(TIMER_CTRL, control_word) != 0) {
    printf("error writing control word.\n"); 
    return 1;
  }

  // calculate divider value
  uint16_t divider = TIMER_FREQ / freq;

  uint8_t lsb, msb;

  // extract LSB and MSB from divider
  if (util_get_LSB(divider, &lsb) != 0) {
    printf("error getting LSB from divider.\n");
    return 1;
  }

  if (util_get_MSB(divider, &msb) != 0) {
    printf("error getting MSB from divider.\n");
    return 1;
  }

  // write LSB and MSB to timer port
  if (sys_outb(TIMER_0 + timer, lsb) != 0) {
    printf("error writing lsb to port.\n");
    return 1;
  }

  if (sys_outb(TIMER_0 + timer, msb) != 0) {
    printf("error writing msb to port.\n");
    return 1;
  }

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {

  // check if pointer is valid
  if (bit_no == NULL) {
    printf("bit_no is NULL.\n");
    return 1;
  }

  // store hook id in bit_no
  *bit_no = hook_id;

  // subscribe timer interrupts (IRQ 0)
  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != 0) {
    printf("sys_irqsetpolicy error.\n");
    return 1;
  }

  return 0;
}

int (timer_unsubscribe_int)() {

  // remove interrupt subscription
  if (sys_irqrmpolicy(&hook_id) != 0) {
    printf("sys_irqrmpolicy error.\n");
    return 1;
  }

  return 0;
}

void (timer_int_handler)() {

  // increment internal interrupt counter
  timer_counter++;
}

uint32_t (get_timer_counter)() {

  // return current number of interrupts
  return timer_counter;
}

void (reset_timer_counter)() {

  // reset interrupt counter to zero
  timer_counter = 0;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {

  // check if timer is valid and pointer is not NULL
  if (timer > 2 || st == NULL) return 1;

  // build read-back command to get status byte
  uint8_t read_back = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);

  // send command to control register
  if (sys_outb(TIMER_CTRL, read_back) != 0) {
    printf("sys_outb error.\n");
    return 1;
  }

  // read status byte from timer
  if (util_sys_inb(TIMER_0 + timer, st) != 0) {
    printf("util_sys_inb error.\n");
    return 1;
  }

  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {

  // union to store selected configuration field
  union timer_status_field_val val;

  switch(field) {

    case tsf_all:
      // store full status byte
      val.byte = st;
      break;

    case tsf_initial: {
      // extract initialization mode (bits 5 and 4)
      uint8_t init = (st >> 4) & 0x03;

      // map bits to enum
      switch(init) {
        case 0: val.in_mode = INVAL_val; break;
        case 1: val.in_mode = LSB_only; break;
        case 2: val.in_mode = MSB_only; break;
        case 3: val.in_mode = MSB_after_LSB; break;
        default: return 1;
      }
      break;
    }

    case tsf_mode: {
      // extract counting mode (bits 3,2,1)
      uint8_t mode = (st >> 1) & 0x07;

      // handle alias modes
      switch(mode) {
        case 0: val.count_mode = 0; break;
        case 1: val.count_mode = 1; break;
        case 2: val.count_mode = 2; break;
        case 3: val.count_mode = 3; break;
        case 4: val.count_mode = 4; break;
        case 5: val.count_mode = 5; break;
        case 6: val.count_mode = 2; break; // 110
        case 7: val.count_mode = 3; break; // 111
        default:
          return 1;
      }
      break;
    }

    case tsf_base:
      // extract counting base (bit 0)
      val.bcd = st & BIT(0);
      break;

    default:
      return 1;
  }

  // print selected configuration field
  return timer_print_config(timer, field, val);
}

void (fix_lazy_linking) (void) {
  return;
}
