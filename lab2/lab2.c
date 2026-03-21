#include <lcom/lcf.h> //imports LCF functions and macros that allow LFC to wrap my program
#include <lcom/lab2.h> //header file containing the test functions for this lab

#include <stdbool.h> //provide the boolean type
#include <stdint.h> //provide fixed length integer types, such as uint8_t, uint16_t, uint32;t


int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab2/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

//timer test function
int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
  
  uint8_t st; //to save the configuration byte
  //reads the hardware configuration using 
  //a function thats reads the status of a timer
  if (timer_get_conf(timer, &st) != 0){ //error handling
    printf("timer_get_conf error.\n");
    return 1; 
  }
  //print the configuration using a function that prints the 
  //specified fields of a timer's configuration/status
  if (timer_display_conf(timer, st, field) != 0) { //error handling
    printf("timer_display_conf error.\n");
    return 1;
  } 

  return 0; //sucess
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
  if (timer_set_frequency(timer, freq) != 0) {
    printf("error calling timer_set_frequency.\n"); return 1;
  }

  return 0;
}

int(timer_test_int)(uint8_t time) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}
