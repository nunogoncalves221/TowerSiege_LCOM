#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  //invalid frequency or timer
  if (timer > 2 || freq < 19) {
    printf("invalid frequency or timer.\n");
    return 1;
  }

  //read the timer configuration from the Status Byte
  uint8_t st;

  if (timer_get_conf(timer, &st) != 0){
    printf("timer_get_conf error.\n");
    return 1;
  }

  //build the new control word from the Status Byte
  //st & 0F to save the 4 right bits
  //set 5 and 4 bits
  uint8_t control_word =  (st & 0x0F) | TIMER_LSB_MSB;

  //select the correct timer
  uint8_t port;
  switch (timer)
  {
  case 0:
    control_word |= TIMER_SEL0;
    port = TIMER_0;
    break;
  
  case 1:
    control_word |= TIMER_SEL1;
     port = TIMER_1;
    break;

  case 2:
    control_word |= TIMER_SEL2;
     port = TIMER_2;
    break;
  
  default:
    break;
  }

  //write the control word in Control Register
  if (sys_outb(TIMER_CTRL, control_word) != 0) {
    printf("error writing control word.\n"); 
    return 1;
  } 

  //write the "divider" initial value 
  uint16_t divider = TIMER_FREQ / freq;
  uint8_t lsb, msb;

  //using utils.c functions
  if (util_get_LSB(divider, &lsb) != 0){
    printf("error getting LSB from divider.\n");
    return 1;
  }

  if (util_get_MSB(divider, &msb) != 0){
    printf("error getting MSB from divider.\n");
    return 1;
  }

  //write the lsb and msb in port
  if (sys_outb(port, lsb) != 0) {
    printf("error writing lsb to port.\n"); return 1;
  }

  if (sys_outb(port, msb) != 0) {
    printf("error writing msb to port.\n"); return 1;
  }

  return 0; //sucess
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
  if (st == NULL) {
    printf("st is NULL.\n");
    return 1;
  }
  
  //timer only can be 0, 1 or 2, if return other value -> error
  if (!(timer == 0 || timer == 1 || timer == 2)) {
    printf("invalid timer.\n");
    return 1;
  };

  //build the control word -> read-back command
  uint8_t command = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);

  //user minix function to write this word in control reg - 0x43
  if (sys_outb(TIMER_CTRL, command) != OK) { //error handling
    printf("sys_outb error.\n");
    return 1;
  } 

  //read the configuration
  switch (timer)
    {
    case 0:
      if (util_sys_inb(TIMER_0, st) != OK) {
        printf("util_sys_inb error.\n");
        return 1;
      }
      break;
    
    case 1:
    if (util_sys_inb(TIMER_1, st) != OK) {
        printf("util_sys_inb error.\n");
        return 1;
    }
    break;
    
    case 2:
    if (util_sys_inb(TIMER_2, st) != OK) {
        printf("util_sys_inb error.\n");
        return 1;
    }
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
