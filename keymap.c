/* Copyright 2015-2021 Jack Humbert
 * Original keymap by William Dizon <wdchromium@gmail.com>
 * QWERTY + HIRAGANA + KATAKANA
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

#define QWERTY 0
#define HIRAGANA 1
#define KATAKANA 2
#define FUNCS 5
#define GUIS 6
#define HIRAGANA_SUPP 7
#define KATAKANA_SUPP 8

enum {
  HRGA_GO = SAFE_RANGE,
  KTKN_GO,
  ENG_GO
};

// Start Recent Key Rememering:
// https://getreuer.info/posts/keyboards/triggers/index.html#based-on-previously-typed-keys
#include <string.h>

#define TIMEOUT_MS 3000  // Timeout in milliseconds.
#define RECENT_SIZE 3    // Number of keys in `recent` buffer.

static uint16_t recent[RECENT_SIZE] = {KC_NO};
static uint16_t deadline = 0;

static void clear_recent_keys(void) {
  memset(recent, 0, sizeof(recent));  // Set all zeros (KC_NO).
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

void matrix_scan_user(void) {
  if (recent[RECENT_SIZE - 1] && timer_expired(timer_read(), deadline)) {
    clear_recent_keys();  // Timed out; clear the buffer.
  }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (update_recent_keys(keycode, record)) {
    if (record->event.pressed) {
      if (IS_LAYER_ON(HIRAGANA) ) {

        // K - SERIES
        if (recent[RECENT_SIZE - 3] == KC_K) {
          if (recent[RECENT_SIZE - 2] == KC_K) {
            // MATCH KK_
            switch (keycode) {
            case UC(HRGN_A):
              send_unicode_string("っか");
              break;
            case UC(HRGN_E):
              send_unicode_string("っけ");
              break;
            case UC(HRGN_I):
              send_unicode_string("っき");
              break;
            case UC(HRGN_O):
              send_unicode_string("っこ");
              break;
            case UC(HRGN_U):
              send_unicode_string("っく");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH KY_
            switch (keycode) {
            case UC(HRGN_A):
              send_unicode_string("きゃ");
              break;
            case UC(HRGN_O):
              send_unicode_string("きょ");
              break;
            case UC(HRGN_U):
              send_unicode_string("きゅ");
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
            send_unicode_string("か");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            send_unicode_string("け");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            send_unicode_string("き");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("こ");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            send_unicode_string("く");
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
              send_unicode_string("っぎ");
              break;
            case UC(HRGN_E):
              send_unicode_string("っげ");
              break;
            case UC(HRGN_I):
              send_unicode_string("っぎ");
              break;
            case UC(HRGN_O):
              send_unicode_string("っご");
              break;
            case UC(HRGN_U):
              send_unicode_string("っぐ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH GY_
            switch (keycode) {
            case UC(HRGN_A):
              send_unicode_string("ぎゃ");
              break;
            case UC(HRGN_O):
              send_unicode_string("ぎょ");
              break;
            case UC(HRGN_U):
              send_unicode_string("ぎゅ");
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
            send_unicode_string("が");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            send_unicode_string("げ");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            send_unicode_string("ぎ");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("ご");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            send_unicode_string("ぐ");
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
              send_unicode_string("った");
              break;
            case UC(HRGN_E):
              send_unicode_string("って");
              break;
            case UC(HRGN_I):
              send_unicode_string("っち");
              break;
            case UC(HRGN_O):
              send_unicode_string("っと");
              break;
            case UC(HRGN_U):
              send_unicode_string("っつ");
              break;
            case KC_S:
              send_unicode_string("っつ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_S) {
            // MATCH TS_
            switch (keycode) {
            case UC(HRGN_U):
              send_unicode_string("つ");
              break;
            case UC(HRGN_U_SM):
              send_unicode_string("っ");
              break;
            default:
              send_unicode_string("っ");
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
            send_unicode_string("た");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            send_unicode_string("て");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            send_unicode_string("ち");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("と");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            send_unicode_string("つ");
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
              send_unicode_string("っさ");
              break;
            case UC(HRGN_E):
              send_unicode_string("っせ");
              break;
            case UC(HRGN_I):
              send_unicode_string("っし");
              break;
            case UC(HRGN_O):
              send_unicode_string("っそ");
              break;
            case UC(HRGN_U):
              send_unicode_string("っす");
              break;
            case KC_H:
              send_unicode_string("っし");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_H) {
            // MATCH SH_
            switch (keycode) {
            case UC(HRGN_A):
              send_unicode_string("しゃ");
              break;
            case UC(HRGN_I):
              send_unicode_string("し");
              break;
            case UC(HRGN_O):
              send_unicode_string("しょ");
              break;
            case UC(HRGN_U):
              send_unicode_string("しゅ");
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
            send_unicode_string("さ");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            send_unicode_string("せ");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            send_unicode_string("し");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("そ");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            send_unicode_string("す");
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
              send_unicode_string("っざ");
              break;
            case UC(HRGN_E):
              send_unicode_string("っぜ");
              break;
            case UC(HRGN_I):
              send_unicode_string("っじ");
              break;
            case UC(HRGN_O):
              send_unicode_string("っぞ");
              break;
            case UC(HRGN_U):
              send_unicode_string("っず");
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
            send_unicode_string("ざ");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            send_unicode_string("ぜ");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            send_unicode_string("じ");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("ぞ");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            send_unicode_string("ず");
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
              send_unicode_string("っじゃ");
              break;
            case UC(HRGN_O):
              send_unicode_string("っじょ");
              break;
            case UC(HRGN_U):
              send_unicode_string("っじゅ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH JY_
            switch (keycode) {
            case UC(HRGN_A):
              send_unicode_string("じゃ");
              break;
            case UC(HRGN_O):
              send_unicode_string("じょ");
              break;
            case UC(HRGN_U):
              send_unicode_string("じゅ");
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
            send_unicode_string("じゃ");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            send_unicode_string("じ");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("じょ");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            send_unicode_string("じゅ");
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
              send_unicode_string("ちゃ");
              break;
            case UC(HRGN_I):
              send_unicode_string("ち");
              break;
            case UC(HRGN_O):
              send_unicode_string("ちょ");
              break;
            case UC(HRGN_U):
              send_unicode_string("ちゅ");
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
              send_unicode_string("っだ");
              break;
            case UC(HRGN_E):
              send_unicode_string("っで");
              break;
            case UC(HRGN_I):
              send_unicode_string("っぢ");
              break;
            case UC(HRGN_O):
              send_unicode_string("っど");
              break;
            case UC(HRGN_U):
              send_unicode_string("っづ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Z) {
            // MATCH DZ_
            switch (keycode) {
            case UC(HRGN_U):
              send_unicode_string("っづ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_J) {
            // MATCH DJ_
            switch (keycode) {
            case UC(HRGN_I):
              send_unicode_string("ぢ");
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
            send_unicode_string("だ");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            send_unicode_string("で");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            send_unicode_string("ぢ");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("ど");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            send_unicode_string("づ");
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
              send_unicode_string("っな");
              break;
            case UC(HRGN_E):
              tap_code(KC_BSPC);
              send_unicode_string("っね");
              break;
            case UC(HRGN_I):
              tap_code(KC_BSPC);
              send_unicode_string("っに");
              break;
            case UC(HRGN_O):
              tap_code(KC_BSPC);
              send_unicode_string("っの");
              break;
            case UC(HRGN_U):
              tap_code(KC_BSPC);
              send_unicode_string("っぬ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH NY_
            switch (keycode) {
            case UC(HRGN_A):
              tap_code(KC_BSPC);
              send_unicode_string("にゃ");
              break;
            case UC(HRGN_O):
              tap_code(KC_BSPC);
              send_unicode_string("にょ");
              break;
            case UC(HRGN_U):
              tap_code(KC_BSPC);
              send_unicode_string("にゅ");
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
            send_unicode_string("な");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            tap_code(KC_BSPC);
            send_unicode_string("ね");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            tap_code(KC_BSPC);
            send_unicode_string("に");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            tap_code(KC_BSPC);
            send_unicode_string("の");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            tap_code(KC_BSPC);
            send_unicode_string("ぬ");
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
              send_unicode_string("っは");
              break;
            case UC(HRGN_E):
              send_unicode_string("っへ");
              break;
            case UC(HRGN_I):
              send_unicode_string("っひ");
              break;
            case UC(HRGN_O):
              send_unicode_string("っほ");
              break;
            case UC(HRGN_U):
              send_unicode_string("っふ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH HY_
            switch (keycode) {
            case UC(HRGN_A):
              send_unicode_string("ひゃ");
              break;
            case UC(HRGN_O):
              send_unicode_string("ひょ");
              break;
            case UC(HRGN_U):
              send_unicode_string("ひゅ");
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
            send_unicode_string("は");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            send_unicode_string("へ");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            send_unicode_string("ひ");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("ほ");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            send_unicode_string("ふ");
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
              send_unicode_string("っふ");
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
            send_unicode_string("ふ");
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
              send_unicode_string("っば");
              break;
            case UC(HRGN_E):
              send_unicode_string("っべ");
              break;
            case UC(HRGN_I):
              send_unicode_string("っび");
              break;
            case UC(HRGN_O):
              send_unicode_string("っぼ");
              break;
            case UC(HRGN_U):
              send_unicode_string("っぶ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH BY_
            switch (keycode) {
            case UC(HRGN_A):
              send_unicode_string("びゃ");
              break;
            case UC(HRGN_O):
              send_unicode_string("びょ");
              break;
            case UC(HRGN_U):
              send_unicode_string("びゅ");
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
            send_unicode_string("ば");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            send_unicode_string("べ");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            send_unicode_string("び");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("ぼ");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            send_unicode_string("ぶ");
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
              send_unicode_string("っぱ");
              break;
            case UC(HRGN_E):
              send_unicode_string("っぺ");
              break;
            case UC(HRGN_I):
              send_unicode_string("っぴ");
              break;
            case UC(HRGN_O):
              send_unicode_string("っぽ");
              break;
            case UC(HRGN_U):
              send_unicode_string("っぷ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH PY_
            switch (keycode) {
            case UC(HRGN_A):
              send_unicode_string("ぴゃ");
              break;
            case UC(HRGN_O):
              send_unicode_string("ぴょ");
              break;
            case UC(HRGN_U):
              send_unicode_string("ぴゅ");
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
            send_unicode_string("ぱ");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            send_unicode_string("ぺ");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            send_unicode_string("ぴ");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("ぽ");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            send_unicode_string("ぷ");
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
              send_unicode_string("っま");
              break;
            case UC(HRGN_E):
              send_unicode_string("っめ");
              break;
            case UC(HRGN_I):
              send_unicode_string("っみ");
              break;
            case UC(HRGN_O):
              send_unicode_string("っも");
              break;
            case UC(HRGN_U):
              send_unicode_string("っむ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH MY_
            switch (keycode) {
            case UC(HRGN_A):
              send_unicode_string("みゃ");
              break;
            case UC(HRGN_O):
              send_unicode_string("みょ");
              break;
            case UC(HRGN_U):
              send_unicode_string("みゅ");
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
            send_unicode_string("ま");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            send_unicode_string("め");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            send_unicode_string("み");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("も");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            send_unicode_string("む");
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
              send_unicode_string("っら");
              break;
            case UC(HRGN_E):
              send_unicode_string("っれ");
              break;
            case UC(HRGN_I):
              send_unicode_string("っり");
              break;
            case UC(HRGN_O):
              send_unicode_string("っろ");
              break;
            case UC(HRGN_U):
              send_unicode_string("っる");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH RY_
            switch (keycode) {
            case UC(HRGN_A):
              send_unicode_string("りゃ");
              break;
            case UC(HRGN_O):
              send_unicode_string("りょ");
              break;
            case UC(HRGN_U):
              send_unicode_string("りゅ");
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
            send_unicode_string("ら");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            send_unicode_string("れ");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            send_unicode_string("り");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("ろ");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            send_unicode_string("る");
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
              send_unicode_string("っわ");
              break;
            case UC(HRGN_O):
              send_unicode_string("っを");
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
            send_unicode_string("わ");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("を");
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
              send_unicode_string("っや");
              break;
            case UC(HRGN_O):
              send_unicode_string("っよ");
              break;
            case UC(HRGN_U):
              send_unicode_string("っゆ");
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
            send_unicode_string("や");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("よ");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            send_unicode_string("ゆ");
            clear_recent_keys();
            break;
          case UC(HRGN_A_SM):
            send_unicode_string("ゃ");
            clear_recent_keys();
            break;
          case UC(HRGN_O_SM):
            send_unicode_string("ょ");
            clear_recent_keys();
            break;
          case UC(HRGN_U_SM):
            send_unicode_string("ゅ");
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
              send_unicode_string("〇"); // maru/zero for 1e0 despite the math
              break;
            case UC(JP_NUM_1):
              send_unicode_string("十");
              break;
            case UC(JP_NUM_2):
              send_unicode_string("百");
              break;
            case UC(JP_NUM_3):
              send_unicode_string("千");
              break;
            case UC(JP_NUM_4):
              send_unicode_string("万");
              break;
            case UC(JP_NUM_8):
              send_unicode_string("億");
              break;
            case KC_W:
              send_unicode_string("兆");
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
              send_unicode_string("ッカ");
              break;
            case UC(KTKN_E):
              send_unicode_string("ッケ");
              break;
            case UC(KTKN_I):
              send_unicode_string("ッキ");
              break;
            case UC(KTKN_O):
              send_unicode_string("ッコ");
              break;
            case UC(KTKN_U):
              send_unicode_string("ック");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH KY_
            switch (keycode) {
            case UC(KTKN_A):
              send_unicode_string("キャ");
              break;
            case UC(KTKN_O):
              send_unicode_string("キョ");
              break;
            case UC(KTKN_U):
              send_unicode_string("キュ");
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
            send_unicode_string("カ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            send_unicode_string("ケ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            send_unicode_string("キ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            send_unicode_string("コ");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            send_unicode_string("ク");
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
              send_unicode_string("ッギ");
              break;
            case UC(KTKN_E):
              send_unicode_string("ッゲ");
              break;
            case UC(KTKN_I):
              send_unicode_string("ッギ");
              break;
            case UC(KTKN_O):
              send_unicode_string("ッゴ");
              break;
            case UC(KTKN_U):
              send_unicode_string("ッグ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH GY_
            switch (keycode) {
            case UC(KTKN_A):
              send_unicode_string("ギャ");
              break;
            case UC(KTKN_O):
              send_unicode_string("ギョ");
              break;
            case UC(KTKN_U):
              send_unicode_string("ギュ");
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
            send_unicode_string("が");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            send_unicode_string("ゲ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            send_unicode_string("ギ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            send_unicode_string("ゴ");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            send_unicode_string("グ");
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
              send_unicode_string("ッタ");
              break;
            case UC(KTKN_E):
              send_unicode_string("ッテ");
              break;
            case UC(KTKN_I):
              send_unicode_string("ッチ");
              break;
            case UC(KTKN_O):
              send_unicode_string("ット");
              break;
            case UC(KTKN_U):
              send_unicode_string("ッツ");
              break;
            case KC_S:
              send_unicode_string("ッツ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_S) {
            // MATCH TS_
            switch (keycode) {
            case UC(KTKN_U):
              send_unicode_string("ツ");
              break;
            case UC(KTKN_U_SM):
              send_unicode_string("ッ");
              break;
            default:
              send_unicode_string("ッ");
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
            send_unicode_string("タ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            send_unicode_string("テ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            send_unicode_string("チ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            send_unicode_string("ト");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            send_unicode_string("ツ");
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
              send_unicode_string("ッサ");
              break;
            case UC(KTKN_E):
              send_unicode_string("ッセ");
              break;
            case UC(KTKN_I):
              send_unicode_string("ッシ");
              break;
            case UC(KTKN_O):
              send_unicode_string("ッソ");
              break;
            case UC(KTKN_U):
              send_unicode_string("ッス");
              break;
            case KC_H:
              send_unicode_string("ッシ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_H) {
            // MATCH SH_
            switch (keycode) {
            case UC(KTKN_A):
              send_unicode_string("シャ");
              break;
            case UC(KTKN_I):
              send_unicode_string("シ");
              break;
            case UC(KTKN_O):
              send_unicode_string("ショ");
              break;
            case UC(KTKN_U):
              send_unicode_string("シュ");
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
            send_unicode_string("サ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            send_unicode_string("セ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            send_unicode_string("シ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            send_unicode_string("ソ");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            send_unicode_string("ス");
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
              send_unicode_string("ッザ");
              break;
            case UC(KTKN_E):
              send_unicode_string("ッゼ");
              break;
            case UC(KTKN_I):
              send_unicode_string("ッジ");
              break;
            case UC(KTKN_O):
              send_unicode_string("ッゾ");
              break;
            case UC(KTKN_U):
              send_unicode_string("ッズ");
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
            send_unicode_string("ザ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            send_unicode_string("ゼ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            send_unicode_string("ジ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            send_unicode_string("ゾ");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            send_unicode_string("ズ");
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
              send_unicode_string("ッジャ");
              break;
            case UC(KTKN_O):
              send_unicode_string("ッジョ");
              break;
            case UC(KTKN_U):
              send_unicode_string("ッジュ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH JY_
            switch (keycode) {
            case UC(KTKN_A):
              send_unicode_string("ジャ");
              break;
            case UC(KTKN_O):
              send_unicode_string("ジョ");
              break;
            case UC(KTKN_U):
              send_unicode_string("ジュ");
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
            send_unicode_string("ジャ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            send_unicode_string("ジ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            send_unicode_string("ジョ");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            send_unicode_string("ジュ");
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
              send_unicode_string("チャ");
              break;
            case UC(KTKN_I):
              send_unicode_string("チ");
              break;
            case UC(KTKN_O):
              send_unicode_string("チョ");
              break;
            case UC(KTKN_U):
              send_unicode_string("チュ");
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
              send_unicode_string("ッダ");
              break;
            case UC(KTKN_E):
              send_unicode_string("ッデ");
              break;
            case UC(KTKN_I):
              send_unicode_string("ッヂ");
              break;
            case UC(KTKN_O):
              send_unicode_string("ッド");
              break;
            case UC(KTKN_U):
              send_unicode_string("ッヅ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Z) {
            // MATCH DZ_
            switch (keycode) {
            case UC(KTKN_U):
              send_unicode_string("ッヅ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_J) {
            // MATCH DJ_
            switch (keycode) {
            case UC(KTKN_I):
              send_unicode_string("ヂ");
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
            send_unicode_string("ダ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            send_unicode_string("デ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            send_unicode_string("ヂ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            send_unicode_string("ド");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            send_unicode_string("ヅ");
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
              send_unicode_string("ッナ");
              break;
            case UC(KTKN_E):
              tap_code(KC_BSPC);
              send_unicode_string("ッネ");
              break;
            case UC(KTKN_I):
              tap_code(KC_BSPC);
              send_unicode_string("ッニ");
              break;
            case UC(KTKN_O):
              tap_code(KC_BSPC);
              send_unicode_string("ッノ");
              break;
            case UC(KTKN_U):
              tap_code(KC_BSPC);
              send_unicode_string("ッヌ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH NY_
            switch (keycode) {
            case UC(KTKN_A):
              tap_code(KC_BSPC);
              send_unicode_string("ニャ");
              break;
            case UC(KTKN_O):
              tap_code(KC_BSPC);
              send_unicode_string("ニョ");
              break;
            case UC(KTKN_U):
              tap_code(KC_BSPC);
              send_unicode_string("ニュ");
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
            send_unicode_string("ナ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            tap_code(KC_BSPC);
            send_unicode_string("ネ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            tap_code(KC_BSPC);
            send_unicode_string("ニ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            tap_code(KC_BSPC);
            send_unicode_string("ノ");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            tap_code(KC_BSPC);
            send_unicode_string("ヌ");
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
              send_unicode_string("ッハ");
              break;
            case UC(KTKN_E):
              send_unicode_string("ッヘ");
              break;
            case UC(KTKN_I):
              send_unicode_string("ッヒ");
              break;
            case UC(KTKN_O):
              send_unicode_string("ッホ");
              break;
            case UC(KTKN_U):
              send_unicode_string("ッフ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH HY_
            switch (keycode) {
            case UC(KTKN_A):
              send_unicode_string("ヒャ");
              break;
            case UC(KTKN_O):
              send_unicode_string("ヒョ");
              break;
            case UC(KTKN_U):
              send_unicode_string("ヒュ");
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
            send_unicode_string("ハ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            send_unicode_string("ヘ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            send_unicode_string("ヒ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            send_unicode_string("ホ");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            send_unicode_string("フ");
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
              send_unicode_string("ッフ");
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
          case UC(KTKN_U):
            send_unicode_string("フ");
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
              send_unicode_string("ッバ");
              break;
            case UC(KTKN_E):
              send_unicode_string("ッベ");
              break;
            case UC(KTKN_I):
              send_unicode_string("ッビ");
              break;
            case UC(KTKN_O):
              send_unicode_string("ッボ");
              break;
            case UC(KTKN_U):
              send_unicode_string("ッブ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH BY_
            switch (keycode) {
            case UC(KTKN_A):
              send_unicode_string("ビャ");
              break;
            case UC(KTKN_O):
              send_unicode_string("ビョ");
              break;
            case UC(KTKN_U):
              send_unicode_string("ビュ");
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
            send_unicode_string("バ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            send_unicode_string("ベ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            send_unicode_string("ビ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            send_unicode_string("ボ");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            send_unicode_string("ブ");
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
              send_unicode_string("ッパ");
              break;
            case UC(KTKN_E):
              send_unicode_string("ッペ");
              break;
            case UC(KTKN_I):
              send_unicode_string("ッピ");
              break;
            case UC(KTKN_O):
              send_unicode_string("ッポ");
              break;
            case UC(KTKN_U):
              send_unicode_string("ップ");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH PY_
            switch (keycode) {
            case UC(KTKN_A):
              send_unicode_string("ピャ");
              break;
            case UC(KTKN_O):
              send_unicode_string("ピョ");
              break;
            case UC(KTKN_U):
              send_unicode_string("ピュ");
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
            send_unicode_string("パ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            send_unicode_string("ペ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            send_unicode_string("ピ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            send_unicode_string("ポ");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            send_unicode_string("プ");
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
              send_unicode_string("ッマ");
              break;
            case UC(KTKN_E):
              send_unicode_string("ッメ");
              break;
            case UC(KTKN_I):
              send_unicode_string("ッミ");
              break;
            case UC(KTKN_O):
              send_unicode_string("ッモ");
              break;
            case UC(KTKN_U):
              send_unicode_string("ッム");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH MY_
            switch (keycode) {
            case UC(KTKN_A):
              send_unicode_string("ミャ");
              break;
            case UC(KTKN_O):
              send_unicode_string("ミョ");
              break;
            case UC(KTKN_U):
              send_unicode_string("ミュ");
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
            send_unicode_string("マ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            send_unicode_string("メ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            send_unicode_string("ミ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            send_unicode_string("モ");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            send_unicode_string("ム");
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
              send_unicode_string("ッラ");
              break;
            case UC(KTKN_E):
              send_unicode_string("ッレ");
              break;
            case UC(KTKN_I):
              send_unicode_string("ッリ");
              break;
            case UC(KTKN_O):
              send_unicode_string("ッロ");
              break;
            case UC(KTKN_U):
              send_unicode_string("ッル");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH RY_
            switch (keycode) {
            case UC(KTKN_A):
              send_unicode_string("リャ");
              break;
            case UC(KTKN_O):
              send_unicode_string("リョ");
              break;
            case UC(KTKN_U):
              send_unicode_string("リュ");
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
            send_unicode_string("ラ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            send_unicode_string("レ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            send_unicode_string("リ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            send_unicode_string("ロ");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            send_unicode_string("ル");
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
              send_unicode_string("ッヴ");
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
          case UC(KTKN_U):
            send_unicode_string("ヴ");
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
              send_unicode_string("ッわ");
              break;
            case UC(KTKN_O):
              send_unicode_string("ッを");
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
            send_unicode_string("ワ");
            clear_recent_keys();
            break;
          case UC(KTKN_E):
            send_unicode_string("ゑ");
            clear_recent_keys();
            break;
          case UC(KTKN_I):
            send_unicode_string("ヰ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            send_unicode_string("ヲ");
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
              send_unicode_string("ッヤ");
              break;
            case UC(KTKN_O):
              send_unicode_string("ッヨ");
              break;
            case UC(KTKN_U):
              send_unicode_string("ッユ");
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
            send_unicode_string("ヤ");
            clear_recent_keys();
            break;
          case UC(KTKN_O):
            send_unicode_string("ヨ");
            clear_recent_keys();
            break;
          case UC(KTKN_U):
            send_unicode_string("ユ");
            clear_recent_keys();
            break;
          case UC(KTKN_A_SM):
            send_unicode_string("ャ");
            clear_recent_keys();
            break;
          case UC(KTKN_O_SM):
            send_unicode_string("ョ");
            clear_recent_keys();
            break;
          case UC(KTKN_U_SM):
            send_unicode_string("ュ");
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
              send_unicode_string("〇"); // maru/zero for 1e0 despite the math
              break;
            case UC(JP_NUM_1):
              send_unicode_string("十");
              break;
            case UC(JP_NUM_2):
              send_unicode_string("百");
              break;
            case UC(JP_NUM_3):
              send_unicode_string("千");
              break;
            case UC(JP_NUM_4):
              send_unicode_string("万");
              break;
            case UC(JP_NUM_8):
              send_unicode_string("億");
              break;
            case KC_W:
              send_unicode_string("兆");
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

  return true;
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* Base
 * ,-----------------------------------------------------------------------------------.
 * | Esc  |   1  |   2  |   3  |   4  |   5  | Del  |   6  |   7  |   8  |   9  |  0   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Tab  |   Q  |   W  |   E  |   R  |   T  | Bksp |   Y  |   U  |   I  |   O  |  P   |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * | MO(5)|   A  |   S  |   D  |   F  |   G  | Ent  |   H  |   J  |   K  |   L  |  ;   |
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * | Shift|   Z  |   X  |   C  |   V  |   B  | Tab  |   N  |   M  |   ,  |   .  |  /   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Ctrl | Alt  | GUI  | LT(5)| MO(6)|    Space    |   +  |   -  |  Del |  Ins | Ent  |
 * `-----------------------------------------------------------------------------------'
 */

