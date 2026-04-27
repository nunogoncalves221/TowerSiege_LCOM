// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you

#include "video.h"

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
    reg86_t reg;

    memset(&reg, 0, sizeof(reg));

    reg.intno = 0x10;              // BIOS video services
    reg.ax = 0x4F02;               // VBE function: Set VBE Mode
    reg.bx = mode | BIT(14);       // Set mode + linear frame buffer

    if (sys_int86(&reg) != OK) {
        printf("video_test_init: sys_int86() failed\n");
        return 1;
    }

    sleep(delay);

    if (vg_exit() != OK) {
        printf("video_test_init: vg_exit() failed\n");
        return 1;
    }

    return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
    if (vg_init(mode) == NULL)
        return 1;

    if (vg_draw_rectangle(x, y, width, height, color) != 0) {
        vg_exit();
        return 1;
    }

    int hook_id = 0;
    uint8_t scancode = 0;

    if (sys_irqsetpolicy(1, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id) != OK) {
        printf("sys_irqsetpolicy failed\n");
        vg_exit();
        return 1;
    }

    printf("hook_id = %d\n", hook_id);
    int irq_set = BIT(hook_id);

    while (scancode != 0x81) {
        message msg;
        int ipc_status;

        if (driver_receive(ANY, &msg, &ipc_status) != OK) {
            printf("driver_receive failed\n");
            break;
        }

        if (is_ipc_notify(ipc_status)) {
            if (_ENDPOINT_P(msg.m_source) == HARDWARE) {
                if (msg.m_notify.interrupts & irq_set) {
                    uint32_t data;
                    if (sys_inb(0x60, &data) != OK) {
                        sys_irqrmpolicy(&hook_id);
                        vg_exit();
                        return 1;
                    }
                    scancode = (uint8_t) data;
                    printf("scancode = 0x%02x\n", scancode);
                }
            }
        }
    }

    sys_irqrmpolicy(&hook_id);
    vg_exit();
    return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
    /* To be completed */
    printf("%s(%8p, %u, %u): under construction\n", __func__, xpm, x, y);

    return 1;
}
