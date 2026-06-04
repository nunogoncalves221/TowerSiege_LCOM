#include "nameentry.h"
#include "font.h"
#include "high_scores.h"
#include <string.h>

#define SC_ENTER     0x1C
#define SC_BACKSPACE 0x0E

// set 1 make codes → lowercase letter (0 means unmapped)
static const char sc_to_char[128] = {
    [0x10]='q',[0x11]='w',[0x12]='e',[0x13]='r',[0x14]='t',
    [0x15]='y',[0x16]='u',[0x17]='i',[0x18]='o',[0x19]='p',
    [0x1E]='a',[0x1F]='s',[0x20]='d',[0x21]='f',[0x22]='g',
    [0x23]='h',[0x24]='j',[0x25]='k',[0x26]='l',
    [0x2C]='z',[0x2D]='x',[0x2E]='c',[0x2F]='v',
    [0x30]='b',[0x31]='n',[0x32]='m',[0x39]=' ',
};

static char name[HS_MAX_NAME_LEN + 1];
static int  name_len;

void nameentry_reset(void) {
    memset(name, 0, sizeof(name));
    name_len = 0;
}

bool nameentry_handle_key(uint8_t scancode) {
    if (scancode >= 0x80) return false; // ignore break codes (key release)

    if (scancode == SC_ENTER && name_len > 0) return true;

    if (scancode == SC_BACKSPACE && name_len > 0) {
        name[--name_len] = '\0';
        return false;
    }

    char c = sc_to_char[scancode];
    if (c && name_len < HS_MAX_NAME_LEN) {
        name[name_len++] = c;
        name[name_len]   = '\0';
    }
    return false;
}

const char *nameentry_get_name(void) {
    return name;
}

void nameentry_draw(uint16_t x, uint16_t y, uint32_t color) {
    // draw current name followed by underscore cursor
    char display[HS_MAX_NAME_LEN + 2];
    int i = 0;
    while (name[i]) { display[i] = name[i]; i++; }
    display[i++] = '_';
    display[i]   = '\0';
    font_draw_string(x, y, display, color);
}
