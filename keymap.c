/* Copyright 2015-2021 Jack Humbert
 * Moonlander port by William Dizon <wdchromium@gmail.com>
 * QWERTY + HIRAGANA + KATAKANA -- developer layout
 *
 * Left outer column and bottom row carry layer/IME controls.
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
    PAREN_IN  = _JP_IME_SAFE_END, /* tap: ()  then left-arrow        */
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
#define SOFT_BLUE      0x00, 0x60, 0xFF
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
        case GUI_DEL:
            if (record->event.pressed) {
                ime_clear();
                gui_del_timer = timer_read();
            } else {
                if (timer_elapsed(gui_del_timer) < HOLD_DURATION) {
                    register_code(KC_LGUI);
                    tap_code(KC_DEL);
                    unregister_code(KC_LGUI);
                } else {
                    register_code(KC_LSFT);
                    register_code(KC_LGUI);
                    tap_code(KC_DEL);
                    unregister_code(KC_LGUI);
                    unregister_code(KC_LSFT);
                }
            }
            return false;

        case GUI_INS:
            if (record->event.pressed) {
                ime_clear();
                gui_ins_timer = timer_read();
            } else {
                if (timer_elapsed(gui_ins_timer) < HOLD_DURATION) {
                    register_code(KC_LGUI);
                    tap_code(KC_INS);
                    unregister_code(KC_LGUI);
                } else {
                    register_code(KC_LSFT);
                    register_code(KC_LGUI);
                    tap_code(KC_INS);
                    unregister_code(KC_LGUI);
                    unregister_code(KC_LSFT);
                }
            }
            return false;
    }

    /* press-only keycodes */
    if (record->event.pressed) {
        switch (keycode) {
            case PAREN_IN:
                ime_clear();
                SEND_STRING("()" SS_TAP(X_LEFT));
                return false;
            case SBRACK_IN:
                ime_clear();
                SEND_STRING("[]" SS_TAP(X_LEFT));
                return false;
            case CT_SF_C:
                ime_clear();
                SEND_STRING(SS_LCTL(SS_LSFT("c")));
                return false;
            case CT_SF_V:
                ime_clear();
                SEND_STRING(SS_LCTL(SS_LSFT("v")));
                return false;
            case QK_GESC:
            case KC_ENT:
            case KC_SPC:
                clear_lit();
                ime_reset_word_count();
                break;
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
                ime_reset_word_count();
                clear_lit();
                key_lit[record->event.key.row][record->event.key.col] = true;
                break;
            default:
                key_lit[record->event.key.row][record->event.key.col] = true;
                break;
        }
        switch (keycode) {
            case FW_EXLM: ime_clear(); send_unicode_string("！"); return false;
            case FW_QUES: ime_clear(); send_unicode_string("？"); return false;
            case FW_AT:   ime_clear(); send_unicode_string("＠"); return false;
            case FW_HASH: ime_clear(); send_unicode_string("＃"); return false;
        }
    }

    return ime_process_record(keycode, record);
}

/* ------------------------------------------------------------------ */
/* RGB indicators                                                       */
/* ------------------------------------------------------------------ */

extern const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS];

/* Keycode a key produces right now, resolving KC_TRNS through the active
 * layers (so predictions still show while SUPP is held). */
static uint16_t keycode_at(uint8_t row, uint8_t col) {
    for (int8_t l = get_highest_layer(layer_state); l >= 0; l--) {
        if (l != 0 && !layer_state_is(l)) continue;
        uint16_t kc = pgm_read_word(&keymaps[l][row][col]);
        if (kc != KC_TRNS) return kc;
    }
    return KC_NO;
}

