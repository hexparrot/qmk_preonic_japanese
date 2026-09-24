#pragma once
#include QMK_KEYBOARD_H

#define QWERTY 0
#define HIRAGANA 1
#define KATAKANA 2
#define FUNCS 5
#define GUIS 6
#define HIRAGANA_SUPP 7
#define KATAKANA_SUPP 8

#define TIMEOUT_MS 3000  // Pending romaji is dropped after this long.

enum {
  HRGA_GO = SAFE_RANGE,
  KTKN_GO,
  ENG_GO,
  _JP_IME_SAFE_END
};

// Lifecycle functions called from keymap.c hooks
void     ime_matrix_scan(void);
bool     ime_process_record(uint16_t keycode, keyrecord_t *record);

// Drops any pending romaji / ん / 1e state. Call from keycodes that
// return before ime_process_record() sees them.
void     ime_clear(void);

// RGB prediction. ime_has_pending() is true while a sequence is in
// progress; ime_accepts(kc) is true if pressing kc now would start
// (nothing pending) or continue/complete (something pending) a kana.
bool     ime_has_pending(void);
bool     ime_accepts(uint16_t keycode);

// Committed-kana word length counter.
void     ime_reset_word_count(void);
uint8_t  ime_get_word_count(void);