[QWERTY] = LAYOUT_preonic_grid(
  QK_GESC , KC_1   , KC_2   , KC_3           , KC_4     , KC_5  , KC_DEL , KC_6  , KC_7   , KC_8   , KC_9  , KC_0   ,
  KC_TAB  , KC_Q   , KC_W   , KC_E           , KC_R     , KC_T  , KC_BSPC, KC_Y  , KC_U   , KC_I   , KC_O  , KC_P   ,
  MO(GUIS), KC_A   , KC_S   , KC_D           , KC_F     , KC_G  , KC_ENT , KC_H  , KC_J   , KC_K   , KC_L  , KC_SCLN,
  KC_LSFT , KC_Z   , KC_X   , KC_C           , KC_V     , KC_B  , KC_TAB , KC_N  , KC_M   , KC_COMM, KC_DOT, KC_SLSH,
  KC_LCTL , KC_LALT, KC_LGUI, LT(GUIS,KC_APP), MO(FUNCS), KC_SPC, KC_SPC , KC_EQL, KC_MINS, KC_DEL , KC_INS, KC_ENT),

/* Usage of the Hiragana/Katakana Layers:
   - Turn on the respective layer with GUI+DEL or GUI+INS
   - Alternate between Windows/Linux input modes with SHIFT+DEL
   - Press SHIFT+INS to return to English
   - Japanese numerals along top row are 1-10 (いち-十)
   - Shift+9, Shift+0 (parens) will create 「」
   - Shift+<, Shift+> (square brackets) will create 〈〉
   - PLACE NUMBERS e.g., 10, 100, 1000 can be written in kanji
     by using the 1e_ syntax, representing how many zeroes in the number:
     - 1e1 -> 十 (10), 1e2 -> 百 (100), 1e8 -> 億
     - The only exception: 1e0 -> 〇
       (十 is far more common than 〇, otherwise these would be reversed
        and 〇 would be on the 0 and 1e1 would be 10)
   - CHARACTERS e.g., "ha" -> は
*/

