/* Copyright 2015-2021 Jack Humbert
 * Moonlander port by William Dizon <wdchromium@gmail.com>
 * QWERTY + HIRAGANA + KATAKANA -- developer layout
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 */

#include QMK_KEYBOARD_H
#include "jp_ime.h"

/* ------------------------------------------------------------------ */
/* Custom keycodes                                                      */
/* ------------------------------------------------------------------ */

enum custom_keycodes {
    PAREN_IN = JP_IME_SAFE_END, /* tap: ()  then left-arrow           */
    SBRACK_IN,              /* tap: []  then left-arrow              */
    CT_SF_C,                /* Ctrl+Shift+C                          */
    CT_SF_V,                /* Ctrl+Shift+V                          */
    GUI_DEL,                /* tap: GUI+Del   hold: Shift+GUI+Del    */
    GUI_INS,                /* tap: GUI+Ins   hold: Shift+GUI+Ins    */
    FW_EXLM,                /* ！  fullwidth; above UC()'s 15 bits   */
    FW_QUES,                /* ？                                    */
    FW_AT,                  /* ＠                                    */
    FW_HASH,                /* ＃                                    */
};

/* ------------------------------------------------------------------ */
/* Constants                                                            */
/* ------------------------------------------------------------------ */

#define HOLD_DURATION  200   /* ms threshold for tap vs hold          */
#define SOFT_BLUE      0x00, 0x60, 0xFF  /* keys pressed this word */
#define PREDICT_DIM    0x40, 0x20, 0x00  /* fresh-state starters   */
#define PREDICT_BRIGHT 0xFF, 0x90, 0x00  /* pending completions    */

/* ------------------------------------------------------------------ */
/* State                                                                */
/* ------------------------------------------------------------------ */

static bool     key_lit[MATRIX_ROWS][MATRIX_COLS] = {0};
static uint16_t gui_del_timer = 0;
static uint16_t gui_ins_timer = 0;

/* ------------------------------------------------------------------ */
/* Helpers                                                              */
/* ------------------------------------------------------------------ */

static void clear_lit(void) {
    memset(key_lit, 0, sizeof(key_lit));
}

/* Tap: GUI+key. Hold past HOLD_DURATION: Shift+GUI+key. */
static void gui_tap_hold(uint16_t *timer, keyrecord_t *record, uint8_t key) {
    if (record->event.pressed) {
        ime_clear();
        *timer = timer_read();
        return;
    }
    bool held = timer_elapsed(*timer) >= HOLD_DURATION;
    if (held) register_code(KC_LSFT);
    register_code(KC_LGUI);
    tap_code(key);
    unregister_code(KC_LGUI);
    if (held) unregister_code(KC_LSFT);
}

/* ------------------------------------------------------------------ */
/* Keyboard init                                                        */
/* ------------------------------------------------------------------ */

void keyboard_post_init_user(void) {
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_sethsv_noeeprom(HSV_OFF);
}

/* ------------------------------------------------------------------ */
/* Matrix scan                                                          */
/* ------------------------------------------------------------------ */

void matrix_scan_user(void) {
    ime_matrix_scan();
}

/* ------------------------------------------------------------------ */
/* Key processing                                                       */
/* ------------------------------------------------------------------ */

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    /* tap/hold keycodes -- must see both press and release */
    switch (keycode) {
        case GUI_DEL: gui_tap_hold(&gui_del_timer, record, KC_DEL); return false;
        case GUI_INS: gui_tap_hold(&gui_ins_timer, record, KC_INS); return false;
    }

    if (!record->event.pressed) {
        return ime_process_record(keycode, record);
    }

    /* press-only keycodes */
    switch (keycode) {
        case PAREN_IN:  ime_clear(); SEND_STRING("()" SS_TAP(X_LEFT));      return false;
        case SBRACK_IN: ime_clear(); SEND_STRING("[]" SS_TAP(X_LEFT));      return false;
        case CT_SF_C:   ime_clear(); SEND_STRING(SS_LCTL(SS_LSFT("c")));    return false;
        case CT_SF_V:   ime_clear(); SEND_STRING(SS_LCTL(SS_LSFT("v")));    return false;

        /* word boundaries: drop the blue trail */
        case QK_GESC:
        case KC_ENT:
        case KC_SPC:
            clear_lit();
            break;

        /* punctuation and layer switches start a new trail from this key */
        case FW_EXLM:
        case FW_QUES:
        case FW_AT:
        case FW_HASH:
        case UC(SYM_PERIOD):
        case UC(SYM_COMMA):
        case UC(SYM_KAKKO1):
        case UC(SYM_KAKKO2):
        case UC(SYM_KAKKO3):
        case UC(SYM_KAKKO4):
        case HRGA_GO:
        case KTKN_GO:
        case ENG_GO:
            clear_lit();
            key_lit[record->event.key.row][record->event.key.col] = true;
            break;

        default:
            key_lit[record->event.key.row][record->event.key.col] = true;
            break;
    }

    /* fullwidth symbols the UC() range can't reach */
    switch (keycode) {
        case FW_EXLM: ime_clear(); send_unicode_string("！"); return false;
        case FW_QUES: ime_clear(); send_unicode_string("？"); return false;
        case FW_AT:   ime_clear(); send_unicode_string("＠"); return false;
        case FW_HASH: ime_clear(); send_unicode_string("＃"); return false;
    }

    return ime_process_record(keycode, record);
}

