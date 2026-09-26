#pragma once
#include QMK_KEYBOARD_H

enum layers {
  QWERTY,
  HIRAGANA,
  KATAKANA,
  HIRAGANA_SUPP,
  KATAKANA_SUPP,
  FUNCS,
  GUIS,
};

#define TIMEOUT_MS 3000  // Pending romaji is dropped after this long.

enum {
  HRGA_GO = SAFE_RANGE,
  KTKN_GO,
  ENG_GO,
  JP_IME_SAFE_END
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