[HIRAGANA] = LAYOUT_preonic_grid(
  QK_GESC          , UC(JP_NUM_1), UC(JP_NUM_2), UC(JP_NUM_3)   , UC(JP_NUM_4), UC(JP_NUM_5), KC_DEL , UC(JP_NUM_6), UC(JP_NUM_7), UC(JP_NUM_8), UC(JP_NUM_9), UC(JP_NUM_10),
  KC_TAB           , KC_NO       , KC_W        , UC(HRGN_E)     , KC_R        , KC_T        , KC_BSPC, KC_Y        , UC(HRGN_U)  , UC(HRGN_I)  , UC(HRGN_O)  , KC_P         ,
  MO(GUIS)         , UC(HRGN_A)  , KC_S        , KC_D           , KC_F        , KC_G        , KC_ENT , KC_H        , KC_J        , KC_K        , KC_NO       , UC(0x3099)   ,
  MO(HIRAGANA_SUPP), KC_Z        , KC_NO       , KC_C           , KC_V        , KC_B        , KC_TAB , UC(HRGN_N)  , KC_M        , KC_COMM     , KC_DOT      , KC_SLSH      ,
  KC_LCTL          , KC_LALT     , KC_LGUI     , LT(GUIS,KC_APP), MO(FUNCS)   , KC_SPC      , KC_SPC , KC_EQL      , KC_MINS     , KC_DEL      , KC_INS      , KC_ENT)      ,

