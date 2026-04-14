#include "keyboard.h"
#include "i8042.h"
#include "../lab2/utils.c"

static int kbd_hook_id = 1;

/* shared with lab3.c via extern in keyboard.h */
uint8_t scancode_byte = 0;
bool    kbc_error     = false;

/* only counted during polling, not during interrupt mode */
static uint32_t sys_inb_count = 0;

int (keyboard_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL) return 1;
  *bit_no = kbd_hook_id;
  if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id) != 0)
    return 1;
  return 0;
}

int (keyboard_unsubscribe_int)(void) {
  if (sys_irqrmpolicy(&kbd_hook_id) != 0) return 1;
  return 0;
}

int (kbc_write_command)(uint8_t cmd) {
  uint8_t status;
  for (int i = 0; i < RETRIES; i++) {
    if (util_sys_inb(KBC_ST_REG, &status) != 0) return 1;
    if ((status & IBF) == 0) {
      if (sys_outb(KBC_CMD_REG, cmd) != 0) return 1;
      return 0;
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }
  return 1;
}

int (kbc_write_argument)(uint8_t arg) {
  uint8_t status;
  for (int i = 0; i < RETRIES; i++) {
    if (util_sys_inb(KBC_ST_REG, &status) != 0) return 1;
    if ((status & IBF) == 0) {
      if (sys_outb(KBC_IN_BUF, arg) != 0) return 1;
      return 0;
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }
  return 1;
}

int (kbc_read_cmd_byte)(uint8_t *cmd_byte) {
  uint8_t status;
  if (cmd_byte == NULL) return 1;
  if (kbc_write_command(READ_CMD_BYTE) != 0) return 1;
  for (int i = 0; i < RETRIES; i++) {
    if (util_sys_inb(KBC_ST_REG, &status) != 0) return 1;
    if (status & OBF) {
      if (util_sys_inb(KBC_OUT_BUF, cmd_byte) != 0) return 1;
      if ((status & (PAR_ERR | TO_ERR)) || (status & AUX)) return 1;
      return 0;
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }
  return 1;
}

int (kbc_write_cmd_byte)(uint8_t cmd_byte) {
  if (kbc_write_command(WRITE_CMD_BYTE) != 0) return 1;
  if (kbc_write_argument(cmd_byte) != 0) return 1;
  return 0;
}

int (kbd_enable_interrupts)(void) {
  uint8_t cmd_byte;
  if (kbc_read_cmd_byte(&cmd_byte) != 0) return 1;
  cmd_byte |= KBD_INT;
  if (kbc_write_cmd_byte(cmd_byte) != 0) return 1;
  return 0;
}

void (kbc_ih)(void) {
  uint8_t status, data;
  kbc_error = false;

  if (util_sys_inb(KBC_ST_REG, &status) != 0) { kbc_error = true; return; }
  if (!(status & OBF))                          { kbc_error = true; return; }
  if (util_sys_inb(KBC_OUT_BUF, &data)  != 0)  { kbc_error = true; return; }
  if ((status & (PAR_ERR | TO_ERR)) || (status & AUX)) { kbc_error = true; return; }

  scancode_byte = data;
}

int (kbd_poll)(void) {
  uint8_t status, data;

  if (util_sys_inb(KBC_ST_REG, &status) != 0) {
    tickdelay(micros_to_ticks(DELAY_US));
    return 1;
  }

  /* no data ready yet */
  if (!(status & OBF)) {
    tickdelay(micros_to_ticks(DELAY_US));
    return 1;
  }

  /* data is available, count the status read */
  sys_inb_count++;

  if (util_sys_inb(KBC_OUT_BUF, &data) != 0) {
    tickdelay(micros_to_ticks(DELAY_US));
    return 1;
  }

  /* count the data read */
  sys_inb_count++;

  if ((status & (PAR_ERR | TO_ERR)) || (status & AUX)) return 1;

  scancode_byte = data;
  return 0;
}

uint32_t (kbd_get_sys_inb_count)(void)   { return sys_inb_count; }
void     (kbd_reset_sys_inb_count)(void) { sys_inb_count = 0; }