static uint16_t count_digit_kc(uint8_t d) {
    switch (d) {
        case 1:  return UC(JP_NUM_1);
        case 2:  return UC(JP_NUM_2);
        case 3:  return UC(JP_NUM_3);
        case 4:  return UC(JP_NUM_4);
        case 5:  return UC(JP_NUM_5);
        case 6:  return UC(JP_NUM_6);
        case 7:  return UC(JP_NUM_7);
        case 8:  return UC(JP_NUM_8);
        case 9:  return UC(JP_NUM_9);
        case 0:  return UC(JP_NUM_10);
        default: return KC_NO;
    }
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

    if (is_ime) {
        uint8_t display = ime_get_word_count();
        if (display > 99) display = 99;
        uint8_t tens = display / 10;
        uint8_t ones = display % 10;
        for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
            for (uint8_t c2 = 0; c2 < MATRIX_COLS; c2++) {
                uint8_t idx = g_led_config.matrix_co[r][c2];
                if (idx < led_min || idx >= led_max || idx == NO_LED) continue;
                uint16_t kc = keycode_at(r, c2);
                if (tens > 0 && kc == count_digit_kc(tens)) {
                    rgb_matrix_set_color(idx, 0xFF, 0x00, 0x00);
                } else if (kc == count_digit_kc(ones)) {
                    if (tens == 0) {
                        rgb_matrix_set_color(idx, 0xFF, 0x00, 0x00);
                    } else {
                        rgb_matrix_set_color(idx, 0x00, 0x80, 0xFF);
                    }
                }
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
 * Row 5 (12): L[Ctl Alt GUI L1 L2 fan]    R[fan - = IME...      ]
 * Thumb ( 6): L[Spc --- ---]              R[--- --- Ent]
 *
 * Left outer col  = modifier / layer keys
 * Left inner col  = DEL / BSPC / ENT  (rows 1-3 only)
 * Right inner col = BSPC / BSPC / ENT (rows 1-3 only; row 2 unused on kana)
 * QWERTY only:
 *   Right outer col = PAREN_IN / SBRACK_IN / KC_QUOTE / KC_RSFT
 *   Row 5 left fan  = GUI_DEL (tap: GUI+Del, hold: Shift+GUI+Del)
 *   Row 5 right fan = GUI_INS (tap: GUI+Ins, hold: Shift+GUI+Ins)
 * Kana layers: see the HIRAGANA diagram (KATAKANA is identical).
 */

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* QWERTY
 * ,---------------------------------------------------.           ,--------------------------------------------------.
 * | ESC/` |   1  |   2  |   3  |   4  |   5   | Del   |           | Bspc  |   6  |   7  |   8  |   9  |   0  |  ()   |
 * |-------+------+------+------+------+-------+-------|           |-------+------+------+------+------+------+-------|
 * | Tab   |   Q  |   W  |   E  |   R  |   T   | Bspc  |           | Bspc  |   Y  |   U  |   I  |   O  |   P  |  []   |
 * |-------+------+------+------+------+-------+-------|           |-------+------+------+------+------+------+-------|
 * | MO(6) |   A  |   S  |   D  |   F  |   G   | Enter |           | Enter |   H  |   J  |   K  |   L  |   ;  |   '   |
 * |-------+------+------+------+------+-------|-------'           |-------+------+------+------+------+------+-------|
 * | Shift |   Z  |   X  |   C  |   V  |   B   |                           |   N  |   M  |   ,  |   .  |   /  | Shift |
 * `-------+------+------+------+------+-------+-------'           `-------+------+------+------+------+------+-------'
 * | Ctrl  | Alt  | GUI  |MO(6) |MO(5) |       |GUI_DEL|           |GUI_INS|      |  - _ |  = + | HRGA | KTKN |ENG_GO |
 * `-------+------+------+------+------+       +-------'           `-------+------+------+------+------+------+-------'
 *                              | Spc  | Copy  |Ct+Sf+C|           |Ct+Sf+V| Pste | Enter|
 *                              `------+-------+-------'           `-------+------+------'
 */
[QWERTY] = LAYOUT(
  QK_GESC , KC_1    , KC_2    , KC_3    , KC_4    , KC_5    , KC_DEL  ,        KC_BSPC , KC_6    , KC_7    , KC_8    , KC_9    , KC_0    , PAREN_IN ,
  KC_TAB  , KC_Q    , KC_W    , KC_E    , KC_R    , KC_T    , KC_BSPC ,        KC_BSPC , KC_Y    , KC_U    , KC_I    , KC_O    , KC_P    , SBRACK_IN,
  MO(GUIS), KC_A    , KC_S    , KC_D    , KC_F    , KC_G    , KC_ENT  ,        KC_ENT  , KC_H    , KC_J    , KC_K    , KC_L    , KC_SCLN , KC_QUOTE ,
  KC_LSFT , KC_Z    , KC_X    , KC_C    , KC_V    , KC_B    ,                            KC_N    , KC_M    , KC_COMM , KC_DOT  , KC_SLSH , KC_RSFT  ,
  KC_LCTL , KC_LALT , KC_LGUI , MO(GUIS), MO(FUNCS)         , GUI_DEL ,        GUI_INS ,           KC_MINS , KC_EQUAL, HRGA_GO , KTKN_GO , ENG_GO   ,
                                           KC_SPC  , KC_COPY, CT_SF_C ,        CT_SF_V , KC_PSTE , KC_ENT
),

/* HIRAGANA
 * Numbers row outputs kanji numerals (ichi ni san...).
 * Vowel keys (A E I O U) and n output hiragana directly.
 * Consonant keys are held by jp_ime.c as pending romaji until a vowel
 *   completes them (see README for the romaji rules).
 * X / L are the small-kana prefixes (xa -> ぁ, xtu -> っ, xwa -> ゎ).
 * Left Shift position = MO(HIRAGANA_SUPP) for small kana / symbols.
 *
 * ,--------------------------------------------------.           ,--------------------------------------------------.
 * | ESC   | ichi |  ni  |  san | yon  |  go  | Del   |           | Bspc  | roku | nana | hachi| ku   | juu  |  ---  |
 * |-------+------+------+------+------+------+-------|           |-------+------+------+------+------+------+-------|
 * | Tab   | ---  |   W  |  e   |   R  |   T  | Bspc  |           |  ---  |   Y  |   u  |  i   |  o   |   P  |  ---  |
 * |-------+------+------+------+------+------+-------|           |-------+------+------+------+------+------+-------|
 * | MO(6) |  a   |   S  |   D  |   F  |   G  | Enter |           | Enter |   H  |   J  |  K   |   L  |dakuten| ---  |
 * |-------+------+------+------+------+------|-------'           |-------+------+------+------+------+------+-------|
 * | MO(7) |   Z  |   X  |   C  |   V  |   B  |                           |   n  |   M  |  、  |  。  |   /  |  ---  |
 * `-------+------+------+------+------+------+-------'           `-------+------+------+------+------+------+-------'
 * | Ctrl  | Alt  | GUI  |MO(6) |MO(5) |      |  ---  |           |  ---  |      |  --- |lngvow| HRGA | KTKN |ENG_GO |
 * `-------+------+------+------+------+      +-------'           `-------+------+------+------+------+------+-------'
 *                              | Spc  | Copy |Ct+Sf+C|           |Ct+Sf+V| Pste | Enter|
 *                              `------+------+-------'           `-------+------+------'
 */
[HIRAGANA] = LAYOUT(
  QK_GESC          , UC(JP_NUM_1) , UC(JP_NUM_2) , UC(JP_NUM_3) , UC(JP_NUM_4) , UC(JP_NUM_5) , KC_DEL  ,        KC_BSPC        , UC(JP_NUM_6) , UC(JP_NUM_7) , UC(JP_NUM_8) , UC(JP_NUM_9) , UC(JP_NUM_10)   , KC_NO ,
  KC_TAB           , KC_NO        , KC_W         , UC(HRGN_E)   , KC_R         , KC_T         , KC_BSPC ,        KC_NO          , KC_Y         , UC(HRGN_U)   , UC(HRGN_I)   , UC(HRGN_O)   , KC_P            , KC_NO ,
  MO(GUIS)         , UC(HRGN_A)   , KC_S         , KC_D         , KC_F         , KC_G         , KC_ENT  ,        KC_ENT         , KC_H         , KC_J         , KC_K         , KC_L         , UC(SYM_DAKUTEN) , KC_NO ,
  MO(HIRAGANA_SUPP), KC_Z         , KC_X         , KC_C         , KC_V         , KC_B         ,                  UC(HRGN_N)     , KC_M         , UC(SYM_COMMA), UC(SYM_PERIOD), KC_SLSH     , KC_NO           ,
  KC_LCTL          , KC_LALT      , KC_LGUI      , MO(GUIS)     , MO(FUNCS)    ,                KC_NO   ,        KC_NO          ,                KC_NO        , UC(SYM_LONGVOW), HRGA_GO    , KTKN_GO         , ENG_GO,
                                           KC_SPC  , KC_COPY, CT_SF_C ,        CT_SF_V , KC_PSTE , KC_ENT
),

/* HIRAGANA_SUPP  (hold MO(HIRAGANA_SUPP) while in hiragana mode)
 * Provides small kana (small a e i o u, small tsu), bracketing symbols,
 * and supplemental punctuation. KC_TRNS falls through to HIRAGANA.
 * Row 1: 〜 ！ ＠ ＃ ¥ ... 「 」    Row 4: 〈 〉 ？
 */
[HIRAGANA_SUPP] = LAYOUT(
  UC(SYM_TILDE) , FW_EXLM       , FW_AT          , FW_HASH       , UC(SYM_YEN)    , KC_NO          , KC_TRNS ,        KC_NO   , KC_NO         , KC_NO         , KC_NO         , UC(SYM_KAKKO1) , UC(SYM_KAKKO2)    , KC_NO   ,
  KC_TRNS       , KC_TRNS       , KC_TRNS        , UC(HRGN_E_SM) , KC_TRNS        , UC(HRGN_TSU_SM), KC_TRNS ,        KC_TRNS , KC_TRNS       , UC(HRGN_U_SM) , UC(HRGN_I_SM) , UC(HRGN_O_SM)  , KC_TRNS           , KC_TRNS ,
  KC_NO         , UC(HRGN_A_SM) , KC_TRNS        , KC_TRNS       , KC_TRNS        , KC_TRNS        , KC_TRNS ,        KC_TRNS , KC_TRNS       , KC_TRNS       , KC_TRNS       , KC_TRNS        , UC(SYM_HANDAKUTEN), KC_TRNS ,
  KC_TRNS       , KC_TRNS       , KC_TRNS        , KC_TRNS       , KC_TRNS        , KC_TRNS        ,                  UC(HRGN_N), KC_TRNS     , UC(SYM_KAKKO3), UC(SYM_KAKKO4), FW_QUES        , KC_TRNS           ,
  KC_LCTL       , KC_TRNS       , KC_TRNS        , KC_TRNS       , KC_TRNS        ,                  KC_TRNS ,        KC_NO   ,                KC_TRNS        , UC(SYM_LONGVOW), UC_PREV       , UC_NEXT           , ENG_GO  ,
                                                   KC_TRNS       , KC_TRNS        , KC_TRNS        ,        KC_TRNS , KC_TRNS , KC_TRNS
),

/* KATAKANA
 * Same layout as HIRAGANA with katakana vowels / ン; consonant sequences
 * produce katakana. V (ヴ) and X / L (small kana) work in both layers.
 * Left Shift position = MO(KATAKANA_SUPP).
 */
[KATAKANA] = LAYOUT(
  QK_GESC          , UC(JP_NUM_1) , UC(JP_NUM_2) , UC(JP_NUM_3) , UC(JP_NUM_4) , UC(JP_NUM_5) , KC_DEL  ,        KC_BSPC        , UC(JP_NUM_6) , UC(JP_NUM_7) , UC(JP_NUM_8) , UC(JP_NUM_9) , UC(JP_NUM_10)   , KC_NO ,
  KC_TAB           , KC_NO        , KC_W         , UC(KTKN_E)   , KC_R         , KC_T         , KC_BSPC ,        KC_NO          , KC_Y         , UC(KTKN_U)   , UC(KTKN_I)   , UC(KTKN_O)   , KC_P            , KC_NO ,
  MO(GUIS)         , UC(KTKN_A)   , KC_S         , KC_D         , KC_F         , KC_G         , KC_ENT  ,        KC_ENT         , KC_H         , KC_J         , KC_K         , KC_L         , UC(SYM_DAKUTEN) , KC_NO ,
  MO(KATAKANA_SUPP), KC_Z         , KC_X         , KC_C         , KC_V         , KC_B         ,                  UC(KTKN_N)     , KC_M         , UC(SYM_COMMA), UC(SYM_PERIOD), KC_SLSH     , KC_NO           ,
  KC_LCTL          , KC_LALT      , KC_LGUI      , MO(GUIS)     , MO(FUNCS)    ,               KC_NO   ,        KC_NO          ,               KC_NO         , UC(SYM_LONGVOW), HRGA_GO     , KTKN_GO         , ENG_GO,
                                           KC_SPC  , KC_COPY, CT_SF_C ,        CT_SF_V , KC_PSTE , KC_ENT
),

/* KATAKANA_SUPP  (hold MO(KATAKANA_SUPP) while in katakana mode)
 * Small katakana and supplemental symbols; same positions as HIRAGANA_SUPP.
 */
[KATAKANA_SUPP] = LAYOUT(
  UC(SYM_TILDE) , FW_EXLM       , FW_AT          , FW_HASH       , UC(SYM_YEN)    , KC_NO          , KC_TRNS ,        KC_NO   , KC_NO         , KC_NO         , KC_NO         , UC(SYM_KAKKO1) , UC(SYM_KAKKO2)    , KC_NO   ,
  KC_TRNS       , KC_TRNS       , KC_TRNS        , UC(KTKN_E_SM) , KC_TRNS        , UC(KTKN_TSU_SM), KC_TRNS ,        KC_TRNS , KC_TRNS       , UC(KTKN_U_SM) , UC(KTKN_I_SM) , UC(KTKN_O_SM)  , KC_TRNS           , KC_TRNS ,
  KC_NO         , UC(KTKN_A_SM) , KC_TRNS        , KC_TRNS       , KC_TRNS        , KC_TRNS        , KC_TRNS ,        KC_TRNS , KC_TRNS       , KC_TRNS       , KC_TRNS       , KC_TRNS        , UC(SYM_HANDAKUTEN), KC_TRNS ,
  KC_TRNS       , KC_TRNS       , KC_TRNS        , KC_TRNS       , KC_TRNS        , KC_TRNS        ,                  UC(KTKN_N), KC_TRNS     , UC(SYM_KAKKO3), UC(SYM_KAKKO4), FW_QUES        , KC_TRNS ,
  KC_LCTL       , KC_TRNS       , KC_TRNS        , KC_TRNS       , KC_TRNS        ,                  KC_TRNS ,        KC_NO   ,                KC_TRNS        , UC(SYM_LONGVOW), UC_PREV       , UC_NEXT           , ENG_GO  ,
                                                   KC_TRNS       , KC_TRNS        , KC_TRNS        ,        KC_TRNS , KC_TRNS , KC_TRNS
),

/* FUNCS  (hold MO(5))
 * Function keys, arrows at ESDF homerow, numpad on right hand.
 */
[FUNCS] = LAYOUT(
  KC_ESC  , KC_F1   , KC_F2   , KC_F3   , KC_F4   , KC_F5   , KC_DEL  ,        KC_NO   , KC_F6   , KC_F7   , KC_F8   , KC_F9   , KC_F10  , KC_NO   ,
  KC_TAB  , KC_F11  , KC_F12  , KC_UP   , KC_NO   , KC_NO   , KC_BSPC ,        KC_NO   , KC_NO   , KC_P7   , KC_P8   , KC_P9   , KC_NO   , KC_NO   ,
  KC_CAPS , KC_NO   , KC_LEFT , KC_DOWN , KC_RGHT , KC_NO   , KC_ENT  ,        KC_NO   , KC_NO   , KC_P4   , KC_P5   , KC_P6   , KC_NO   , KC_NO   ,
  KC_LSFT , KC_NO   , KC_NO   , KC_NO   , KC_NO   , KC_NO   ,                            KC_NO   , KC_P1   , KC_P2   , KC_P3   , KC_NO   , KC_NO   ,
  KC_LCTL , KC_LALT , KC_NO   , KC_NO   , KC_TRNS ,           KC_NO   ,        KC_NO   ,           KC_MINS , KC_P0   , KC_PDOT , KC_NO   , KC_NO   ,
                                          KC_NO   , KC_NO   , KC_NO   ,        KC_NO   , KC_NO   , KC_ENT
),

/* GUIS  (hold MO(6))
 * Left-GUI window-manager shortcuts (i3 / similar).
 * Also the layer from which HRGA_GO and KTKN_GO are reachable
 * when accessed via MO(6)+key rather than the direct bottom-row keys.
 */
[GUIS] = LAYOUT(
  KC_GRV  , LGUI(KC_1)   , LGUI(KC_2)   , LGUI(KC_3)   , LGUI(KC_4)   , LGUI(KC_5)   , LGUI(KC_DEL) ,        KC_NO        , LGUI(KC_6)   , LGUI(KC_7)   , LGUI(KC_8)   , LGUI(KC_9)   , LGUI(KC_0)   , KC_NO        ,
  KC_NO   , KC_NO        , LGUI(KC_LBRC), LGUI(KC_UP)  , LGUI(KC_RBRC), KC_NO        , LGUI(KC_BSPC),        KC_NO        , KC_NO        , KC_NO        , KC_NO        , KC_LBRC      , KC_RBRC      , KC_NO        ,
  KC_TRNS , KC_NO        , LGUI(KC_LEFT), LGUI(KC_DOWN), LGUI(KC_RGHT), KC_NO        , LGUI(KC_ENT) ,        KC_NO        , KC_NO        , KC_NO        , KC_NO        , LGUI(KC_L)   , KC_QUOT      , KC_NO        ,
  KC_LSFT , KC_NO        , KC_NO        , KC_NO        , KC_NO        , KC_NO        ,                       KC_NO        , KC_NO        , KC_NO        , KC_NO        , KC_BSLS      , KC_NO        ,
  KC_LCTL , KC_LALT      , KC_NO        , KC_TRNS      , KC_TRNS                     , KC_NO        ,        KC_NO                       , KC_NO        , KC_NO        , HRGA_GO      , KTKN_GO      , LGUI(KC_END) ,
                                          KC_NO        , KC_NO        , KC_NO        ,        KC_NO        , KC_NO        , KC_NO
),

};
// clang-format on

