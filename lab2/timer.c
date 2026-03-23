#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int hook_id = 0; //global variable
int timer_counter = 0; //global counter variable

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

//we want to be noticed when Timer call us
int (timer_subscribe_int)(uint8_t *bit_no) {
  //error handling
  if (bit_no == NULL) {
    printf("bit_no is NULL.\n"); return 1;
  }

  //we tell lab2.c that the Timer is bit number 0
  *bit_no = hook_id;

  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != 0){
    printf("sys_irqsetpolicy error.\n"); return 1;
  }

  return 0;
}

//to be used at the end of the program 
int (timer_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hook_id) != 0){
    //we just need to call sys_irqrmpolicy on the hook_id of the policy
    printf("sys_irqrmpolicy error.\n"); return 1;
  }

  return 0;
}

//rapid action that our program does when he receives a Kernel call telling the Timer called us
void (timer_int_handler)() {
  //always we have an interruption, we sum 1 to the counter
  timer_counter++;
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
