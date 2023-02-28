/* Copyright 2015-2021 Jack Humbert
 * Original keymap by William Dizon <wdchromium@gmail.com>
 * QWERTY + HIRAGANA
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
#include "g/keymap_combo.h"

#define QWERTY 0
#define HIRAGANA 1
#define FUNCS 2
#define GUIS 3
#define HIRAGANA_SUPP 4

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* Base
 * ,-----------------------------------------------------------------------------------.
 * | Esc  |   1  |   2  |   3  |   4  |   5  | Del  |   6  |   7  |   8  |   9  |  0   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Tab  |   Q  |   W  |   E  |   R  |   T  | Bksp |   Y  |   U  |   I  |   O  |  P   |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * | MO(3)|   A  |   S  |   D  |   F  |   G  | Ent  |   H  |   J  |   K  |   L  |  ;   |
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * | Shift|   Z  |   X  |   C  |   V  |   B  | Tab  |   N  |   M  |   ,  |   .  |  /   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Ctrl | Alt  | GUI  | LT(3)| MO(4)|    Space    |   +  |   -  |  Del |  Ins | Ent  |
 * `-----------------------------------------------------------------------------------'
 */

[QWERTY] = LAYOUT_preonic_grid(
  QK_GESC , KC_1   , KC_2   , KC_3           , KC_4     , KC_5  , KC_DEL , KC_6  , KC_7   , KC_8   , KC_9  , KC_0   ,
  KC_TAB  , KC_Q   , KC_W   , KC_E           , KC_R     , KC_T  , KC_BSPC, KC_Y  , KC_U   , KC_I   , KC_O  , KC_P   ,
  MO(GUIS), KC_A   , KC_S   , KC_D           , KC_F     , KC_G  , KC_ENT , KC_H  , KC_J   , KC_K   , KC_L  , KC_SCLN,
  KC_LSFT , KC_Z   , KC_X   , KC_C           , KC_V     , KC_B  , KC_TAB , KC_N  , KC_M   , KC_COMM, KC_DOT, KC_SLSH,
  KC_LCTL , KC_LALT, KC_LGUI, LT(GUIS,KC_APP), MO(FUNCS), KC_SPC, KC_SPC , KC_EQL, KC_MINS, KC_DEL , KC_INS, KC_ENT),

[HIRAGANA] = LAYOUT_preonic_grid(
  QK_GESC          , KC_1      , KC_2   , KC_3           , KC_4     , KC_5  , KC_DEL , KC_6      , KC_7      , KC_8      , KC_9      , KC_0      ,
  KC_TAB           , KC_NO     , KC_W   , UC(0x3048)     , KC_R     , KC_T  , KC_BSPC, KC_Y      , UC(0x3046), UC(0x3044), UC(0x304A), KC_P      ,
  MO(GUIS)         , UC(0x3042), KC_S   , KC_D           , KC_F     , KC_G  , KC_ENT , KC_H      , KC_J      , KC_K      , KC_NO     , UC(0x3099),
  MO(HIRAGANA_SUPP), KC_Z      , KC_NO  , KC_C           , KC_NO    , KC_B  , KC_TAB , UC(0x3093), KC_M      , KC_COMM   , KC_DOT    , KC_SLSH   ,
  KC_LCTL          , KC_LALT   , KC_LGUI, LT(GUIS,KC_APP), MO(FUNCS), KC_SPC, KC_SPC , KC_EQL    , KC_MINS   , DF(QWERTY), KC_NO     , KC_ENT)   ,

/* HIRAGANA_SUPP is a pseudoshifted layer; pressing and holding shift provides access
   to size-shifted chars and square/angle brackets. */

