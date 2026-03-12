#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
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

//read the configuration by going to hardware
int (timer_get_conf)(uint8_t timer, uint8_t *st) {

  //error handling -> segmentation faults
  if (st == NULL) return 1;
  
  //timer only can be 0, 1 or 2, if return other value -> error
  if (!(timer == 0 || timer == 1 || timer == 2)) return 1;

  //build the control word -> read-back command
  uint8_t comm = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);

  //user minix function to write this word in control reg - 0x43
  if (sys_outb(TIMER_CTRL, comm) != OK) return 1; //error handling

  //read the configuration
  switch (timer)
    {
    case 0:
      if (util_sys_inb(TIMER_0, st) != OK) return 1;
      break;
    
    case 1:
    if (util_sys_inb(TIMER_1, st) != OK) return 1;
    break;
    
    case 2:
    if (util_sys_inb(TIMER_2, st) != OK) return 1;
    break;

    default:
      break;
    }

    return 0; //sucess
}

//to print legibly this configuration on the screen
//st -> configuration byte
int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {

  union timer_status_field_val conf;

  switch (field)
  {
  case tsf_all: //display status byte, in hexadecimal
    conf.byte = st;
    break;
  
  case tsf_initial: //display the initialization mode only
    //isolate 5 and 4 bits
    conf.in_mode = (st >> 4) & 0x03;
    break;

  case tsf_mode: //display the counting mode, only
    //isolate 3, 2 and 1 bits
    //counting mode : 0 - 5
    conf.count_mode = (st >> 1) & 0x07;
    if (conf.count_mode == 6) conf.count_mode = 2;
    else if (conf.count_mode == 7) conf.count_mode = 3;
    break;

  case tsf_base: //display the counting base (BCD or binary), only
    //isolate bit 0
    conf.bcd = st & 0x01;
    break;

  default:
    break;
  }

  return timer_print_config(timer, field, conf);
}