/* ------------------------------------------------------------------ */
/* RGB indicators                                                       */
/* ------------------------------------------------------------------ */

/* Keycode a key produces right now, with KC_TRNS resolved through the
 * active layers (so predictions still show while SUPP is held). */
static uint16_t keycode_at(uint8_t row, uint8_t col) {
    keypos_t key = { .row = row, .col = col };
    return keymap_key_to_keycode(layer_switch_get_layer(key), key);
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    bool is_ime  = IS_LAYER_ON(HIRAGANA) || IS_LAYER_ON(KATAKANA);
    bool pending = is_ime && ime_has_pending();

    /* Prediction comes from the IME's own romaji table (ime_accepts), so the
     * lights can't disagree with what the matcher will do. */
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint8_t index = g_led_config.matrix_co[row][col];

            if (index < led_min || index >= led_max || index == NO_LED) {
                continue;
            }

            if (key_lit[row][col]) {
                rgb_matrix_set_color(index, SOFT_BLUE);
            } else if (is_ime && ime_accepts(keycode_at(row, col))) {
                if (pending) {
                    rgb_matrix_set_color(index, PREDICT_BRIGHT);
                } else {
                    rgb_matrix_set_color(index, PREDICT_DIM);
                }
            } else {
                rgb_matrix_set_color(index, RGB_OFF);
            }
        }
    }

    return false;
}

/* ------------------------------------------------------------------ */
/* Keymaps                                                              */
/* ------------------------------------------------------------------ */

/* Moonlander LAYOUT key order (72 keys):
 *
 * Row 1 (14): L[outer 1 2 3 4 5 inner]    R[inner 6 7 8 9 0 outer]
 * Row 2 (14): L[outer Q W E R T inner]    R[inner Y U I O P outer]
 * Row 3 (14): L[outer A S D F G inner]    R[inner H J K L ; outer]
 * Row 4 (12): L[outer Z X C V B      ]    R[     N M , . / outer]
 * Row 5 (12): L[Ctl Alt GUI GUIS FUNCS fan] R[fan - = HRGA KTKN ENG]
 * Thumb ( 6): L[Spc Copy Ct+Sf+C]        R[Ct+Sf+V Pste Ent]
 *
 * Left inner col  = Del / Bspc / Enter (rows 1-3)
 * Right inner col = Bspc / Bspc / Enter (rows 1-3)
 * Right outer col = ( )  [ ]  '  Shift on QWERTY;
 *                   「   」   ゜  ---   on the kana layers
 * Fans            = GUI_DEL / GUI_INS on QWERTY, unused on the kana layers
 *
 * The kana layers only spell out the keys that differ from QWERTY; the
 * chrome (Esc, Tab, modifiers, MO() keys, thumbs) is KC_TRNS. The SUPP
 * layers are KC_TRNS everywhere except the small kana and symbols.
 */

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* QWERTY
 * ,---------------------------------------------------.           ,--------------------------------------------------.
 * | ESC/` |   1  |   2  |   3  |   4  |   5   | Del   |           | Bspc  |   6  |   7  |   8  |   9  |   0  |  ()   |
 * |-------+------+------+------+------+-------+-------|           |-------+------+------+------+------+------+-------|
 * | Tab   |   Q  |   W  |   E  |   R  |   T   | Bspc  |           | Bspc  |   Y  |   U  |   I  |   O  |   P  |  []   |
 * |-------+------+------+------+------+-------+-------|           |-------+------+------+------+------+------+-------|
 * | GUIS  |   A  |   S  |   D  |   F  |   G   | Enter |           | Enter |   H  |   J  |   K  |   L  |   ;  |   '   |
 * |-------+------+------+------+------+-------|-------'           |-------+------+------+------+------+------+-------|
 * | Shift |   Z  |   X  |   C  |   V  |   B   |                           |   N  |   M  |   ,  |   .  |   /  | Shift |
 * `-------+------+------+------+------+-------+-------'           `-------+------+------+------+------+------+-------'
 * | Ctrl  | Alt  | GUI  | GUIS |FUNCS |       |GUI_DEL|           |GUI_INS|      |  - _ |  = + | HRGA | KTKN |ENG_GO |
 * `-------+------+------+------+------+       +-------'           `-------+------+------+------+------+------+-------'
 *                              | Spc  | Copy  |Ct+Sf+C|           |Ct+Sf+V| Pste | Enter|
 *                              `------+-------+-------'           `-------+------+------'
 */
