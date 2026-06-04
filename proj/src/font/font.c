#include "font.h"
#include "video.h"
#include <lcom/lcf.h>

#include "../assets/font/letter_a.xpm"
#include "../assets/font/letter_b.xpm"
#include "../assets/font/letter_c.xpm"
#include "../assets/font/letter_d.xpm"
#include "../assets/font/letter_e.xpm"
#include "../assets/font/letter_f.xpm"
#include "../assets/font/letter_g.xpm"
#include "../assets/font/letter_h.xpm"
#include "../assets/font/letter_i.xpm"
#include "../assets/font/letter_j.xpm"
#include "../assets/font/letter_k.xpm"
#include "../assets/font/letter_l.xpm"
#include "../assets/font/letter_m.xpm"
#include "../assets/font/letter_n.xpm"
#include "../assets/font/letter_o.xpm"
#include "../assets/font/letter_p.xpm"
#include "../assets/font/letter_q.xpm"
#include "../assets/font/letter_r.xpm"
#include "../assets/font/letter_s.xpm"
#include "../assets/font/letter_t.xpm"
#include "../assets/font/letter_u.xpm"
#include "../assets/font/letter_v.xpm"
#include "../assets/font/letter_w.xpm"
#include "../assets/font/letter_x.xpm"
#include "../assets/font/letter_y.xpm"
#include "../assets/font/letter_z.xpm"

#include "../assets/font/number_zero.xpm"
#include "../assets/font/number_one.xpm"
#include "../assets/font/number_two.xpm"
#include "../assets/font/number_three.xpm"
#include "../assets/font/number_four.xpm"
#include "../assets/font/number_five.xpm"
#include "../assets/font/number_six.xpm"
#include "../assets/font/number_seven.xpm"
#include "../assets/font/number_eight.xpm"
#include "../assets/font/number_nine.xpm"

#include "../assets/font/char_colon.xpm"
#include "../assets/font/char_slash.xpm"
#include "../assets/font/char_underscore.xpm"
#include "../assets/font/char_exclaim.xpm"


static xpm_map_t get_xpm(char c) {
    switch (c) {
        case 'a': return (xpm_map_t) a_xpm;
        case 'b': return (xpm_map_t) b_xpm;
        case 'c': return (xpm_map_t) c_xpm;
        case 'd': return (xpm_map_t) d_xpm;
        case 'e': return (xpm_map_t) e_xpm;
        case 'f': return (xpm_map_t) f_xpm;
        case 'g': return (xpm_map_t) g_xpm;
        case 'h': return (xpm_map_t) h_xpm;
        case 'i': return (xpm_map_t) i_xpm;
        case 'j': return (xpm_map_t) j_xpm;
        case 'k': return (xpm_map_t) k_xpm;
        case 'l': return (xpm_map_t) l_xpm;
        case 'm': return (xpm_map_t) m_xpm;
        case 'n': return (xpm_map_t) n_xpm;
        case 'o': return (xpm_map_t) o_xpm;
        case 'p': return (xpm_map_t) p_xpm;
        case 'q': return (xpm_map_t) q_xpm;
        case 'r': return (xpm_map_t) r_xpm;
        case 's': return (xpm_map_t) s_xpm;
        case 't': return (xpm_map_t) t_xpm;
        case 'u': return (xpm_map_t) u_xpm;
        case 'v': return (xpm_map_t) v_xpm;
        case 'w': return (xpm_map_t) w_xpm;
        case 'x': return (xpm_map_t) x_xpm;
        case 'y': return (xpm_map_t) y_xpm;
        case 'z': return (xpm_map_t) z_xpm;
        case '0': return (xpm_map_t) number_zero_xpm;
        case '1': return (xpm_map_t) number_one_xpm;
        case '2': return (xpm_map_t) number_two_xpm;
        case '3': return (xpm_map_t) number_three_xpm;
        case '4': return (xpm_map_t) number_four_xpm;
        case '5': return (xpm_map_t) number_five_xpm;
        case '6': return (xpm_map_t) number_six_xpm;
        case '7': return (xpm_map_t) number_seven_xpm;
        case '8': return (xpm_map_t) number_eight_xpm;
        case '9': return (xpm_map_t) number_nine_xpm;
        case ':': return (xpm_map_t) colon_xpm;
        case '/': return (xpm_map_t) slash_xpm;
        case '_': return (xpm_map_t) underscore_xpm;
        case '!': return (xpm_map_t) exclaim_xpm;
        default:  return NULL;
    }
}