/* HIRAGANA_SUPP is a pseudoshifted layer; pressing and holding shift provides access
   to size-shifted chars and square/angle brackets. */

[HIRAGANA_SUPP] = LAYOUT_preonic_grid(
  KC_TRNS , KC_NO        , KC_NO  , KC_NO        , KC_NO  , KC_NO          , KC_TRNS, KC_NO     , KC_NO        , KC_NO        , UC(0x300C)   , UC(0x300D),
  KC_TRNS , KC_TRNS      , KC_TRNS, UC(HRGN_E_SM), KC_TRNS, UC(HRGN_TSU_SM), KC_TRNS, KC_TRNS   , UC(HRGN_U_SM), UC(HRGN_I_SM), UC(HRGN_O_SM), KC_TRNS   ,
  KC_NO   , UC(HRGN_A_SM), KC_TRNS, KC_TRNS      , KC_TRNS, KC_TRNS        , KC_TRNS, KC_TRNS   , KC_TRNS      , KC_TRNS      , KC_TRNS      , UC(0x309A),
  KC_TRNS , KC_TRNS      , KC_TRNS, KC_TRNS      , KC_TRNS, KC_TRNS        , KC_TRNS, UC(HRGN_N), KC_TRNS      , UC(0x3008)   , UC(0x3009)   , KC_TRNS   ,
  KC_LCTL , KC_TRNS      , KC_TRNS, KC_TRNS      , KC_TRNS, KC_TRNS        , KC_TRNS, KC_TRNS   , KC_TRNS      , UC_NEXT      , ENG_GO       , KC_TRNS)  ,