[QWERTY] = LAYOUT(
  QK_GESC , KC_1    , KC_2    , KC_3    , KC_4    , KC_5    , KC_DEL  ,        KC_BSPC , KC_6    , KC_7    , KC_8    , KC_9    , KC_0    , PAREN_IN ,
  KC_TAB  , KC_Q    , KC_W    , KC_E    , KC_R    , KC_T    , KC_BSPC ,        KC_BSPC , KC_Y    , KC_U    , KC_I    , KC_O    , KC_P    , SBRACK_IN,
  MO(GUIS), KC_A    , KC_S    , KC_D    , KC_F    , KC_G    , KC_ENT  ,        KC_ENT  , KC_H    , KC_J    , KC_K    , KC_L    , KC_SCLN , KC_QUOT  ,
  KC_LSFT , KC_Z    , KC_X    , KC_C    , KC_V    , KC_B    ,                            KC_N    , KC_M    , KC_COMM , KC_DOT  , KC_SLSH , KC_RSFT  ,
  KC_LCTL , KC_LALT , KC_LGUI , MO(GUIS), MO(FUNCS)         , GUI_DEL ,        GUI_INS ,           KC_MINS , KC_EQL  , HRGA_GO , KTKN_GO , ENG_GO   ,
                                           KC_SPC  , KC_COPY, CT_SF_C ,        CT_SF_V , KC_PSTE , KC_ENT
),

/* HIRAGANA
 * Number row outputs kanji numerals (ichi ni san...).
 * Vowel keys (A E I O U) and n output hiragana directly.
 * Consonant keys are held by jp_ime.c as pending romaji until a vowel
 *   completes them (see README for the romaji rules).
 * X / L are the small-kana prefixes (xa -> ぁ, xtu -> っ, xwa -> ゎ).
 * Left Shift position = MO(HIRAGANA_SUPP) for small kana / symbols.
 * Right outer column = 「 」 ゜ ; - is ー.
 * Everything not shown is KC_TRNS to QWERTY (fans and right Shift are off).
 *
 * ,--------------------------------------------------.           ,--------------------------------------------------.
 * | ESC   | ichi |  ni  |  san | yon  |  go  | Del   |           | Bspc  | roku | nana | hachi| ku   | juu  |  「   |
 * |-------+------+------+------+------+------+-------|           |-------+------+------+------+------+------+-------|
 * | Tab   | ---  |   W  |  e   |   R  |   T  | Bspc  |           | Bspc  |   Y  |   u  |  i   |  o   |   P  |  」   |
 * |-------+------+------+------+------+------+-------|           |-------+------+------+------+------+------+-------|
 * | GUIS  |  a   |   S  |   D  |   F  |   G  | Enter |           | Enter |   H  |   J  |  K   |   L  |  ゛  |  ゜   |
 * |-------+------+------+------+------+------|-------'           |-------+------+------+------+------+------+-------|
 * | SUPP  |   Z  |   X  |   C  |   V  |   B  |                           |   n  |   M  |  、  |  。  |   /  |  ---  |
 * `-------+------+------+------+------+------+-------'           `-------+------+------+------+------+------+-------'
 * | Ctrl  | Alt  | GUI  | GUIS |FUNCS |      |  ---  |           |  ---  |      |  ー  |  --- | HRGA | KTKN |ENG_GO |
 * `-------+------+------+------+------+      +-------'           `-------+------+------+------+------+------+-------'
 *                              | Spc  | Copy |Ct+Sf+C|           |Ct+Sf+V| Pste | Enter|
 *                              `------+------+-------'           `-------+------+------'
 */
