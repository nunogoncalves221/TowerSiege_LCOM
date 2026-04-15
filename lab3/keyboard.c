#include "keyboard.h"
#include "i8042.h"
#include "../lab2/utils.c"

// hook id used to subscribe keyboard interrupts
static int kbd_hook_id = 1;

// shared with lab3.c via extern in keyboard.h
uint8_t scancode_byte = 0; // last byte read from keyboard
bool    kbc_error     = false; // indicates if an error occurred

// only counted during polling, not during interrupt mode
static uint32_t sys_inb_count = 0; // counts sys_inb calls

int (keyboard_subscribe_int)(uint8_t *bit_no) {
  // check pointer
  if (bit_no == NULL) return 1;

  // set bit number for interrupts
  *bit_no = kbd_hook_id;

  // subscribe keyboard interrupts
  if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id) != 0)
    return 1;

  return 0;
}

int (keyboard_unsubscribe_int)(void) {
  // remove interrupt policy
  if (sys_irqrmpolicy(&kbd_hook_id) != 0) return 1;
  return 0;
}

int (kbc_write_command)(uint8_t cmd) {
  uint8_t status;

  // try multiple times
  for (int i = 0; i < RETRIES; i++) {

    // read status register
    if (util_sys_inb(KBC_ST_REG, &status) != 0) return 1;

    // check if input buffer is empty
    if ((status & IBF) == 0) {

      // write command
      if (sys_outb(KBC_CMD_REG, cmd) != 0) return 1;
      return 0;
    }

    // wait before retry
    tickdelay(micros_to_ticks(DELAY_US));
  }

  return 1;
}

int (kbc_write_argument)(uint8_t arg) {
  uint8_t status;

  // try multiple times
  for (int i = 0; i < RETRIES; i++) {

    // read status register
    if (util_sys_inb(KBC_ST_REG, &status) != 0) return 1;

    // check if input buffer is empty
    if ((status & IBF) == 0) {

      // write argument to input buffer
      if (sys_outb(KBC_IN_BUF, arg) != 0) return 1;
      return 0;
    }

    // wait before retry
    tickdelay(micros_to_ticks(DELAY_US));
  }

  return 1;
}

int (kbc_read_cmd_byte)(uint8_t *cmd_byte) {
  uint8_t status;

  // check pointer
  if (cmd_byte == NULL) return 1;

  // send read command
  if (kbc_write_command(READ_CMD_BYTE) != 0) return 1;

  // try to read result
  for (int i = 0; i < RETRIES; i++) {

    // read status register
    if (util_sys_inb(KBC_ST_REG, &status) != 0) return 1;

    // check if output buffer has data
    if (status & OBF) {

      // read command byte
      if (util_sys_inb(KBC_OUT_BUF, cmd_byte) != 0) return 1;

      // check for errors or mouse data
      if ((status & (PAR_ERR | TO_ERR)) || (status & AUX)) return 1;

      return 0;
    }

    // wait before retry
    tickdelay(micros_to_ticks(DELAY_US));
  }

  return 1;
}

int (kbc_write_cmd_byte)(uint8_t cmd_byte) {
  // send write command
  if (kbc_write_command(WRITE_CMD_BYTE) != 0) return 1;

  // send new command byte value
  if (kbc_write_argument(cmd_byte) != 0) return 1;

  return 0;
}

int (kbd_enable_interrupts)(void) {
  uint8_t cmd_byte;

  // read current command byte
  if (kbc_read_cmd_byte(&cmd_byte) != 0) return 1;

  // set keyboard interrupt bit
  cmd_byte |= KBD_INT;

  // write updated command byte
  if (kbc_write_cmd_byte(cmd_byte) != 0) return 1;

  return 0;
}

void (kbc_ih)(void) {
  uint8_t status, data;

  // assume no error at start
  kbc_error = false;

  // read status register
  if (util_sys_inb(KBC_ST_REG, &status) != 0) { kbc_error = true; return; }

  // check if output buffer has data
  if (!(status & OBF)) { kbc_error = true; return; }

  // read data from output buffer
  if (util_sys_inb(KBC_OUT_BUF, &data) != 0) { kbc_error = true; return; }

  // check for errors or mouse data
  if ((status & (PAR_ERR | TO_ERR)) || (status & AUX)) { kbc_error = true; return; }

  // store scancode byte
  scancode_byte = data;
}

int (kbd_poll)(void) {
  uint8_t status, data;

  // read status register
  if (util_sys_inb(KBC_ST_REG, &status) != 0) {
    tickdelay(micros_to_ticks(DELAY_US));
    return 1;
  }

  // no data available
  if (!(status & OBF)) {
    tickdelay(micros_to_ticks(DELAY_US));
    return 1;
  }

  // count status read
  sys_inb_count++;

  // read data
  if (util_sys_inb(KBC_OUT_BUF, &data) != 0) {
    tickdelay(micros_to_ticks(DELAY_US));
    return 1;
  }

  // count data read
  sys_inb_count++;

  // check for errors or mouse data
  if ((status & (PAR_ERR | TO_ERR)) || (status & AUX)) return 1;

  // store scancode byte
  scancode_byte = data;
  return 0;
}

// return number of sys_inb calls
uint32_t (kbd_get_sys_inb_count)(void) { return sys_inb_count; }

// reset sys_inb counter
void (kbd_reset_sys_inb_count)(void) { sys_inb_count = 0; }