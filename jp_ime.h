#pragma once
#include QMK_KEYBOARD_H

#define QWERTY 0
#define HIRAGANA 1
#define KATAKANA 2
#define FUNCS 5
#define GUIS 6
#define HIRAGANA_SUPP 7
#define KATAKANA_SUPP 8

#define TIMEOUT_MS 3000  // Timeout in milliseconds.
#define RECENT_SIZE 3    // Number of keys in `recent` buffer.

enum {
  HRGA_GO = SAFE_RANGE,
  KTKN_GO,
  ENG_GO,
  _JP_IME_SAFE_END
};

// Lifecycle functions called from keymap.c hooks
void     ime_matrix_scan(void);
bool     ime_process_record(uint16_t keycode, keyrecord_t *record);

// Exposed so keymap.c can call clear if needed
void     clear_recent_keys(void);

// Exposes the two most-recent pending keys for RGB prediction.
// prev = recent[RECENT_SIZE-2], last = recent[RECENT_SIZE-1]
void     ime_get_pending(uint16_t *prev, uint16_t *last);

// Committed-kana word length counter.
void     ime_reset_word_count(void);
uint8_t  ime_get_word_count(void);
