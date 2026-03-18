#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {

  // check if timer and frequency are valid
  if (timer > 2 || freq < 19 || freq > TIMER_FREQ) {
    printf("Timer or frequency invalid");
    return 1;
  };

  uint8_t st;

  // read current timer configuration
  if (timer_get_conf(timer, &st) != 0) {
    return 1;
  }

  // preserve the 4 least significant bits
  uint8_t control_word = st & 0x0F;

  // set initialization mode to LSB followed by MSB
  control_word |= TIMER_LSB_MSB;

  // select the correct timer
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
    return 1;
  }

  // calculate divider
  uint16_t divider = TIMER_FREQ / freq;

  uint8_t lsb, msb;

  // get LSB and MSB of divider
  if (util_get_LSB(divider, &lsb) != 0) {
    return 1;
  }

  if (util_get_MSB(divider, &msb) != 0) {
    return 1;
  }

  // write LSB and then MSB to selected timer
  if (sys_outb(TIMER_0 + timer, lsb) != 0) {
    return 1;
  }

  if (sys_outb(TIMER_0 + timer, msb) != 0) {
    return 1;
  }

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
    /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void (timer_int_handler)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {

  // check if timer number is valid and pointer is not NULL
  if (timer > 2 || st == NULL) return 1;

  // build read-back command to request the status of the selected timer
  uint8_t read_back = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);

  // send read-back command to timer control register
  if (sys_outb(TIMER_CTRL, read_back) != 0) {
    return 1;
  }

  // read the status byte from the selected timer port
  if (util_sys_inb(TIMER_0 + timer, st) != 0) {
    return 1;
  }

  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {

  // union used to store the requested configuration field
  union timer_status_field_val val;

  switch(field) {

    case tsf_all:
      // store the entire status byte
      val.byte = st;
      break;

    case tsf_initial: {
      // extract initialization mode (bits 5 and 4)
      uint8_t init = (st >> 4) & 0x03;

      // convert bits to enum value
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
      // check counting base (bit 0): 0 = binary, 1 = BCD
      val.bcd = st & BIT(0);
      break;

    default:
      return 1;
  }

  return timer_print_config(timer, field, val);
}