[KATAKANA] = LAYOUT_preonic_grid(
  QK_GESC          , UC(JP_NUM_1), UC(JP_NUM_2), UC(JP_NUM_3)   , UC(JP_NUM_4), UC(JP_NUM_5), KC_DEL , UC(JP_NUM_6), UC(JP_NUM_7), UC(JP_NUM_8), UC(JP_NUM_9), UC(JP_NUM_10),
  KC_TAB           , KC_NO       , KC_W        , UC(KTKN_E)     , KC_R        , KC_T        , KC_BSPC, KC_Y        , UC(KTKN_U)  , UC(KTKN_I)  , UC(KTKN_O)  , KC_P         ,
  MO(GUIS)         , UC(KTKN_A)  , KC_S        , KC_D           , KC_F        , KC_G        , KC_ENT , KC_H        , KC_J        , KC_K        , KC_NO       , UC(0x3099)   ,
  MO(KATAKANA_SUPP), KC_Z        , KC_NO       , KC_C           , KC_V        , KC_B        , KC_TAB , UC(KTKN_N)  , KC_M        , KC_COMM     , KC_DOT      , KC_SLSH      ,
  KC_LCTL          , KC_LALT     , KC_LGUI     , LT(GUIS,KC_APP), MO(FUNCS)   , KC_SPC      , KC_SPC , KC_EQL      , UC(0x30FC)  , KC_DEL      , KC_INS      , KC_ENT)      ,

