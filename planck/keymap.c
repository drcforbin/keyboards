/* Copyright 2015-2017 Jack Humbert
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H

enum planck_layers {
  _QWERTY,
  _LOWER,
  _RAISE,
  _ADJUST
};

enum planck_keycodes {
  ALT_L = SAFE_RANGE,
  ALT_R
};

#define LOWER MO(_LOWER)
#define RAISE MO(_RAISE)
#define CTL_ESC CTL_T(KC_ESC)
#define CTL_ENT CTL_T(KC_ENT)
#define GUI_L MT(MOD_LGUI, KC_LBRC)
#define GUI_R MT(MOD_RGUI, KC_RBRC)
#define GUIX(K) RGUI(KC_##K)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[_LOWER] = LAYOUT_planck_grid(
    KC_GRV,  KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, _______,
    KC_LCTL, _______, _______, _______, _______, _______, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, _______, KC_PIPE,
    KC_LSFT, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_HOME, KC_PGUP,
    KC_LGUI, KC_LALT, _______, _______, _______, KC_SPC,  KC_DEL,  _______, _______, _______, KC_END,  KC_PGDN
),

[_QWERTY] = LAYOUT_planck_grid(
    KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,     KC_U,    KC_I,    KC_O,    KC_P,    KC_QUOT,
    CTL_ESC, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,     KC_J,    KC_K,    KC_L,    KC_SCLN, CTL_ENT,
    SC_LSPO, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,     KC_M,    KC_COMM, KC_DOT,  KC_SLSH, SC_RSPC,
    GUI_L,   ALT_L,   KC_LT,   _______, LOWER,   KC_SPC,  KC_BSPC,  RAISE,   _______, KC_GT,   ALT_R,   GUI_R
),

[_RAISE] = LAYOUT_planck_grid(
    KC_TILD, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_DEL,
    KC_LCTL, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_MINS, KC_EQL,  KC_LBRC, KC_RBRC, KC_BSLS,
    KC_LSFT, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR, KC_SLSH,
    KC_LGUI, KC_LALT, _______, _______, _______, KC_SPC,  KC_DEL,  _______, _______, _______, KC_RALT, KC_LGUI
),

[_ADJUST] = LAYOUT_planck_grid(
    _______, GUIX(1), GUIX(2), GUIX(3), GUIX(4), GUIX(5), GUIX(6), GUIX(7), GUIX(8), GUIX(9), GUIX(0), KC_SCRL,
    KC_LCTL, QK_RBT,  _______, _______, _______, _______, GUIX(H), GUIX(J), GUIX(K), GUIX(L), _______, _______,
    KC_LSFT, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, GUIX(ENT),
    KC_LGUI, KC_LALT, _______, _______, _______, _______, _______, _______, _______, _______, KC_PSCR, KC_PAUS
)
};

layer_state_t layer_state_set_user(layer_state_t state) {
  return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case ALT_L:
      perform_space_cadet(record, KC_LALT, KC_LSFT, KC_LBRACKET);
      return false;

    case ALT_R:
      perform_space_cadet(record, KC_RALT, KC_RSFT, KC_RBRACKET);
      return false;
  }
  return true;
}

