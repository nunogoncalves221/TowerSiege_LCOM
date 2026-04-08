#ifndef _LCOM_KBC_H_
#define _LCOM_KBC_H_

#include <stdbool.h>
#include <stdint.h>

// subscribe to keyboard interrupts
int (kbd_subscribe_int)(uint8_t *bit_no);

// unsubscribe from keyboard interrupts
int (kbd_unsubscribe_int)();

// our custom interrupt handler for the keyboard
void (kbc_ih)();

#endif /* _LCOM_KBC_H_ */