static uint16_t char_advance(char c) {
    if (c >= 'a' && c <= 'z')
        return FONT_LETTER_W;
    if (c >= '0' && c <= '9')
        return FONT_DIGIT_W;
    if (c == ' ')
        return FONT_SPACE_W;
    if (c == ':' || c == '/' || c == '_' || c == '!')
        return FONT_LETTER_W;
    return 0;
}

static void draw_xpm_tinted(xpm_map_t xpm, uint16_t x, uint16_t y, uint32_t color) {
    xpm_image_t img;
    uint32_t *map = (uint32_t *) xpm_load(xpm, XPM_8_8_8_8, &img);
    if (map == NULL) return;

    uint32_t transparent = xpm_transparency_color(XPM_8_8_8_8);
    uint16_t sw = (uint16_t) vg_get_x_res();
    uint16_t sh = (uint16_t) vg_get_y_res();

    for (uint16_t row = 0; row < (uint16_t) img.height; row++) {
        for (uint16_t col = 0; col < (uint16_t) img.width; col++) {
            if (map[(uint32_t) row * img.width + col] == transparent) continue;
            uint16_t px = x + col;
            uint16_t py = y + row;
            if (px < sw && py < sh)
                vg_draw_pixel(px, py, color);
        }
    }

    free(map);
}

uint16_t font_draw_string(uint16_t x, uint16_t y, const char *s, uint32_t color) {
    uint16_t cx = x;
    while (*s) {
        xpm_map_t xpm = get_xpm(*s);
        if (xpm != NULL) draw_xpm_tinted(xpm, cx, y, color);
        cx += char_advance(*s);
        s++;
    }
    return cx - x;
}

uint16_t font_string_width(const char *s) {
    uint16_t w = 0;
    while (*s) {
        w += char_advance(*s++);
    }
    if (w > CHAR_SPACING) {
        w -= CHAR_SPACING;
    }
    return w;
}

static void draw_xpm_tinted_scaled(xpm_map_t xpm, uint16_t x, uint16_t y, uint32_t color, uint8_t scale) {
    xpm_image_t img;
    uint32_t *map = (uint32_t *) xpm_load(xpm, XPM_8_8_8_8, &img);
    if (map == NULL) return;

    uint32_t transparent = xpm_transparency_color(XPM_8_8_8_8);
    uint16_t sw = (uint16_t) vg_get_x_res();
    uint16_t sh = (uint16_t) vg_get_y_res();

    for (uint16_t row = 0; row < (uint16_t) img.height; row++) {
        for (uint16_t col = 0; col < (uint16_t) img.width; col++) {
            if (map[(uint32_t) row * img.width + col] == transparent) continue;
            for (uint8_t dy = 0; dy < scale; dy++) {
                for (uint8_t dx = 0; dx < scale; dx++) {
                    uint16_t px = x + (uint16_t)(col * scale + dx);
                    uint16_t py = y + (uint16_t)(row * scale + dy);
                    if (px < sw && py < sh)
                        vg_draw_pixel(px, py, color);
                }
            }
        }
    }

    free(map);
}

uint16_t font_draw_string_scaled(uint16_t x, uint16_t y, const char *s, uint32_t color, uint8_t scale) {
    uint16_t cx = x;
    while (*s) {
        xpm_map_t xpm = get_xpm(*s);
        if (xpm != NULL) draw_xpm_tinted_scaled(xpm, cx, y, color, scale);
        cx += (uint16_t)(char_advance(*s) * scale);
        s++;
    }
    return cx - x;
}

uint16_t font_string_width_scaled(const char *s, uint8_t scale) {
    return (uint16_t)(font_string_width(s) * scale);
}
