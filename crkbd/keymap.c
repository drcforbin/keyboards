#include QMK_KEYBOARD_H


#ifdef RGBLIGHT_ENABLE
//Following line allows macro to read current RGB settings
extern rgblight_config_t rgblight_config;
#endif

extern uint8_t is_master;

enum crkbd_layers {
  _QWERTY,
  _LOWER,
  _RAISE,
  _NAV,
  _SPECIAL
};

enum custom_keycodes {
  ALT_L = SAFE_RANGE,
  ALT_R,
  SPECIAL
};

#define LOWER MO(_LOWER)
#define RAISE MO(_RAISE)
#define CTL_ESC CTL_T(KC_ESC)
#define CTL_ENT CTL_T(KC_ENT)
#define GUI_L MT(MOD_LGUI, KC_LBRC)
#define GUI_R MT(MOD_RGUI, KC_RBRC)
#define GUIX(K) RGUI(KC_##K)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [_LOWER] = LAYOUT(
    KC_GRV,  KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_PIPE, KC_BSLS, _______,
    KC_LCTL, SPECIAL, _______, KC_HOME, KC_PGUP, _______, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, _______, KC_RCTL,
    KC_LSFT, _______, _______, KC_END,  KC_PGDN, _______, _______, KC_LCBR, KC_LBRC, KC_RBRC, KC_RCBR, KC_RSFT,
                               KC_LALT, _______, KC_SPC,  KC_DEL,  _______, KC_RGUI
    ),

  [_QWERTY] = LAYOUT(
    KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,     KC_U,   KC_I,    KC_O,    KC_P,    KC_QUOT,
    CTL_ESC, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,     KC_J,   KC_K,    KC_L,    KC_SCLN, CTL_ENT,
    KC_LSPO, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,     KC_M,   KC_COMM, KC_DOT,  KC_SLSH, KC_RSPC,
                               KC_LALT, LOWER,   KC_SPC,  KC_BSPC,  RAISE,  KC_RGUI
  ),

  [_RAISE] = LAYOUT(
    KC_TILD, KC_EQL,  KC_PLUS, KC_UNDS, KC_MINS, _______, _______, KC_LT,   KC_GT,   _______, _______, _______,
    KC_LCTL, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_RCTL,
    KC_LSFT, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_RSFT,
                               KC_LALT, _______, KC_SPC,  KC_DEL,  _______, KC_RGUI
  ),

  [_NAV] = LAYOUT(
    _______, GUIX(1), GUIX(2), GUIX(3), GUIX(4), GUIX(5), GUIX(6), GUIX(7), GUIX(8), GUIX(9), GUIX(0), KC_SLCK,
    KC_LCTL, _______, RESET,   _______, _______, _______, GUIX(H), GUIX(J), GUIX(K), GUIX(L), _______,    _______,
    KC_LSFT, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_PSCR, KC_PAUS,
                               KC_LALT, _______, KC_SPC,  KC_DEL,  _______, KC_RGUI
  ),

  [_SPECIAL] = LAYOUT(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_SLCK,
    KC_LCTL, _______, RESET,   _______, _______, _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,
    KC_LSFT, _______, _______, _______, _______, _______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,
                               KC_LALT, _______, KC_SPC,  KC_DEL,  _______, KC_RGUI
  ),
};

layer_state_t layer_state_set_user(layer_state_t state) {
  return update_tri_layer_state(state, _LOWER, _RAISE, _NAV);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case SPECIAL:
      if (record->event.pressed) {
          layer_on(_SPECIAL);
      } else {
          layer_off(_SPECIAL);
      }
      return false;

    case ALT_L:
      perform_space_cadet(record, KC_LALT, KC_LSFT, KC_LBRACKET);
      return false;

    case ALT_R:
      perform_space_cadet(record, KC_RALT, KC_RSFT, KC_RBRACKET);
      return false;
  }
  return true;
}

/*
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (record->event.pressed) {
#ifdef SSD1306OLED
    set_keylog(keycode, record);
#endif
    // set_timelog();
  }

  switch (keycode) {
    case QWERTY:
      if (record->event.pressed) {
        persistent_default_layer_set(1UL<<_QWERTY);
      }
      return false;
    case LOWER:
      if (record->event.pressed) {
        layer_on(_LOWER);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      } else {
        layer_off(_LOWER);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      }
      return false;
    case RAISE:
      if (record->event.pressed) {
        layer_on(_RAISE);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      } else {
        layer_off(_RAISE);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      }
      return false;
    case ADJUST:
        if (record->event.pressed) {
          layer_on(_ADJUST);
        } else {
          layer_off(_ADJUST);
        }
        return false;
    case RGB_MOD:
      #ifdef RGBLIGHT_ENABLE
        if (record->event.pressed) {
          rgblight_mode(RGB_current_mode);
          rgblight_step();
          RGB_current_mode = rgblight_config.mode;
        }
      #endif
      return false;
    case RGBRST:
      #ifdef RGBLIGHT_ENABLE
        if (record->event.pressed) {
          eeconfig_update_rgblight_default();
          rgblight_enable();
          RGB_current_mode = rgblight_config.mode;
        }
      #endif
      break;
  }
  return true;
}
*/