/* KATAKANA_SUPP is a pseudoshifted layer; pressing and holding shift provides access
   to size-shifted chars and square/angle brackets. */

[KATAKANA_SUPP] = LAYOUT_preonic_grid(
  KC_TRNS , KC_NO        , KC_NO  , KC_NO        , KC_NO  , KC_NO          , KC_TRNS, KC_NO     , KC_NO        , KC_NO        , UC(0x300C)   , UC(0x300D),
  KC_TRNS , KC_TRNS      , KC_TRNS, UC(KTKN_E_SM), KC_TRNS, UC(KTKN_TSU_SM), KC_TRNS, KC_TRNS   , UC(KTKN_U_SM), UC(KTKN_I_SM), UC(KTKN_O_SM), KC_TRNS   ,
  KC_NO   , UC(KTKN_A_SM), KC_TRNS, KC_TRNS      , KC_TRNS, KC_TRNS        , KC_TRNS, KC_TRNS   , KC_TRNS      , KC_TRNS      , KC_TRNS      , UC(0x309A),
  KC_TRNS , KC_TRNS      , KC_TRNS, KC_TRNS      , KC_TRNS, KC_TRNS        , KC_TRNS, UC(KTKN_N), KC_TRNS      , UC(0x3008)   , UC(0x3009)   , KC_TRNS   ,
  KC_LCTL , KC_TRNS      , KC_TRNS, KC_TRNS      , KC_TRNS, KC_TRNS        , KC_TRNS, UC(0x003D), UC(0x2E40)   , UC_NEXT      , ENG_GO       , KC_TRNS)  ,

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
  KC_GRV , LGUI(KC_1), LGUI(KC_2)   , LGUI(KC_3)   , LGUI(KC_4)   , LGUI(KC_5), LGUI(KC_DEL) , LGUI(KC_6)  , LGUI(KC_7), LGUI(KC_8), LGUI(KC_9), LGUI(KC_0)   ,
  KC_NO  , KC_NO     , LGUI(KC_LBRC), LGUI(KC_UP)  , LGUI(KC_RBRC), KC_NO     , LGUI(KC_BSPC), KC_NO       , KC_NO     , KC_NO     , KC_LBRC   , KC_RBRC      ,
  KC_TRNS, KC_NO     , LGUI(KC_LEFT), LGUI(KC_DOWN), LGUI(KC_RGHT), KC_NO     , LGUI(KC_ENT) , KC_NO       , KC_NO     , KC_NO     , LGUI(KC_L), KC_QUOT      ,
  KC_LSFT, KC_NO     , KC_NO        , KC_NO        , KC_NO        , KC_NO     , LGUI(KC_TAB) , KC_NO       , KC_NO     , KC_NO     , KC_NO     , KC_BSLS      ,
  KC_LCTL, KC_LALT   , KC_NO        , KC_TRNS      , KC_TRNS      , KC_NO     , KC_NO        , KC_NO       , KC_NO     , HRGA_GO   , KTKN_GO   , LGUI(KC_END)),

};

