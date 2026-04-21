#ifndef _LCOM_MOUSE_H_
#define _LCOM_MOUSE_H_

#include <lcom/lcf.h>

// shared global variables 
extern uint8_t packet_bytes[3]; // stores the 3 bytes of the current packet
extern int byte_idx;            // keeps track of which byte we are reading (0, 1, or 2)
extern uint8_t current_byte;    // stores the most recently read byte from the kbc
extern bool valid_byte;         // flag to know if the read was valid

// mouse functions
int mouse_subscribe_int(uint8_t *bit_no);   // subscribes to mouse interrupts
int mouse_unsubscribe_int();                // unsubscribes from mouse interrupts
void (mouse_ih)();                          // mouse interrupt handler
void parse_mouse_packet(struct packet *pp); // parses a complete mouse packet
bool mouse_process_packet_byte();           // processes one byte of a packet, returns true if packet is complete
int mouse_write_command(uint8_t cmd);       // writes a command to the mouse

#endif /* _LCOM_MOUSE_H_ */