[HIRAGANA] = LAYOUT(
  KC_TRNS          , UC(JP_NUM_1) , UC(JP_NUM_2) , UC(JP_NUM_3) , UC(JP_NUM_4) , UC(JP_NUM_5) , KC_TRNS ,        KC_TRNS        , UC(JP_NUM_6) , UC(JP_NUM_7) , UC(JP_NUM_8) , UC(JP_NUM_9) , UC(JP_NUM_10)   , UC(SYM_KAKKO1)    ,
  KC_TRNS          , KC_NO        , KC_W         , UC(HRGN_E)   , KC_R         , KC_T         , KC_TRNS ,        KC_TRNS        , KC_Y         , UC(HRGN_U)   , UC(HRGN_I)   , UC(HRGN_O)   , KC_P            , UC(SYM_KAKKO2)    ,
  KC_TRNS          , UC(HRGN_A)   , KC_S         , KC_D         , KC_F         , KC_G         , KC_TRNS ,        KC_TRNS        , KC_H         , KC_J         , KC_K         , KC_L         , UC(SYM_DAKUTEN) , UC(SYM_HANDAKUTEN),
  MO(HIRAGANA_SUPP), KC_Z         , KC_X         , KC_C         , KC_V         , KC_B         ,                  UC(HRGN_N)     , KC_M         , UC(SYM_COMMA), UC(SYM_PERIOD), KC_SLSH     , KC_NO             ,
  KC_TRNS          , KC_TRNS      , KC_TRNS      , KC_TRNS      , KC_TRNS      ,                KC_NO   ,        KC_NO          ,                UC(SYM_LONGVOW), KC_NO      , KC_TRNS      , KC_TRNS         , KC_TRNS           ,
                                           KC_TRNS , KC_TRNS, KC_TRNS ,        KC_TRNS , KC_TRNS , KC_TRNS
),

/* HIRAGANA_SUPP  (hold the left Shift position while in hiragana mode)
 * Small kana (ぁぃぅぇぉっ), symbols and Unicode-mode cycling.
 * Everything else is KC_TRNS to HIRAGANA.
 *
 * Row 1: 〜 on Esc, ！ ＠ ＃ ¥ on 1-4, 「 」 on 9 / 0
 * Row 2: ぇ on E, っ on T, ぅ ぃ ぉ on U I O
 * Row 3: ぁ on A, ゜ on ;
 * Row 4: 〈 〉 on 、 。, ？ on /
 * Row 5: UC_PREV / UC_NEXT on HRGA / KTKN
 */
[HIRAGANA_SUPP] = LAYOUT(
  UC(SYM_TILDE) , FW_EXLM       , FW_AT          , FW_HASH       , UC(SYM_YEN)    , KC_TRNS        , KC_TRNS ,        KC_TRNS , KC_TRNS       , KC_TRNS       , KC_TRNS       , UC(SYM_KAKKO1) , UC(SYM_KAKKO2)    , KC_TRNS ,
  KC_TRNS       , KC_TRNS       , KC_TRNS        , UC(HRGN_E_SM) , KC_TRNS        , UC(HRGN_TSU_SM), KC_TRNS ,        KC_TRNS , KC_TRNS       , UC(HRGN_U_SM) , UC(HRGN_I_SM) , UC(HRGN_O_SM)  , KC_TRNS           , KC_TRNS ,
  KC_TRNS       , UC(HRGN_A_SM) , KC_TRNS        , KC_TRNS       , KC_TRNS        , KC_TRNS        , KC_TRNS ,        KC_TRNS , KC_TRNS       , KC_TRNS       , KC_TRNS       , KC_TRNS        , UC(SYM_HANDAKUTEN), KC_TRNS ,
  KC_TRNS       , KC_TRNS       , KC_TRNS        , KC_TRNS       , KC_TRNS        , KC_TRNS        ,                  KC_TRNS , KC_TRNS       , UC(SYM_KAKKO3), UC(SYM_KAKKO4), FW_QUES        , KC_TRNS           ,
  KC_TRNS       , KC_TRNS       , KC_TRNS        , KC_TRNS       , KC_TRNS        ,                  KC_TRNS ,        KC_TRNS ,                 KC_TRNS       , KC_TRNS       , UC_PREV        , UC_NEXT           , KC_TRNS ,
                                                   KC_TRNS       , KC_TRNS        , KC_TRNS        ,        KC_TRNS , KC_TRNS , KC_TRNS
),

