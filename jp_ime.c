#include "jp_ime.h"
// Start Recent Key Rememering:
// https://getreuer.info/posts/keyboards/triggers/index.html#based-on-previously-typed-keys
#include <string.h>

static uint16_t recent[RECENT_SIZE] = {KC_NO};
static uint16_t deadline = 0;

static uint8_t  ime_char_count = 0;

static uint8_t utf8_codepoint_count(const char *s) {
    uint8_t n = 0;
    for (; *s; s++) if (((uint8_t)*s & 0xC0) != 0x80) n++;
    return n;
}

static void kana_out(const char *s) {
    ime_char_count += utf8_codepoint_count(s);
    send_unicode_string(s);
}

void ime_reset_word_count(void) { ime_char_count = 0; }
uint8_t ime_get_word_count(void) { return ime_char_count; }

void clear_recent_keys(void) {
  memset(recent, 0, sizeof(recent));  // Set all zeros (KC_NO).
}

void ime_get_pending(uint16_t *prev, uint16_t *last) {
  *prev = recent[RECENT_SIZE - 2];
  *last = recent[RECENT_SIZE - 1];
}

// --- Matrix scan (timeout) ---
void ime_matrix_scan(void) {
    if (recent[RECENT_SIZE - 1] && timer_expired(timer_read(), deadline)) {
        clear_recent_keys();
    }
}

// Handles one event. Returns true if the key was appended to `recent`.
static bool update_recent_keys(uint16_t keycode, keyrecord_t* record) {
  if (!record->event.pressed) { return false; }
 
  if (((get_mods() | get_oneshot_mods()) & ~MOD_MASK_SHIFT) != 0) {
    clear_recent_keys();  // Avoid interfering with hotkeys.
    return false;
  }

  switch (keycode) {
    case KC_A ... KC_SLASH:  // These keys type letters, digits, symbols.
      break;
    case UC(0x3040) ... UC(0x30FF): // Hiragana + Katakana
    case UC(0x4E00) ... UC(0x767E): // plus numerals
      break;
    case KC_LSFT:  // These keys don't type anything on their own.
    case KC_RSFT:
    case QK_ONE_SHOT_MOD ... QK_ONE_SHOT_MOD_MAX:
      return false;

    default:  // Avoid acting otherwise, particularly on navigation keys.
      clear_recent_keys();
      return false;
  }

  // Slide the buffer left by one element.
  memmove(recent, recent + 1, (RECENT_SIZE - 1) * sizeof(*recent));

  recent[RECENT_SIZE - 1] = keycode;
  deadline = record->event.time + TIMEOUT_MS;
  return true;
}

