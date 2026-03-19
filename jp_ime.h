#pragma once
#include QMK_KEYBOARD_H

#define HIRAGANA 0
#define KATAKANA 1
#define HIRAGANA_SUPP 2
#define KATAKANA_SUPP 3
#define FN 4

#define TIMEOUT_MS 3000  // Timeout in milliseconds.
#define RECENT_SIZE 3    // Number of keys in `recent` buffer.

enum {
  HRGA_GO = SAFE_RANGE,
  KTKN_GO,
  ENG_GO
};

// Lifecycle functions called from keymap.c hooks
void     ime_matrix_scan(void);
bool     ime_process_record(uint16_t keycode, keyrecord_t *record);

// Exposed so keymap.c can call clear if needed
void     clear_recent_keys(void);