/* KATAKANA
 * Same layout as HIRAGANA with katakana vowels / ン; consonant sequences
 * produce katakana. V (ヴ) and X / L (small kana) work in both layers.
 * Left Shift position = MO(KATAKANA_SUPP).
 */
[KATAKANA] = LAYOUT(
  KC_TRNS          , UC(JP_NUM_1) , UC(JP_NUM_2) , UC(JP_NUM_3) , UC(JP_NUM_4) , UC(JP_NUM_5) , KC_TRNS ,        KC_TRNS        , UC(JP_NUM_6) , UC(JP_NUM_7) , UC(JP_NUM_8) , UC(JP_NUM_9) , UC(JP_NUM_10)   , UC(SYM_KAKKO1)    ,
  KC_TRNS          , KC_NO        , KC_W         , UC(KTKN_E)   , KC_R         , KC_T         , KC_TRNS ,        KC_TRNS        , KC_Y         , UC(KTKN_U)   , UC(KTKN_I)   , UC(KTKN_O)   , KC_P            , UC(SYM_KAKKO2)    ,
  KC_TRNS          , UC(KTKN_A)   , KC_S         , KC_D         , KC_F         , KC_G         , KC_TRNS ,        KC_TRNS        , KC_H         , KC_J         , KC_K         , KC_L         , UC(SYM_DAKUTEN) , UC(SYM_HANDAKUTEN),
  MO(KATAKANA_SUPP), KC_Z         , KC_X         , KC_C         , KC_V         , KC_B         ,                  UC(KTKN_N)     , KC_M         , UC(SYM_COMMA), UC(SYM_PERIOD), KC_SLSH     , KC_NO             ,
  KC_TRNS          , KC_TRNS      , KC_TRNS      , KC_TRNS      , KC_TRNS      ,                KC_NO   ,        KC_NO          ,                UC(SYM_LONGVOW), KC_NO      , KC_TRNS      , KC_TRNS         , KC_TRNS           ,
                                           KC_TRNS , KC_TRNS, KC_TRNS ,        KC_TRNS , KC_TRNS , KC_TRNS
),

/* KATAKANA_SUPP  (hold the left Shift position while in katakana mode)
 * Small katakana and symbols; same positions as HIRAGANA_SUPP.
 */
[KATAKANA_SUPP] = LAYOUT(
  UC(SYM_TILDE) , FW_EXLM       , FW_AT          , FW_HASH       , UC(SYM_YEN)    , KC_TRNS        , KC_TRNS ,        KC_TRNS , KC_TRNS       , KC_TRNS       , KC_TRNS       , UC(SYM_KAKKO1) , UC(SYM_KAKKO2)    , KC_TRNS ,
  KC_TRNS       , KC_TRNS       , KC_TRNS        , UC(KTKN_E_SM) , KC_TRNS        , UC(KTKN_TSU_SM), KC_TRNS ,        KC_TRNS , KC_TRNS       , UC(KTKN_U_SM) , UC(KTKN_I_SM) , UC(KTKN_O_SM)  , KC_TRNS           , KC_TRNS ,
  KC_TRNS       , UC(KTKN_A_SM) , KC_TRNS        , KC_TRNS       , KC_TRNS        , KC_TRNS        , KC_TRNS ,        KC_TRNS , KC_TRNS       , KC_TRNS       , KC_TRNS       , KC_TRNS        , UC(SYM_HANDAKUTEN), KC_TRNS ,
  KC_TRNS       , KC_TRNS       , KC_TRNS        , KC_TRNS       , KC_TRNS        , KC_TRNS        ,                  KC_TRNS , KC_TRNS       , UC(SYM_KAKKO3), UC(SYM_KAKKO4), FW_QUES        , KC_TRNS           ,
  KC_TRNS       , KC_TRNS       , KC_TRNS        , KC_TRNS       , KC_TRNS        ,                  KC_TRNS ,        KC_TRNS ,                 KC_TRNS       , KC_TRNS       , UC_PREV        , UC_NEXT           , KC_TRNS ,
                                                   KC_TRNS       , KC_TRNS        , KC_TRNS        ,        KC_TRNS , KC_TRNS , KC_TRNS
),

