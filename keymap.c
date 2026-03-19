/* Copyright 2015-2021 Jack Humbert
 * Original keymap by William Dizon <wdchromium@gmail.com>
 * HIRAGANA + KATAKANA
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
#include "jp_ime.h"

// Delegate QMK hooks to the IME module
void matrix_scan_user(void) {
    ime_matrix_scan();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    return ime_process_record(keycode, record);
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/*
 * ,----------------------------------------------------------------------------------.
 * | Esc  |      |   W  |   E  |   R  |   T  |   Y  |   U  |  I  |   O  |   P  | Bspc |
 * |------+------+------+------+------+------+------+------+-----+------+------+------|
 * | Ctrl |   A  |   S  |   D  |   F  |   G  |   H  |   J  |  K  |   L  | dktn | DF(H)|
 * |------+------+------+------+------+------+------+------+-----+------+------+------|
 * | MO(H)|   Z  |   X  |   C  |   V  |   B  |   N  |   M  |  、 |   。 |   /  | Ent  |
 * |------+------+------+------+------+------+------+------+-----+------+------+------|
 * |                    | L-alt| MO(F)|    Space    |   +  | RGui|                    |
 * `----------------------------------------------------------------------------------'
 * ,----------------------------------------------------------------------------------.
 * | Esc  |      |      |  UP  |      |      |   1  |   2  |  3  |   0  |      | Bspc |
 * |------+------+------+------+------+------+------+------+-----+------+------+------|
 * | Ctrl |      | LEFT | DOWN | RIGHT|      |   4  |   5  |  6  |   L  |      | DF(H)|
 * |------+------+------+------+------+------+------+------+-----+------+------+------|
 * | Shift|      |   X  |   C  |   V  |      |   7  |   8  |  9  |   .  |      | Ent  |
 * |------+------+------+------+------+------+------+------+-----+------+------+------|
 * |                    | L-alt| MO(F)|    Space    | MO(F)|  ー |                    |
 * `----------------------------------------------------------------------------------'
 */
  [HIRAGANA] = LAYOUT(
    KC_ESC           , KC_NO        , KC_W   , UC(HRGN_E)   , KC_R    , KC_T           , KC_Y      , UC(HRGN_U)   , UC(HRGN_I)   , UC(HRGN_O)    , KC_P              , KC_BSPC        , 
    KC_LCTL          , UC(HRGN_A)   , KC_S   , KC_D         , KC_F    , KC_G           , KC_H      , KC_J         , KC_K         , KC_L          , UC(SYM_DAKUTEN)   , TO(KATAKANA)   , 
    MO(HIRAGANA_SUPP), KC_Z         , KC_NO  , KC_C         , KC_V    , KC_B           , UC(HRGN_N), KC_M         , UC(SYM_COMMA), UC(SYM_PERIOD), KC_SLSH           , RSFT_T(KC_ENT) , 
                                               KC_LALT      , MO(FN)  , KC_SPC         , MO(FN)    , KC_RGUI
  ),

  [HIRAGANA_SUPP] = LAYOUT(
    KC_ESC           , KC_NO        , KC_W   , UC(HRGN_E_SM), KC_R    , UC(HRGN_TSU_SM), KC_Y      , UC(HRGN_U_SM), UC(HRGN_I_SM), UC(HRGN_O_SM) , KC_P              , KC_BSPC        , 
    KC_LCTL          , UC(HRGN_A_SM), KC_S   , KC_D         , KC_F    , KC_G           , KC_H      , KC_J         , KC_K         , KC_L          , UC(SYM_HANDAKUTEN), DF(FN)         , 
    MO(HIRAGANA_SUPP), KC_Z         , KC_X   , KC_C         , KC_V    , KC_B           , UC(HRGN_N), KC_M         , UC(SYM_COMMA), UC(SYM_PERIOD), KC_SLSH           , RSFT_T(KC_ENT) , 
                                               KC_LALT      , KC_NO   , KC_SPC         , KC_NO     , KC_RGUI
  ),

  [KATAKANA] = LAYOUT(
    KC_ESC           , KC_NO        , KC_W   , UC(KTKN_E)   , KC_R    , KC_T           , KC_Y      , UC(KTKN_U)   , UC(KTKN_I)   , UC(KTKN_O)    , KC_P              , KC_BSPC        , 
    KC_LCTL          , UC(KTKN_A)   , KC_S   , KC_D         , KC_F    , KC_G           , KC_H      , KC_J         , KC_K         , KC_L          , UC(SYM_DAKUTEN)   , TO(HIRAGANA)   , 
    MO(KATAKANA_SUPP), KC_Z         , KC_NO  , KC_C         , KC_V    , KC_B           , UC(KTKN_N), KC_M         , UC(SYM_COMMA), UC(SYM_PERIOD), KC_SLSH           , RSFT_T(KC_ENT) , 
                                               KC_LALT      , MO(FN)  , KC_SPC         , MO(FN)    , KC_RGUI
  ),

  [KATAKANA_SUPP] = LAYOUT(
    KC_ESC           , KC_NO        , KC_W   , UC(KTKN_E_SM), KC_R    , UC(KTKN_TSU_SM), KC_Y      , UC(KTKN_U_SM), UC(KTKN_I_SM), UC(KTKN_O_SM) , KC_P              , KC_BSPC        , 
    KC_LCTL          , UC(KTKN_A_SM), KC_S   , KC_D         , KC_F    , KC_G           , KC_H      , KC_J         , KC_K         , KC_L          , UC(SYM_HANDAKUTEN), DF(FN)         , 
    MO(KATAKANA_SUPP), KC_Z         , KC_X   , KC_C         , KC_V    , KC_B           , UC(KTKN_N), KC_M         , UC(SYM_COMMA), UC(SYM_PERIOD), KC_SLSH           , RSFT_T(KC_ENT) , 
                                               KC_LALT      , KC_NO   , KC_SPC         , KC_NO     , KC_RGUI
  ),

  [FN] = LAYOUT(
    KC_ESC           , KC_NO        , KC_NO  , KC_UP        , KC_NO   , KC_NO        , UC(JP_NUM_1), UC(JP_NUM_2) , UC(JP_NUM_3) , UC(JP_NUM_10) , KC_NO             , KC_BSPC        ,
    KC_LCTL          , KC_NO        , KC_LEFT, KC_DOWN      , KC_RIGHT, KC_NO        , UC(JP_NUM_4), UC(JP_NUM_5) , UC(JP_NUM_6) , KC_NO         , KC_NO             , DF(HIRAGANA)   , 
    KC_LSFT          , KC_NO        , KC_X   , KC_C         , KC_V    , KC_NO        , UC(JP_NUM_7), UC(JP_NUM_9) , UC(JP_NUM_9) , KC_DOT        , KC_NO             , RSFT_T(KC_ENT) , 
                                               KC_LALT      , KC_TRNS , KC_SPC       , KC_TRNS     , UC(SYM_LONGVOW)
  ),

};
