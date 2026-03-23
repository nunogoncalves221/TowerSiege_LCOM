#include <lcom/lcf.h> //imports LCF functions and macros that allow LFC to wrap my program
#include <lcom/lab2.h> //header file containing the test functions for this lab

#include <stdbool.h> //provide the boolean type
#include <stdint.h> //provide fixed length integer types, such as uint8_t, uint16_t, uint32;t

extern int timer_counter; //to timer_test_int function

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

//the program needs to be running and print a message with the elapsed time on the console
int(timer_test_int)(uint8_t time) {
    uint8_t bit_no; //variable to receive the bit number that the Timer will use

    //subscribe to Timer's interrupts
    if (timer_subscribe_int(&bit_no) != 0) {
      printf("timer_subscribe_int error.\n"); return 1;
    }

    //create a bit mask to identify the Timer's messages
    uint32_t irq_set = BIT(bit_no);

    int ipc_status;
    message msg;

    //interruption loop, runs until our counter reaches the total number of ticks 
    while (timer_counter < 60 * time) {

    //waiting for a MINIX message, the program "sleeps" until a message arrives
    if (driver_receive(ANY, &msg, &ipc_status) != 0) { 
      printf("driver_receive failed.\n"); continue;
    }
    
    //verify if the message is a notification
    if (is_ipc_notify(ipc_status)) { 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: //verify if it's a hardware interruption
          
          //verify if Timer's bit is set in the message's interrupt mask
          if (msg.m_notify.interrupts & irq_set) { 
            
            //the Timer called us, so we sum 1 to the counter
            timer_int_handler();

            //if exactly 60 ticks have passed (1 second)
            if (timer_counter % 60 == 0) {
              //we call this LCF function to print the elapsed on the screen
              timer_print_elapsed_time();
            }
          }
          break;
        default:
          break; //if its not from hardware, we ignore it 
      }
    }
  }

  //time is over, now we unsubscribe to clean up
  if (timer_unsubscribe_int() != 0) {
    printf("timer_unsubscribe_int error.\n"); return 1;
  }

  return 0; 
}