/* FUNCS  (hold MO(FUNCS), bottom row)
 * Function keys, arrows at ESDF homerow, numpad on right hand.
 */
[FUNCS] = LAYOUT(
  KC_ESC  , KC_F1   , KC_F2   , KC_F3   , KC_F4   , KC_F5   , KC_DEL  ,        KC_NO   , KC_F6   , KC_F7   , KC_F8   , KC_F9   , KC_F10  , KC_NO   ,
  KC_TAB  , KC_F11  , KC_F12  , KC_UP   , KC_NO   , KC_NO   , KC_BSPC ,        KC_NO   , KC_NO   , KC_P7   , KC_P8   , KC_P9   , KC_NO   , KC_NO   ,
  KC_CAPS , KC_NO   , KC_LEFT , KC_DOWN , KC_RGHT , KC_NO   , KC_ENT  ,        KC_NO   , KC_NO   , KC_P4   , KC_P5   , KC_P6   , KC_NO   , KC_NO   ,
  KC_LSFT , KC_NO   , KC_NO   , KC_NO   , KC_NO   , KC_NO   ,                            KC_NO   , KC_P1   , KC_P2   , KC_P3   , KC_NO   , KC_NO   ,
  KC_LCTL , KC_LALT , KC_NO   , KC_NO   , KC_TRNS ,           KC_NO   ,        KC_NO   ,           KC_MINS , KC_P0   , KC_PDOT , KC_NO   , KC_NO   ,
                                          KC_SPC  , KC_NO   , KC_NO   ,        KC_NO   , KC_NO   , KC_ENT
),

/* GUIS  (hold MO(GUIS), left outer home row or bottom row)
 * Left-GUI window-manager shortcuts (i3 / similar), plus the plain
 * ` [ ] \ keys that QWERTY hides behind PAREN_IN / SBRACK_IN.
 * HRGA_GO / KTKN_GO are also reachable here.
 */
[GUIS] = LAYOUT(
  KC_GRV  , LGUI(KC_1)   , LGUI(KC_2)   , LGUI(KC_3)   , LGUI(KC_4)   , LGUI(KC_5)   , LGUI(KC_DEL) ,        KC_NO        , LGUI(KC_6)   , LGUI(KC_7)   , LGUI(KC_8)   , LGUI(KC_9)   , LGUI(KC_0)   , KC_NO        ,
  KC_NO   , KC_NO        , LGUI(KC_LBRC), LGUI(KC_UP)  , LGUI(KC_RBRC), KC_NO        , LGUI(KC_BSPC),        KC_NO        , KC_NO        , KC_NO        , KC_NO        , KC_LBRC      , KC_RBRC      , KC_NO        ,
  KC_TRNS , KC_NO        , LGUI(KC_LEFT), LGUI(KC_DOWN), LGUI(KC_RGHT), KC_NO        , LGUI(KC_ENT) ,        KC_NO        , KC_NO        , KC_NO        , KC_NO        , LGUI(KC_L)   , KC_TRNS      , KC_NO        ,
  KC_LSFT , KC_NO        , KC_NO        , KC_NO        , KC_NO        , KC_NO        ,                       KC_NO        , KC_NO        , KC_NO        , KC_NO        , KC_BSLS      , KC_NO        ,
  KC_LCTL , KC_LALT      , KC_NO        , KC_TRNS      , KC_TRNS                     , KC_NO        ,        KC_NO                       , KC_NO        , KC_NO        , HRGA_GO      , KTKN_GO      , LGUI(KC_END) ,
                                          KC_NO        , KC_NO        , KC_NO        ,        KC_NO        , KC_NO        , KC_NO
),

};
// clang-format on
