#include QMK_KEYBOARD_H
#include "oled.h"

// forwards
bool is_keyboard_left(void);
const char *read_logo(void);
extern const unsigned char font[] PROGMEM;

#define ROW_1 OLED_DISPLAY_WIDTH
#define ROW_2 (OLED_DISPLAY_WIDTH * 2)

static uint32_t oled_sleep_timer = 0;

// 40 fps
#define FRAME_TIMEOUT (1000/40)
// 30 sec
#define SLEEP_TIMEOUT 30000

static const char PROGMEM corne_logo[] = {
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94,
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0x00
};

static uint16_t anim_timer = 0;

static uint8_t next_bar_val = 0;
static uint8_t next_log_byte[OLED_FONT_WIDTH] = {0};
static uint8_t line1[OLED_DISPLAY_WIDTH] = {0};
static uint8_t line2[OLED_DISPLAY_WIDTH] = {0};

static const uint8_t PROGMEM bar_lut[8] = {0, 16, 24, 56, 60, 124, 126, 255};

#define BAR_KEY_WEIGHT 128
#define BAR_KEY_DECAY_MAX 18
static uint8_t bar_key_decay = BAR_KEY_DECAY_MAX;

// clang-format off
static const char PROGMEM code_to_name[0xFF] = {
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    ' ', ' ', ' ', ' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',  // 0x
    'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '1', '2',  // 1x
    '3', '4', '5', '6', '7', '8', '9', '0',  20,  19,  27,  26,  22, '-', '=', '[',  // 2x
    ']','\\', '#', ';','\'', '`', ',', '.', '/', 128,   7,   7,   7,   7,   7,   7,  // 3x
      7,   7,   7,   7,   7,   7, 'P', 'S', 'P', 'I', 'H',  30,  16,  31,  17,  27,  // 4x
     26,  25,  24, ' ', ' ', '/', '*', '-', '+',  20, '1', '+', '3', '4', '5', '6',  // 5x
    '7', '8', '9', '0', '.','\\', 'A', 'P', '=',   7,   7,   7,   7,   7,   7,   7,  // 6x
      7,   7,   7,   7, 'X', 'H', 'M', 'S', 'S', 'A', 'U', 'C', 'C', 'P', 'F', 'M',  // 7x
    ' ', ' ', ' ', ' ', ' ', ',', '=', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 8x
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 9x
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // Ax
    ' ', ' ', ' ', ' ', ' ', ' ', '(', ')', '{', '}', ' ', ' ', 'A', 'B', 'C', 'D',  // Bx
    'E', 'F', 'X', '^', '%', '<', '>', '&', '&', '|', '|', ':', '#', ' ', '@', '!',  // Cx
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // Dx
    'C', 'S', 'A', 'C', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // Ex
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '        // Fx
};
// clang-format on

void add_keylog(uint16_t keycode) {
    if ((keycode >= QK_MOD_TAP && keycode <= QK_MOD_TAP_MAX) ||
            (keycode >= QK_LAYER_TAP && keycode <= QK_LAYER_TAP_MAX) ||
            (keycode >= QK_MODS && keycode <= QK_MODS_MAX)) {
        keycode = keycode & 0xFF;
    } else if (keycode > 0xFF) {
        keycode = 0;
    }

    if (keycode < (sizeof(code_to_name) / sizeof(char))) {
        char log_char = pgm_read_byte(&code_to_name[keycode]);
        for (uint8_t j = 0; j < OLED_FONT_WIDTH; j++) {
            const uint8_t glyph_line = pgm_read_byte(&font[log_char * OLED_FONT_WIDTH + j]);
            next_log_byte[j] = glyph_line;
        }
    }
}

bool process_record_user_oled(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        oled_sleep_timer = timer_read32();
        add_keylog(keycode);

        uint8_t t = next_bar_val + BAR_KEY_WEIGHT;
        if (t < next_bar_val) {
            next_bar_val = 255;
        } else {
            next_bar_val = t;
        }

        bar_key_decay = BAR_KEY_DECAY_MAX;
    }
    return true;
}