[HIRAGANA_SUPP] = LAYOUT_preonic_grid(
  KC_TRNS , KC_NO     , KC_NO  , KC_NO     , KC_NO  , KC_NO     , KC_TRNS, KC_NO     , KC_NO     , KC_NO     , UC(0x300C), UC(0x300D),
  KC_TRNS , KC_TRNS   , KC_TRNS, UC(0x3047), KC_TRNS, UC(0x3063), KC_TRNS, KC_TRNS   , UC(0x3045), UC(0x3043), UC(0x3049), KC_TRNS   ,
  KC_NO   , UC(0x3041), KC_TRNS, KC_TRNS   , KC_TRNS, KC_TRNS   , KC_TRNS, KC_TRNS   , KC_TRNS   , KC_TRNS   , KC_TRNS   , UC(0x309A),
  KC_TRNS , KC_TRNS   , KC_TRNS, KC_TRNS   , KC_TRNS, KC_TRNS   , KC_TRNS, UC(0x3093), KC_TRNS   , UC(0x3008), UC(0x3009), KC_TRNS   ,
  KC_LCTL , KC_TRNS   , KC_TRNS, KC_TRNS   , KC_TRNS, KC_TRNS   , KC_TRNS, KC_TRNS   , KC_TRNS   , KC_TRNS   , KC_TRNS   , KC_TRNS)  ,

/* FUNCS provides all the remaining functional keys absent from a 60%;
   - Function keys align with their single digit counterparts. See QW
   - U/D/L/R arrows provided at ESDF homerow.
   - 10 key numerals on righthand side with 5 located at K. */

[FUNCS] = LAYOUT_preonic_grid(
  KC_ESC , KC_F1  , KC_F2  , KC_F3  , KC_F4  , KC_F5, KC_DEL , KC_F6 , KC_F7  , KC_F8, KC_F9  , KC_F10,
  KC_TAB , KC_F11 , KC_F12 , KC_UP  , KC_NO  , KC_NO, KC_BSPC, KC_NO , KC_P7  , KC_P8, KC_P9  , KC_NO ,
  KC_CAPS, KC_NO  , KC_LEFT, KC_DOWN, KC_RGHT, KC_NO, KC_ENT , KC_NO , KC_P4  , KC_P5, KC_P6  , KC_NO , 
  KC_LSFT, KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO, KC_TAB , KC_NO , KC_P1  , KC_P2, KC_P3  , KC_NO , 
  KC_LCTL, KC_LALT, KC_NO  , KC_NO  , KC_TRNS, KC_NO, KC_NO  , KC_EQL, KC_MINS, KC_P0, KC_PDOT, KC_ENT),

/* GUIS provides Left-GUI modified keys in limited fashion.
   This configuration matches personal configuration of i3 tiling manager */

[GUIS] = LAYOUT_preonic_grid(
  KC_GRV , LGUI(KC_1), LGUI(KC_2)   , LGUI(KC_3)   , LGUI(KC_4)   , LGUI(KC_5), LGUI(KC_DEL) , LGUI(KC_6)  , LGUI(KC_7), LGUI(KC_8)  , LGUI(KC_9), LGUI(KC_0)   ,
  KC_NO  , KC_NO     , LGUI(KC_LBRC), LGUI(KC_UP)  , LGUI(KC_RBRC), KC_NO     , LGUI(KC_BSPC), KC_NO       , KC_NO     , KC_NO       , KC_LBRC   , KC_RBRC      ,
  KC_TRNS, KC_NO     , LGUI(KC_LEFT), LGUI(KC_DOWN), LGUI(KC_RGHT), KC_NO     , LGUI(KC_ENT) , KC_NO       , KC_NO     , KC_NO       , LGUI(KC_L), KC_QUOT      ,
  KC_LSFT, KC_NO     , KC_NO        , KC_NO        , KC_NO        , KC_NO     , LGUI(KC_TAB) , KC_NO       , KC_NO     , KC_NO       , KC_NO     , KC_BSLS      ,
  KC_LCTL, KC_LALT   , KC_NO        , KC_TRNS      , KC_TRNS      , KC_NO     , KC_NO        , KC_NO       , KC_NO     , DF(HIRAGANA), UC_NEXT   , LGUI(KC_END)),

};
