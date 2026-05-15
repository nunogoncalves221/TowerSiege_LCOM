#include "keyboard.h"
#include "i8042.h"
#include "utils.h"

static int kbd_hook_id = 1;

uint8_t scancode_byte = 0; // last byte read from keyboard
bool    kbc_error     = false; // indicates if an error occurred
uint8_t scancode_bytes[2];
uint8_t scancode_size = 0;


int (keyboard_subscribe_int)(uint8_t *bit_no) {
  // check pointer
  if (bit_no == NULL) return 1;

  // set bit number for interrupts
  *bit_no = kbd_hook_id;

  // subscribe keyboard interrupts
  if (sys_irqsetpolicy(IRQ_KEYBOARD, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id) != 0)
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
  for (int i = 0; i < MAX_RETRIES; i++) {

    // read status register
    if (util_sys_inb(KBC_CMD_REG, &status) != 0) return 1;

    // check if input buffer is empty
    if ((status & KBC_IBF) == 0) {

      // write command
      if (sys_outb(KBC_CMD_REG, cmd) != 0) return 1;
      return 0;
    }

    // wait before retry (using DELAY from i8042.h)
    tickdelay(micros_to_ticks(DELAY));
  }

  return 1;
}

int (kbc_write_argument)(uint8_t arg) {
  uint8_t status;

  // try multiple times
  for (int i = 0; i < MAX_RETRIES; i++) {

    // read status register
    if (util_sys_inb(KBC_CMD_REG, &status) != 0) return 1;

    // check if input buffer is empty
    if ((status & KBC_IBF) == 0) {

      // write argument to input buffer
      if (sys_outb(KBC_IN_BUF, arg) != 0) return 1;
      return 0;
    }

    // wait before retry (using DELAY from i8042.h)
    tickdelay(micros_to_ticks(DELAY));
  }

  return 1;
}

int (kbc_read_cmd_byte)(uint8_t *cmd_byte) {
  uint8_t status;

  // check pointer
  if (cmd_byte == NULL) return 1;

  // send read command
  if (kbc_write_command(READ_COMMAND_BYTE) != 0) return 1;

  // try to read result
  for (int i = 0; i < MAX_RETRIES; i++) {

    // read status register
    if (util_sys_inb(KBC_CMD_REG, &status) != 0) return 1;

    // check if output buffer has data (using KBC_OBF from i8042.h)
    if (status & KBC_OBF) {

      // read command byte
      if (util_sys_inb(KBC_OUT_BUF, cmd_byte) != 0) return 1;

      // check for errors or mouse data (using KBC_PAR_ERR, KBC_TO_ERR, KBC_AUX from i8042.h)
      if ((status & (KBC_PAR_ERR | KBC_TO_ERR)) || (status & KBC_AUX)) return 1;

      return 0;
    }

    // wait before retry (using DELAY from i8042.h)
    tickdelay(micros_to_ticks(DELAY));
  }

  return 1;
}

int (kbc_write_cmd_byte)(uint8_t cmd_byte) {
  // send write command
  if (kbc_write_command(WRITE_COMMAND_BYTE) != 0) return 1;

  // send new command byte value
  if (kbc_write_argument(cmd_byte) != 0) return 1;

  return 0;
}

int (kbd_enable_interrupts)(void) {
  uint8_t cmd_byte;

  // read current command byte
  if (kbc_read_cmd_byte(&cmd_byte) != 0) return 1;

  // set keyboard interrupt bit
  cmd_byte |= ENABLE_INTERRUPTS_KEYBOARD;

  // write updated command byte
  if (kbc_write_cmd_byte(cmd_byte) != 0) return 1;

  return 0;
}

void (kbc_ih)(void) {
  uint8_t status, data;

  kbc_error = false; // assume no error at start

  // read status register
  if (util_sys_inb(KBC_CMD_REG, &status) != 0) { kbc_error = true; return; }

  // check if output buffer has data
  if (!(status & KBC_OBF)) { kbc_error = true; return; }

  // read data from output buffer
  if (util_sys_inb(KBC_OUT_BUF, &data) != 0) { kbc_error = true; return; }

  scancode_byte = data;

  // check for errors or mouse data (using KBC_PAR_ERR, KBC_TO_ERR, KBC_AUX from i8042.h)
  if ((status & (KBC_PAR_ERR | KBC_TO_ERR)) || (status & KBC_AUX)) { kbc_error = true; return; }
}

int (kbd_poll)(void) {
  uint8_t status, data;

  // read status register
  if (util_sys_inb(KBC_CMD_REG, &status) != 0) {
    tickdelay(micros_to_ticks(DELAY));
    return 1;
  }

  // no data available
  if (!(status & KBC_OBF)) {
    tickdelay(micros_to_ticks(DELAY));
    return 1;
  }

  // read data
  if (util_sys_inb(KBC_OUT_BUF, &data) != 0) {
    tickdelay(micros_to_ticks(DELAY));
    return 1;
  }

  // check for errors or mouse data (using KBC_PAR_ERR, KBC_TO_ERR, KBC_AUX from i8042.h)
  if ((status & (KBC_PAR_ERR | KBC_TO_ERR)) || (status & KBC_AUX)) return 1;

  // store scancode byte
  scancode_byte = data;
  return 0;
}

bool (kbd_process_scancode)(uint8_t *size, uint8_t scancodes_array[]) {
  // add the newly read byte to our array
  scancodes_array[*size] = scancode_byte; 
  (*size)++;

  // if it's the two-byte scancode prefix (0xE0), we need to wait for the second byte
  if (scancode_byte == 0xE0 && *size == 1) {
    return false; 
  }

  // the scancode is complete and ready to be printed
  return true; 
}
