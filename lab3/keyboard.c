#include "keyboard.h"

static int kbd_hook_id = 1;

int (keyboard_subscribe_int)(uint8_t *bit_no) {

  if (bit_no == NULL) {
    printf("bit_no is NULL.\n");
    return 1;
  }

  *bit_no = kbd_hook_id;

  if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id) != 0) {
    printf("sys_irqsetpolicy error.\n");
    return 1;
  }

  return 0;
}

int (keyboard_unsubscribe_int)() {

  if (sys_irqrmpolicy(&kbd_hook_id) != 0) {
    printf("sys_irqrmpolicy error.\n");
    return 1;
  }

  return 0;
}