bool ime_process_record(uint16_t keycode, keyrecord_t *record) {
  // Pass Ctrl+everything through before any layer or IME logic
  if (record->event.pressed && (get_mods() & MOD_MASK_CTRL)) {
    return true;  // Let QMK handle it normally
  }

  if (update_recent_keys(keycode, record)) {
    if (record->event.pressed) {
      if (IS_LAYER_ON(HIRAGANA) ) {

        // K - SERIES
        if (recent[RECENT_SIZE - 3] == KC_K) {
          if (recent[RECENT_SIZE - 2] == KC_K) {
            // MATCH KK_
            switch (keycode) {
            case UC(HRGN_A):
              kana_out("っか");
              break;
            case UC(HRGN_E):
              kana_out("っけ");
              break;
            case UC(HRGN_I):
              kana_out("っき");
              break;
            case UC(HRGN_O):
              kana_out("っこ");
              break;
            case UC(HRGN_U):
              kana_out("っく");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH KY_
            switch (keycode) {
            case UC(HRGN_A):
              kana_out("きゃ");
              break;
            case UC(HRGN_O):
              kana_out("きょ");
              break;
            case UC(HRGN_U):
              kana_out("きゅ");
              break;
            }
          }
          // any unmatched k** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_K) {
          // if K isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(HRGN_A):
            kana_out("か");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            kana_out("け");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            kana_out("き");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            kana_out("こ");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            kana_out("く");
            clear_recent_keys();
            break;
          case KC_K:
          case KC_Y:
            // K,Y exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched k* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // G - SERIES
        if (recent[RECENT_SIZE - 3] == KC_G) {
          if (recent[RECENT_SIZE - 2] == KC_G) {
            // MATCH GG_
            switch (keycode) {
            case UC(HRGN_A):
              kana_out("っぎ");
              break;
            case UC(HRGN_E):
              kana_out("っげ");
              break;
            case UC(HRGN_I):
              kana_out("っぎ");
              break;
            case UC(HRGN_O):
              kana_out("っご");
              break;
            case UC(HRGN_U):
              kana_out("っぐ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH GY_
            switch (keycode) {
            case UC(HRGN_A):
              kana_out("ぎゃ");
              break;
            case UC(HRGN_O):
              kana_out("ぎょ");
              break;
            case UC(HRGN_U):
              kana_out("ぎゅ");
              break;
            }
          }
          // any unmatched g** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_G) {
          // if G isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(HRGN_A):
            kana_out("が");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            kana_out("げ");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            kana_out("ぎ");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            kana_out("ご");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            kana_out("ぐ");
            clear_recent_keys();
            break;
          case KC_G:
          case KC_Y:
            // G,Y exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched g* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // T - SERIES
        // Position T-Series before S-Series to ensure TSU can be captured.
        if (recent[RECENT_SIZE - 3] == KC_T) {
          if (recent[RECENT_SIZE - 2] == KC_T) {
            // MATCH TT_
            switch (keycode) {
            case UC(HRGN_A):
              kana_out("った");
              break;
            case UC(HRGN_E):
              kana_out("って");
              break;
            case UC(HRGN_I):
              kana_out("っち");
              break;
            case UC(HRGN_O):
              kana_out("っと");
              break;
            case UC(HRGN_U):
              kana_out("っつ");
              break;
            case KC_S:
              kana_out("っつ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_S) {
            // MATCH TS_
            switch (keycode) {
            case UC(HRGN_U):
              kana_out("つ");
              break;
            case UC(HRGN_U_SM):
              kana_out("っ");
              break;
            default:
              kana_out("っ");
              break;
            }
          }
          // any unmatched t** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_T) {
          // if T isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(HRGN_A):
            kana_out("た");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            kana_out("て");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            kana_out("ち");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            kana_out("と");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            kana_out("つ");
            clear_recent_keys();
            break;
          case KC_T:
          case KC_S:
            // T,S exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched t* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // S - SERIES
        if (recent[RECENT_SIZE - 3] == KC_S) {
          if (recent[RECENT_SIZE - 2] == KC_S) {
            // MATCH SS_
            switch (keycode) {
            case UC(HRGN_A):
              kana_out("っさ");
              break;
            case UC(HRGN_E):
              kana_out("っせ");
              break;
            case UC(HRGN_I):
              kana_out("っし");
              break;
            case UC(HRGN_O):
              kana_out("っそ");
              break;
            case UC(HRGN_U):
              kana_out("っす");
              break;
            case KC_H:
              kana_out("っし");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_H) {
            // MATCH SH_
            switch (keycode) {
            case UC(HRGN_A):
              kana_out("しゃ");
              break;
            case UC(HRGN_I):
              kana_out("し");
              break;
            case UC(HRGN_O):
              kana_out("しょ");
              break;
            case UC(HRGN_U):
              kana_out("しゅ");
              break;
            }
          }
          // any unmatched s** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_S) {
          // if S isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(HRGN_A):
            kana_out("さ");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            kana_out("せ");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            kana_out("し");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            kana_out("そ");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            kana_out("す");
            clear_recent_keys();
            break;
          case KC_S:
          case KC_H:
            // S,H exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched s* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // Z - SERIES
        if (recent[RECENT_SIZE - 3] == KC_Z) {
          if (recent[RECENT_SIZE - 2] == KC_Z) {
            // MATCH ZZ_
            switch (keycode) {
            case UC(HRGN_A):
              kana_out("っざ");
              break;
            case UC(HRGN_E):
              kana_out("っぜ");
              break;
            case UC(HRGN_I):
              kana_out("っじ");
              break;
            case UC(HRGN_O):
              kana_out("っぞ");
              break;
            case UC(HRGN_U):
              kana_out("っず");
              break;
            }
          }
          // any unmatched z** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_Z) {
          // if Z isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(HRGN_A):
            kana_out("ざ");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            kana_out("ぜ");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            kana_out("じ");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            kana_out("ぞ");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            kana_out("ず");
            clear_recent_keys();
            break;
          case KC_Z:
            // Z exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched z* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // J - SERIES
        if (recent[RECENT_SIZE - 3] == KC_J) {
          if (recent[RECENT_SIZE - 2] == KC_J) {
            // MATCH JJ_
            switch (keycode) {
            case UC(HRGN_A):
              kana_out("っじゃ");
              break;
            case UC(HRGN_O):
              kana_out("っじょ");
              break;
            case UC(HRGN_U):
              kana_out("っじゅ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH JY_
            switch (keycode) {
            case UC(HRGN_A):
              kana_out("じゃ");
              break;
            case UC(HRGN_O):
              kana_out("じょ");
              break;
            case UC(HRGN_U):
              kana_out("じゅ");
              break;
            }
          }
          // any unmatched j** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_J) {
          // if J isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(HRGN_A):
            kana_out("じゃ");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            kana_out("じ");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            kana_out("じょ");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            kana_out("じゅ");
            clear_recent_keys();
            break;
          case KC_J:
          case KC_Y:
            // J,Y exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched j* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // C - SERIES
        if (recent[RECENT_SIZE - 3] == KC_C) {
          if (recent[RECENT_SIZE - 2] == KC_H) {
            // MATCH CH_
            switch (keycode) {
            case UC(HRGN_A):
              kana_out("ちゃ");
              break;
            case UC(HRGN_I):
              kana_out("ち");
              break;
            case UC(HRGN_O):
              kana_out("ちょ");
              break;
            case UC(HRGN_U):
              kana_out("ちゅ");
              break;
            }
          }
          // any unmatched c** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        }

        // D - SERIES
        if (recent[RECENT_SIZE - 3] == KC_D) {
          if (recent[RECENT_SIZE - 2] == KC_D) {
            // MATCH DD_
            switch (keycode) {
            case UC(HRGN_A):
              kana_out("っだ");
              break;
            case UC(HRGN_E):
              kana_out("っで");
              break;
            case UC(HRGN_I):
              kana_out("っぢ");
              break;
            case UC(HRGN_O):
              kana_out("っど");
              break;
            case UC(HRGN_U):
              kana_out("っづ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Z) {
            // MATCH DZ_
            switch (keycode) {
            case UC(HRGN_U):
              kana_out("っづ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_J) {
            // MATCH DJ_
            switch (keycode) {
            case UC(HRGN_I):
              kana_out("ぢ");
              break;
            }
          }
          // any unmatched d** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_D) {
          // if D isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(HRGN_A):
            kana_out("だ");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            kana_out("で");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            kana_out("ぢ");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            kana_out("ど");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            kana_out("づ");
            clear_recent_keys();
            break;
          case KC_D:
          case KC_Z:
          case KC_J:
            // D,Z,J exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched d* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // N - SERIES
        if (recent[RECENT_SIZE - 3] == UC(HRGN_N)) {
          if (recent[RECENT_SIZE - 2] == UC(HRGN_N)) {
            // MATCH NN_
            switch (keycode) {
            case UC(HRGN_A):
              tap_code(KC_BSPC);
              kana_out("っな");
              break;
            case UC(HRGN_E):
              tap_code(KC_BSPC);
              kana_out("っね");
              break;
            case UC(HRGN_I):
              tap_code(KC_BSPC);
              kana_out("っに");
              break;
            case UC(HRGN_O):
              tap_code(KC_BSPC);
              kana_out("っの");
              break;
            case UC(HRGN_U):
              tap_code(KC_BSPC);
              kana_out("っぬ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH NY_
            switch (keycode) {
            case UC(HRGN_A):
              tap_code(KC_BSPC);
              kana_out("にゃ");
              break;
            case UC(HRGN_O):
              tap_code(KC_BSPC);
              kana_out("にょ");
              break;
            case UC(HRGN_U):
              tap_code(KC_BSPC);
              kana_out("にゅ");
              break;
            }
          }
          // any unmatched n** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == UC(HRGN_N)) {
          // if N isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(HRGN_A):
            tap_code(KC_BSPC);
            kana_out("な");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            tap_code(KC_BSPC);
            kana_out("ね");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            tap_code(KC_BSPC);
            kana_out("に");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            tap_code(KC_BSPC);
            kana_out("の");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            tap_code(KC_BSPC);
            kana_out("ぬ");
            clear_recent_keys();
            break;
          case UC(HRGN_N):
          case KC_Y:
            // UC(HRGN_N),Y exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched n* 2char clears
            clear_recent_keys();
            update_recent_keys(keycode, record);
          }
          return false;
        }

        // H - SERIES
        if (recent[RECENT_SIZE - 3] == KC_H) {
          if (recent[RECENT_SIZE - 2] == KC_H) {
            // MATCH HH_
            switch (keycode) {
            case UC(HRGN_A):
              kana_out("っは");
              break;
            case UC(HRGN_E):
              kana_out("っへ");
              break;
            case UC(HRGN_I):
              kana_out("っひ");
              break;
            case UC(HRGN_O):
              kana_out("っほ");
              break;
            case UC(HRGN_U):
              kana_out("っふ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH HY_
            switch (keycode) {
            case UC(HRGN_A):
              kana_out("ひゃ");
              break;
            case UC(HRGN_O):
              kana_out("ひょ");
              break;
            case UC(HRGN_U):
              kana_out("ひゅ");
              break;
            }
          }
          // any unmatched h** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_H) {
          // if H isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(HRGN_A):
            kana_out("は");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            kana_out("へ");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            kana_out("ひ");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            kana_out("ほ");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            kana_out("ふ");
            clear_recent_keys();
            break;
          case KC_H:
          case KC_Y:
            // H,Y exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched h* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // F - SERIES
        if (recent[RECENT_SIZE - 3] == KC_F) {
          if (recent[RECENT_SIZE - 2] == KC_F) {
            // MATCH FF_
            switch (keycode) {
            case UC(HRGN_U):
              kana_out("っふ");
              break;
            }
          }
          // any unmatched f** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_F) {
          // if F isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(HRGN_U):
            kana_out("ふ");
            clear_recent_keys();
            break;
          case KC_F:
            // F exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched f* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // B - SERIES
        if (recent[RECENT_SIZE - 3] == KC_B) {
          if (recent[RECENT_SIZE - 2] == KC_B) {
            // MATCH BB_
            switch (keycode) {
            case UC(HRGN_A):
              kana_out("っば");
              break;
            case UC(HRGN_E):
              kana_out("っべ");
              break;
            case UC(HRGN_I):
              kana_out("っび");
              break;
            case UC(HRGN_O):
              kana_out("っぼ");
              break;
            case UC(HRGN_U):
              kana_out("っぶ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH BY_
            switch (keycode) {
            case UC(HRGN_A):
              kana_out("びゃ");
              break;
            case UC(HRGN_O):
              kana_out("びょ");
              break;
            case UC(HRGN_U):
              kana_out("びゅ");
              break;
            }
          }
          // any unmatched b** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_B) {
          // if B isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(HRGN_A):
            kana_out("ば");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            kana_out("べ");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            kana_out("び");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            kana_out("ぼ");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            kana_out("ぶ");
            clear_recent_keys();
            break;
          case KC_B:
          case KC_Y:
            // B,Y exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched b* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // P - SERIES
        if (recent[RECENT_SIZE - 3] == KC_P) {
          if (recent[RECENT_SIZE - 2] == KC_P) {
            // MATCH PP_
            switch (keycode) {
            case UC(HRGN_A):
              kana_out("っぱ");
              break;
            case UC(HRGN_E):
              kana_out("っぺ");
              break;
            case UC(HRGN_I):
              kana_out("っぴ");
              break;
            case UC(HRGN_O):
              kana_out("っぽ");
              break;
            case UC(HRGN_U):
              kana_out("っぷ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH PY_
            switch (keycode) {
            case UC(HRGN_A):
              kana_out("ぴゃ");
              break;
            case UC(HRGN_O):
              kana_out("ぴょ");
              break;
            case UC(HRGN_U):
              kana_out("ぴゅ");
              break;
            }
          }
          // any unmatched p** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_P) {
          // if P isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(HRGN_A):
            kana_out("ぱ");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            kana_out("ぺ");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            kana_out("ぴ");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            kana_out("ぽ");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            kana_out("ぷ");
            clear_recent_keys();
            break;
          case KC_P:
          case KC_Y:
            // P,Y exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched p* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // M - SERIES
        if (recent[RECENT_SIZE - 3] == KC_M) {
          if (recent[RECENT_SIZE - 2] == KC_M) {
            // MATCH MM_
            switch (keycode) {
            case UC(HRGN_A):
              kana_out("っま");
              break;
            case UC(HRGN_E):
              kana_out("っめ");
              break;
            case UC(HRGN_I):
              kana_out("っみ");
              break;
            case UC(HRGN_O):
              kana_out("っも");
              break;
            case UC(HRGN_U):
              kana_out("っむ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH MY_
            switch (keycode) {
            case UC(HRGN_A):
              kana_out("みゃ");
              break;
            case UC(HRGN_O):
              kana_out("みょ");
              break;
            case UC(HRGN_U):
              kana_out("みゅ");
              break;
            }
          }
          // any unmatched m** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_M) {
          // if M isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(HRGN_A):
            kana_out("ま");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            kana_out("め");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            kana_out("み");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            kana_out("も");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            kana_out("む");
            clear_recent_keys();
            break;
          case KC_M:
          case KC_Y:
            // M,Y exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched m* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // R - SERIES
        if (recent[RECENT_SIZE - 3] == KC_R) {
          if (recent[RECENT_SIZE - 2] == KC_R) {
            // MATCH RR_
            switch (keycode) {
            case UC(HRGN_A):
              kana_out("っら");
              break;
            case UC(HRGN_E):
              kana_out("っれ");
              break;
            case UC(HRGN_I):
              kana_out("っり");
              break;
            case UC(HRGN_O):
              kana_out("っろ");
              break;
            case UC(HRGN_U):
              kana_out("っる");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH RY_
            switch (keycode) {
            case UC(HRGN_A):
              kana_out("りゃ");
              break;
            case UC(HRGN_O):
              kana_out("りょ");
              break;
            case UC(HRGN_U):
              kana_out("りゅ");
              break;
            }
          }
          // any unmatched r** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_R) {
          // if R isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(HRGN_A):
            kana_out("ら");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            kana_out("れ");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            kana_out("り");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            kana_out("ろ");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            kana_out("る");
            clear_recent_keys();
            break;
          case KC_R:
          case KC_Y:
            // R,Y exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched r* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // W - SERIES
        if (recent[RECENT_SIZE - 3] == KC_W) {
          if (recent[RECENT_SIZE - 2] == KC_W) {
            // MATCH WW_
            switch (keycode) {
            case UC(HRGN_A):
              kana_out("っわ");
              break;
            case UC(HRGN_O):
              kana_out("っを");
              break;
            }
          }
          // any unmatched w** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_W) {
          // if W isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(HRGN_A):
            kana_out("わ");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            kana_out("を");
            clear_recent_keys();
            break;
          case KC_W:
            // W exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched w* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // Y - SERIES
        /* Needs to come after all other letters that might use Y
        // Such as Ryo, Mya... to ensure proper execution that this
        // does not clear recent keys on 2nd key Y */
        if (recent[RECENT_SIZE - 3] == KC_Y) {
          if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH YY_
            switch (keycode) {
            case UC(HRGN_A):
              kana_out("っや");
              break;
            case UC(HRGN_O):
              kana_out("っよ");
              break;
            case UC(HRGN_U):
              kana_out("っゆ");
              break;
            }
          }
          // any unmatched f** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_Y) {
          // if Y isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(HRGN_A):
            kana_out("や");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            kana_out("よ");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            kana_out("ゆ");
            clear_recent_keys();
            break;
          case UC(HRGN_A_SM):
            kana_out("ゃ");
            clear_recent_keys();
            break;
          case UC(HRGN_O_SM):
            kana_out("ょ");
            clear_recent_keys();
            break;
          case UC(HRGN_U_SM):
            kana_out("ゅ");
            clear_recent_keys();
            break;
          case KC_Y:
            // Y exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched y* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // NUM - SERIES
        if (recent[RECENT_SIZE - 3] == UC(JP_NUM_1)) {
          if (recent[RECENT_SIZE - 2] == UC(HRGN_E)) {
            // MATCH 1E_
            tap_code(KC_BSPC);
            tap_code(KC_BSPC);
            // right or wrong, backspace. 1,E have already been
            // pressed, entered, so they should always get removed
            switch (keycode) {
            case UC(JP_NUM_10):
              kana_out("〇"); // maru/zero for 1e0 despite the math
              break;
            case UC(JP_NUM_1):
              kana_out("十");
              break;
            case UC(JP_NUM_2):
              kana_out("百");
              break;
            case UC(JP_NUM_3):
              kana_out("千");
              break;
            case UC(JP_NUM_4):
              kana_out("万");
              break;
            case UC(JP_NUM_8):
              kana_out("億");
              break;
            case KC_W:
              kana_out("兆");
              break;
            }
            unregister_code(keycode);
            clear_recent_keys();
            return false;
          } // end e-press

        }
        // END HIRAGANA
      } else if (IS_LAYER_ON(KATAKANA) ) {
        /* START KATAKANA HERE
        ****************************************
        ****************************************
        ****************************************
        ****************************************
        ****************************************
        ****************************************
        ************************************* */
        // K - SERIES
        if (recent[RECENT_SIZE - 3] == KC_K) {
          if (recent[RECENT_SIZE - 2] == KC_K) {
            // MATCH KK_
            switch (keycode) {
            case UC(KTKN_A):
              kana_out("ッカ");
              break;
            case UC(KTKN_E):
              kana_out("ッケ");
              break;
            case UC(KTKN_I):
              kana_out("ッキ");
              break;
            case UC(KTKN_O):
              kana_out("ッコ");
              break;
            case UC(KTKN_U):
              kana_out("ック");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH KY_
            switch (keycode) {
            case UC(KTKN_A):
              kana_out("キャ");
              break;
            case UC(KTKN_O):
              kana_out("キョ");
              break;
            case UC(KTKN_U):
              kana_out("キュ");
              break;
            }
          }
          // any unmatched k** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_K) {
          // if K isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(KTKN_A):
            kana_out("カ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            kana_out("ケ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            kana_out("キ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            kana_out("コ");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            kana_out("ク");
            clear_recent_keys();
            break;
          case KC_K:
          case KC_Y:
            // K,Y exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched k* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // G - SERIES
        if (recent[RECENT_SIZE - 3] == KC_G) {
          if (recent[RECENT_SIZE - 2] == KC_G) {
            // MATCH GG_
            switch (keycode) {
            case UC(KTKN_A):
              kana_out("ッギ");
              break;
            case UC(KTKN_E):
              kana_out("ッゲ");
              break;
            case UC(KTKN_I):
              kana_out("ッギ");
              break;
            case UC(KTKN_O):
              kana_out("ッゴ");
              break;
            case UC(KTKN_U):
              kana_out("ッグ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH GY_
            switch (keycode) {
            case UC(KTKN_A):
              kana_out("ギャ");
              break;
            case UC(KTKN_O):
              kana_out("ギョ");
              break;
            case UC(KTKN_U):
              kana_out("ギュ");
              break;
            }
          }
          // any unmatched g** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_G) {
          // if G isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(KTKN_A):
            kana_out("が");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            kana_out("ゲ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            kana_out("ギ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            kana_out("ゴ");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            kana_out("グ");
            clear_recent_keys();
            break;
          case KC_G:
          case KC_Y:
            // G,Y exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched g* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // T - SERIES
        // Position T-Series before S-Series to ensure TSU can be captured.
        if (recent[RECENT_SIZE - 3] == KC_T) {
          if (recent[RECENT_SIZE - 2] == KC_T) {
            // MATCH TT_
            switch (keycode) {
            case UC(KTKN_A):
              kana_out("ッタ");
              break;
            case UC(KTKN_E):
              kana_out("ッテ");
              break;
            case UC(KTKN_I):
              kana_out("ッチ");
              break;
            case UC(KTKN_O):
              kana_out("ット");
              break;
            case UC(KTKN_U):
              kana_out("ッツ");
              break;
            case KC_S:
              kana_out("ッツ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_S) {
            // MATCH TS_
            switch (keycode) {
            case UC(KTKN_U):
              kana_out("ツ");
              break;
            case UC(KTKN_U_SM):
              kana_out("ッ");
              break;
            default:
              kana_out("ッ");
              break;
            }
          }
          // any unmatched t** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_T) {
          // if T isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(KTKN_A):
            kana_out("タ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            kana_out("テ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            kana_out("ティ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            kana_out("ト");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            kana_out("トゥ");
            clear_recent_keys();
            break;
          case KC_Y:
            kana_out("テュ");
            clear_recent_keys();
            break;
          case KC_T:
          case KC_S:
            // T,S exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched t* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // S - SERIES
        if (recent[RECENT_SIZE - 3] == KC_S) {
          if (recent[RECENT_SIZE - 2] == KC_S) {
            // MATCH SS_
            switch (keycode) {
            case UC(KTKN_A):
              kana_out("ッサ");
              break;
            case UC(KTKN_E):
              kana_out("ッセ");
              break;
            case UC(KTKN_I):
              kana_out("ッシ");
              break;
            case UC(KTKN_O):
              kana_out("ッソ");
              break;
            case UC(KTKN_U):
              kana_out("ッス");
              break;
            case KC_H:
              kana_out("ッシ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_H) {
            // MATCH SH_
            switch (keycode) {
            case UC(KTKN_A):
              kana_out("シャ");
              break;
            case UC(KTKN_E):
              kana_out("シェ");
              break;
            case UC(KTKN_I):
              kana_out("シ");
              break;
            case UC(KTKN_O):
              kana_out("ショ");
              break;
            case UC(KTKN_U):
              kana_out("シュ");
              break;
            }
          }
          // any unmatched s** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_S) {
          // if S isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(KTKN_A):
            kana_out("サ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            kana_out("セ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            kana_out("シ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            kana_out("ソ");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            kana_out("ス");
            clear_recent_keys();
            break;
          case KC_S:
          case KC_H:
            // S,H exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched s* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // Z - SERIES
        if (recent[RECENT_SIZE - 3] == KC_Z) {
          if (recent[RECENT_SIZE - 2] == KC_Z) {
            // MATCH ZZ_
            switch (keycode) {
            case UC(KTKN_A):
              kana_out("ッザ");
              break;
            case UC(KTKN_E):
              kana_out("ッゼ");
              break;
            case UC(KTKN_I):
              kana_out("ッジ");
              break;
            case UC(KTKN_O):
              kana_out("ッゾ");
              break;
            case UC(KTKN_U):
              kana_out("ッズ");
              break;
            }
          }
          // any unmatched z** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_Z) {
          // if Z isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(KTKN_A):
            kana_out("ザ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            kana_out("ゼ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            kana_out("ジ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            kana_out("ゾ");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            kana_out("ズ");
            clear_recent_keys();
            break;
          case KC_Z:
            // Z exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched z* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // J - SERIES
        if (recent[RECENT_SIZE - 3] == KC_J) {
          if (recent[RECENT_SIZE - 2] == KC_J) {
            // MATCH JJ_
            switch (keycode) {
            case UC(KTKN_A):
              kana_out("ッジャ");
              break;
            case UC(KTKN_O):
              kana_out("ッジョ");
              break;
            case UC(KTKN_U):
              kana_out("ッジュ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH JY_
            switch (keycode) {
            case UC(KTKN_A):
              kana_out("ジャ");
              break;
            case UC(KTKN_O):
              kana_out("ジョ");
              break;
            case UC(KTKN_U):
              kana_out("ジュ");
              break;
            }
          }
          // any unmatched j** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_J) {
          // if J isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(KTKN_A):
            kana_out("ジャ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            kana_out("ジェ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            kana_out("ジ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            kana_out("ジョ");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            kana_out("ジュ");
            clear_recent_keys();
            break;
          case KC_J:
          case KC_Y:
            // J,Y exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched j* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // C - SERIES
        if (recent[RECENT_SIZE - 3] == KC_C) {
          if (recent[RECENT_SIZE - 2] == KC_H) {
            // MATCH CH_
            switch (keycode) {
            case UC(KTKN_A):
              kana_out("チャ");
              break;
            case UC(KTKN_E):
              kana_out("チェ");
              break;
            case UC(KTKN_I):
              kana_out("チ");
              break;
            case UC(KTKN_O):
              kana_out("チョ");
              break;
            case UC(KTKN_U):
              kana_out("チュ");
              break;
            }
          }
          // any unmatched c** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        }

        // D - SERIES
        if (recent[RECENT_SIZE - 3] == KC_D) {
          if (recent[RECENT_SIZE - 2] == KC_D) {
            // MATCH DD_
            switch (keycode) {
            case UC(KTKN_A):
              kana_out("ッダ");
              break;
            case UC(KTKN_E):
              kana_out("ッデ");
              break;
            case UC(KTKN_I):
              kana_out("ッヂ");
              break;
            case UC(KTKN_O):
              kana_out("ッド");
              break;
            case UC(KTKN_U):
              kana_out("ッヅ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Z) {
            // MATCH DZ_
            switch (keycode) {
            case UC(KTKN_U):
              kana_out("ッヅ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_J) {
            // MATCH DJ_
            switch (keycode) {
            case UC(KTKN_I):
              kana_out("ヂ");
              break;
            }
          }
          // any unmatched d** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_D) {
          // if D isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(KTKN_A):
            kana_out("ダ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            kana_out("デ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            kana_out("ディ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            kana_out("ド");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            kana_out("ドゥ");
            clear_recent_keys();
            break;
          case KC_Y:
            kana_out("ドュ");
            clear_recent_keys();
            break;
          case KC_D:
          case KC_Z:
          case KC_J:
            // D,Z,J exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched d* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // N - SERIES
        if (recent[RECENT_SIZE - 3] == UC(KTKN_N)) {
          if (recent[RECENT_SIZE - 2] == UC(KTKN_N)) {
            // MATCH NN_
            switch (keycode) {
            case UC(KTKN_A):
              tap_code(KC_BSPC);
              kana_out("ッナ");
              break;
            case UC(KTKN_E):
              tap_code(KC_BSPC);
              kana_out("ッネ");
              break;
            case UC(KTKN_I):
              tap_code(KC_BSPC);
              kana_out("ッニ");
              break;
            case UC(KTKN_O):
              tap_code(KC_BSPC);
              kana_out("ッノ");
              break;
            case UC(KTKN_U):
              tap_code(KC_BSPC);
              kana_out("ッヌ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH NY_
            switch (keycode) {
            case UC(KTKN_A):
              tap_code(KC_BSPC);
              kana_out("ニャ");
              break;
            case UC(KTKN_O):
              tap_code(KC_BSPC);
              kana_out("ニョ");
              break;
            case UC(KTKN_U):
              tap_code(KC_BSPC);
              kana_out("ニュ");
              break;
            }
          }
          // any unmatched n** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == UC(KTKN_N)) {
          // if N isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(KTKN_A):
            tap_code(KC_BSPC);
            kana_out("ナ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            tap_code(KC_BSPC);
            kana_out("ネ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            tap_code(KC_BSPC);
            kana_out("ニ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            tap_code(KC_BSPC);
            kana_out("ノ");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            tap_code(KC_BSPC);
            kana_out("ヌ");
            clear_recent_keys();
            break;
          case UC(KTKN_N):
          case KC_Y:
            // UC(KTKN_N),Y exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched n* 2char clears
            clear_recent_keys();
            update_recent_keys(keycode, record);
          }
          return false;
        }

        // H - SERIES
        if (recent[RECENT_SIZE - 3] == KC_H) {
          if (recent[RECENT_SIZE - 2] == KC_H) {
            // MATCH HH_
            switch (keycode) {
            case UC(KTKN_A):
              kana_out("ッハ");
              break;
            case UC(KTKN_E):
              kana_out("ッヘ");
              break;
            case UC(KTKN_I):
              kana_out("ッヒ");
              break;
            case UC(KTKN_O):
              kana_out("ッホ");
              break;
            case UC(KTKN_U):
              kana_out("ッフ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH HY_
            switch (keycode) {
            case UC(KTKN_A):
              kana_out("ヒャ");
              break;
            case UC(KTKN_O):
              kana_out("ヒョ");
              break;
            case UC(KTKN_U):
              kana_out("ヒュ");
              break;
            }
          }
          // any unmatched h** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_H) {
          // if H isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(KTKN_A):
            kana_out("ハ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            kana_out("ヘ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            kana_out("ヒ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            kana_out("ホ");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            kana_out("フ");
            clear_recent_keys();
            break;
          case KC_H:
          case KC_Y:
            // H,Y exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched h* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // F - SERIES
        if (recent[RECENT_SIZE - 3] == KC_F) {
          if (recent[RECENT_SIZE - 2] == KC_F) {
            // MATCH FF_
            switch (keycode) {
            case UC(KTKN_U):
              kana_out("ッフ");
              break;
            }
          } 
          // any unmatched f** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_F) {
          // if F isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(KTKN_A):
            kana_out("ファ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            kana_out("フェ");
            clear_recent_keys(); 
            break;
          case UC(KTKN_I):
            kana_out("フィ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            kana_out("フォ");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            kana_out("フ");
            clear_recent_keys();
            break;
          case KC_F:
            // F exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched f* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // B - SERIES
        if (recent[RECENT_SIZE - 3] == KC_B) {
          if (recent[RECENT_SIZE - 2] == KC_B) {
            // MATCH BB_
            switch (keycode) {
            case UC(KTKN_A):
              kana_out("ッバ");
              break;
            case UC(KTKN_E):
              kana_out("ッベ");
              break;
            case UC(KTKN_I):
              kana_out("ッビ");
              break;
            case UC(KTKN_O):
              kana_out("ッボ");
              break;
            case UC(KTKN_U):
              kana_out("ッブ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH BY_
            switch (keycode) {
            case UC(KTKN_A):
              kana_out("ビャ");
              break;
            case UC(KTKN_O):
              kana_out("ビョ");
              break;
            case UC(KTKN_U):
              kana_out("ビュ");
              break;
            }
          }
          // any unmatched b** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_B) {
          // if B isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(KTKN_A):
            kana_out("バ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            kana_out("ベ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            kana_out("ビ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            kana_out("ボ");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            kana_out("ブ");
            clear_recent_keys();
            break;
          case KC_B:
          case KC_Y:
            // B,Y exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched b* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // P - SERIES
        if (recent[RECENT_SIZE - 3] == KC_P) {
          if (recent[RECENT_SIZE - 2] == KC_P) {
            // MATCH PP_
            switch (keycode) {
            case UC(KTKN_A):
              kana_out("ッパ");
              break;
            case UC(KTKN_E):
              kana_out("ッペ");
              break;
            case UC(KTKN_I):
              kana_out("ッピ");
              break;
            case UC(KTKN_O):
              kana_out("ッポ");
              break;
            case UC(KTKN_U):
              kana_out("ップ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH PY_
            switch (keycode) {
            case UC(KTKN_A):
              kana_out("ピャ");
              break;
            case UC(KTKN_O):
              kana_out("ピョ");
              break;
            case UC(KTKN_U):
              kana_out("ピュ");
              break;
            }
          }
          // any unmatched p** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_P) {
          // if P isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(KTKN_A):
            kana_out("パ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            kana_out("ペ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            kana_out("ピ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            kana_out("ポ");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            kana_out("プ");
            clear_recent_keys();
            break;
          case KC_P:
          case KC_Y:
            // P,Y exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched p* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // M - SERIES
        if (recent[RECENT_SIZE - 3] == KC_M) {
          if (recent[RECENT_SIZE - 2] == KC_M) {
            // MATCH MM_
            switch (keycode) {
            case UC(KTKN_A):
              kana_out("ッマ");
              break;
            case UC(KTKN_E):
              kana_out("ッメ");
              break;
            case UC(KTKN_I):
              kana_out("ッミ");
              break;
            case UC(KTKN_O):
              kana_out("ッモ");
              break;
            case UC(KTKN_U):
              kana_out("ッム");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH MY_
            switch (keycode) {
            case UC(KTKN_A):
              kana_out("ミャ");
              break;
            case UC(KTKN_O):
              kana_out("ミョ");
              break;
            case UC(KTKN_U):
              kana_out("ミュ");
              break;
            }
          }
          // any unmatched m** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_M) {
          // if M isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(KTKN_A):
            kana_out("マ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            kana_out("メ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            kana_out("ミ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            kana_out("モ");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            kana_out("ム");
            clear_recent_keys();
            break;
          case KC_M:
          case KC_Y:
            // M,Y exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched m* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // R - SERIES
        if (recent[RECENT_SIZE - 3] == KC_R) {
          if (recent[RECENT_SIZE - 2] == KC_R) {
            // MATCH RR_
            switch (keycode) {
            case UC(KTKN_A):
              kana_out("ッラ");
              break;
            case UC(KTKN_E):
              kana_out("ッレ");
              break;
            case UC(KTKN_I):
              kana_out("ッリ");
              break;
            case UC(KTKN_O):
              kana_out("ッロ");
              break;
            case UC(KTKN_U):
              kana_out("ッル");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH RY_
            switch (keycode) {
            case UC(KTKN_A):
              kana_out("リャ");
              break;
            case UC(KTKN_O):
              kana_out("リョ");
              break;
            case UC(KTKN_U):
              kana_out("リュ");
              break;
            }
          }
          // any unmatched r** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_R) {
          // if R isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(KTKN_A):
            kana_out("ラ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            kana_out("レ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            kana_out("リ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            kana_out("ロ");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            kana_out("ル");
            clear_recent_keys();
            break;
          case KC_R:
          case KC_Y:
            // R,Y exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched r* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // V - SERIES
        if (recent[RECENT_SIZE - 3] == KC_V) {
          if (recent[RECENT_SIZE - 2] == KC_V) {
            // MATCH VV_
            switch (keycode) {
            case UC(KTKN_U):
              kana_out("ッヴ");
              break;
            }
          }
          // any unmatched v** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_V) {
          // if V isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(KTKN_A):
            kana_out("ヴァ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            kana_out("ヴェ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            kana_out("ヴィ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            kana_out("ヴォ");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            kana_out("ヴ");
            clear_recent_keys();
            break;
          case KC_V:
            // V exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched v* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // W - SERIES
        if (recent[RECENT_SIZE - 3] == KC_W) {
          if (recent[RECENT_SIZE - 2] == KC_W) {
            // MATCH WW_
            switch (keycode) {
            case UC(KTKN_A):
              kana_out("ッわ");
              break;
            case UC(KTKN_O):
              kana_out("ッを");
              break;
            }
          }
          // any unmatched w** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_W) {
          // if W isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(KTKN_A):
            kana_out("ワ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            kana_out("ウェ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            kana_out("ウィ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            kana_out("ウォ");
            clear_recent_keys();
            break;
          case KC_W:
            // W exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched w* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // Y - SERIES
        if (recent[RECENT_SIZE - 3] == KC_Y) {
          if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH YY_
            switch (keycode) {
            case UC(KTKN_A):
              kana_out("ッヤ");
              break;
            case UC(KTKN_O):
              kana_out("ッヨ");
              break;
            case UC(KTKN_U):
              kana_out("ッユ");
              break;
            }
          }
          // any unmatched f** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_Y) {
          // if Y isn't 3rd most recent, is it still 2nd most recent?
          unregister_code(keycode);
          switch (keycode) {
          case UC(KTKN_A):
            kana_out("ヤ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            kana_out("ヨ");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            kana_out("ユ");
            clear_recent_keys();
            break;
          case UC(KTKN_A_SM):
            kana_out("ャ");
            clear_recent_keys();
            break;
          case UC(KTKN_O_SM):
            kana_out("ョ");
            clear_recent_keys();
            break;
          case UC(KTKN_U_SM):
            kana_out("ュ");
            clear_recent_keys();
            break;
          case KC_Y:
            // Y exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched y* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // NUM - SERIES
        if (recent[RECENT_SIZE - 3] == UC(JP_NUM_1)) {
          if (recent[RECENT_SIZE - 2] == UC(KTKN_E)) {
            // MATCH 1E_
            tap_code(KC_BSPC);
            tap_code(KC_BSPC);
            // right or wrong, backspace. 1,E have already been
            // pressed, entered, so they should always get removed
            switch (keycode) {
            case UC(JP_NUM_10):
              kana_out("〇"); // maru/zero for 1e0 despite the math
              break;
            case UC(JP_NUM_1):
              kana_out("十");
              break;
            case UC(JP_NUM_2):
              kana_out("百");
              break;
            case UC(JP_NUM_3):
              kana_out("千");
              break;
            case UC(JP_NUM_4):
              kana_out("万");
              break;
            case UC(JP_NUM_8):
              kana_out("億");
              break;
            case KC_W:
              kana_out("兆");
              break;
            }
            unregister_code(keycode);
            clear_recent_keys();
            return false;
          } // end e-press

        }

      } // end katakana layer check
    } // end record.pressed
  } // end update_recent_keys

  switch (keycode) {
  case HRGA_GO:
    if (record->event.pressed) {
      layer_clear();
      layer_on(HIRAGANA);
      return false;
    }
    break;
  case KTKN_GO:
    if (record->event.pressed) {
      layer_clear();
      layer_on(KATAKANA);
      return false;
    }
    break;
  case ENG_GO:
    if (record->event.pressed) {
      layer_clear();
    } else {
      layer_on(QWERTY);
      return false;
    }
    break;
  case KC_K:
  case KC_G:
  case KC_S:
  case KC_Z:
  case KC_T:
  case KC_D:
  case KC_N:
  case KC_H:
  case KC_B:
  case KC_P:
  case KC_M:
  case KC_Y:
  case KC_R:
  case KC_W:
  case KC_V:
  case KC_C:
  case KC_F:
  case KC_J:
    if (IS_LAYER_ON(HIRAGANA) ) {
      // unregister because it is already saved in recent buffer
      unregister_code(keycode);
      return false;
    } else if (IS_LAYER_ON(KATAKANA) ) {
      unregister_code(keycode);
      return false;
    }
    break;
  }

  // Standalone kana (lone vowels, ん/ン, small kana, ー) are emitted directly
  // by QMK's UC() handling and never pass through kana_out(), so count them
  // here. Gated on press to avoid double-counting the key-release event; all
  // kana_out() paths return earlier, so multi-key kana can't be counted twice.
  if (record->event.pressed) {
    switch (keycode) {
    case UC(0x3040) ... UC(0x30FF):
      ime_char_count++;
      break;
    }
  }

  return true;
};