uint8_t render_layer_state(void) {
    uint8_t len = 0;
    oled_write_P(PSTR("LAYER: "), false);
    switch (get_highest_layer(layer_state)) {
        case _QWERTY:
            oled_write_P(PSTR("QWERTY"), false);
            len = 6;
            break;
        case _LOWER:
            oled_write_P(PSTR("LOWER"), false);
            len = 5;
            break;
        case _RAISE:
            oled_write_P(PSTR("RAISE"), false);
            len = 5;
            break;
        case _NAV:
            oled_write_P(PSTR("NAV"), false);
            len = 3;
            break;
        case _SPECIAL:
            oled_write_P(PSTR("SPCL"), false);
            len = 4;
            break;
    }
    return len;
}

void render_mod_status(uint8_t modifiers) {
    //oled_write_P(PSTR("Mods: "), false);
    oled_write_char('S', (modifiers & MOD_MASK_SHIFT));
    oled_write_char('C', (modifiers & MOD_MASK_CTRL));
    oled_write_char('A', (modifiers & MOD_MASK_ALT));
    oled_write_char('G', (modifiers & MOD_MASK_GUI));
    //oled_write_char('\n', false);
}

void render_status_left(void) {
    uint8_t len = render_layer_state();
    uint8_t t = 6 - len;
    if (t > len) {
        t = 0;
    }
    len = t + 4;

    for (; len; len--) {
        oled_write_char(' ', false);
    }
    render_mod_status(get_mods() | get_oneshot_mods());
}

void render_status_right(void) {
    // draw the logo
    oled_write_P(corne_logo, false);

    // indent the layer state
    oled_write_P(PSTR("       "), false);
    render_layer_state();
    oled_write_char('\n', false);
}

void render_frame(void) {
    // no animation for right side
    if (!is_keyboard_left()) {
        return;
    }

    // rotate line 1, and stick in the next byte of the next char,
    // then rotate the next char buffer
    memmove(line1+1, line1, OLED_DISPLAY_WIDTH - 1);
    line1[0] = next_log_byte[OLED_FONT_WIDTH - 1];
    memmove(next_log_byte+1, next_log_byte, OLED_FONT_WIDTH - 1);
    next_log_byte[0] = 0;

    // rotate line 2, sticking in the next display value
    uint8_t disp_val = pgm_read_byte(&bar_lut[next_bar_val / 32]);
    memmove(line2+1, line2, OLED_DISPLAY_WIDTH - 1);
    line2[0] = disp_val;

    // draw both bars
    for (uint8_t i = 0; i < OLED_DISPLAY_WIDTH; i++) {
        oled_write_raw_byte(line1[i], ROW_1 + i);
        oled_write_raw_byte(line2[i], ROW_2 + i);
    }

    // decay the next bar value
    if (next_bar_val > bar_key_decay) {
        next_bar_val -= bar_key_decay;
    } else {
        next_bar_val = 0;
    }

    if (bar_key_decay > 1) {
        bar_key_decay -= 1;
    }
}

bool oled_task_user(void) {
    // sleep if it has been long enough since we last got a char
    if (timer_elapsed32(oled_sleep_timer) > SLEEP_TIMEOUT) {
        oled_off();
        return false;
    } else {
        oled_on();
    }

    if (is_keyboard_left()) {
        render_status_left();
    } else {
        render_status_right();
    }

    // time for the next frame?
    if (timer_elapsed(anim_timer) > FRAME_TIMEOUT) {
        anim_timer = timer_read();
        render_frame();
    }

    return false;
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    anim_timer = timer_read();
    if (is_keyboard_left()) {
        return rotation;
    }  else {
        return OLED_ROTATION_180;
    }
}
