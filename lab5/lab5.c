// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>
#include "video.h"
#include "../lab3/keyboard.h"

#include <stdint.h>
#include <stdio.h>

extern uint8_t scancode_byte;

int main(int argc, char *argv[]) {
    // sets the language of LCF messages (can be either EN-US or PT-PT)
    lcf_set_language("EN-US");

    // enables to log function invocations that are being "wrapped" by LCF
    // [comment this out if you don't want/need it]
    lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

    // enables to save the output of printf function calls on a file
    // [comment this out if you don't want/need it]
    lcf_log_output("/home/lcom/labs/lab5/output.txt");

    // handles control over to LCF
    // [LCF handles command line arguments and invokes the right function]
    if (lcf_start(argc, argv))
        return 1;

    // LCF clean up tasks
    // [must be the last statement before return]
    lcf_cleanup();

    return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
    if (vg_init(mode) == NULL) return 1; // change to specified mode
    tickdelay(micros_to_ticks(delay * 1000000)); // wait
    return vg_exit(); // restore minix text mode
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    if (vg_init(mode) == NULL) return 1;

    if ((vg_draw_rectangle)(x, y, width, height, color) != 0) {
        vg_exit();
        return 1;
    }

    uint8_t bit_no;
    if (keyboard_subscribe_int(&bit_no) != 0) {
        vg_exit();
        return 1;
    }

    uint32_t irq_set = BIT(bit_no);
    int ipc_status;
    message msg;
    extern uint8_t scancode_byte;
    scancode_byte = 0;

    while (scancode_byte != 0x81) { // 0x81 is the breakcode for esc
        if (driver_receive(ANY, &msg, &ipc_status) != 0) { // wait for an interrupt message
            printf("driver_receive failed\n");
            continue;
        }

        if (is_ipc_notify(ipc_status)) { // received notification
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:
                    if (msg.m_notify.interrupts & irq_set) kbc_ih();
                    break;
                default:
                    break;
            }
        }
    }

    keyboard_unsubscribe_int();
    return vg_exit();
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
    // load xpm image
    xpm_image_t img;
    uint8_t *map = xpm_load(xpm, XPM_INDEXED, &img);
    if (map == NULL) {
        printf("Failed to load XPM\\n");
        return 1;
    }

    // graphics mode 0x105
    if (vg_init(0x105) == NULL) return 1;

    unsigned bpp = get_bytes_per_pixel();
    uint8_t *ptr = map;

    // draw image
    for (int i = 0; i < img.height; i++) {
        for (int j = 0; j < img.width; j++) {
            // get color
            uint32_t color = 0;
            memcpy(&color, ptr, bpp);
            ptr += bpp;

            // draw pixel
            vg_draw_pixel(x + j, y + i, color);
        }
    }

    uint8_t bit_no;
    if (keyboard_subscribe_int(&bit_no) != 0) {
        vg_exit();
        return 1;
    }

    uint32_t irq_set = BIT(bit_no);
    int ipc_status;
    message msg;
    extern uint8_t scancode_byte;

    while (scancode_byte != 0x81) {
        if (driver_receive(ANY, &msg, &ipc_status) != 0) {
            continue;
        }
        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:
                    if (msg.m_notify.interrupts & irq_set) {
                        kbc_ih();
                    }
                    break;
                default:
                    break;
            }
        }
    }

    keyboard_unsubscribe_int();
    return vg_exit();
}
