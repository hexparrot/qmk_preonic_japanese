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
    case UC(0x3040) ... UC(0x30FF):  // Hiragana + Katakana codepoints
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
              send_unicode_string("??????");
              break;
            case UC(HRGN_E):
              send_unicode_string("??????");
              break;
            case UC(HRGN_I):
              send_unicode_string("??????");
              break;
            case UC(HRGN_O):
              send_unicode_string("??????");
              break;
            case UC(HRGN_U):
              send_unicode_string("??????");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH KY_
            switch (keycode) {
            case UC(HRGN_A):
              send_unicode_string("??????");
              break;
            case UC(HRGN_O):
              send_unicode_string("??????");
              break;
            case UC(HRGN_U):
              send_unicode_string("??????");
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
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            send_unicode_string("???");
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
              send_unicode_string("??????");
              break;
            case UC(HRGN_E):
              send_unicode_string("??????");
              break;
            case UC(HRGN_I):
              send_unicode_string("??????");
              break;
            case UC(HRGN_O):
              send_unicode_string("??????");
              break;
            case UC(HRGN_U):
              send_unicode_string("??????");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH GY_
            switch (keycode) {
            case UC(HRGN_A):
              send_unicode_string("??????");
              break;
            case UC(HRGN_O):
              send_unicode_string("??????");
              break;
            case UC(HRGN_U):
              send_unicode_string("??????");
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
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            send_unicode_string("???");
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
              send_unicode_string("??????");
              break;
            case UC(HRGN_E):
              send_unicode_string("??????");
              break;
            case UC(HRGN_I):
              send_unicode_string("??????");
              break;
            case UC(HRGN_O):
              send_unicode_string("??????");
              break;
            case UC(HRGN_U):
              send_unicode_string("??????");
              break;
            case KC_S:
              send_unicode_string("??????");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_S) {
            // MATCH TS_
            switch (keycode) {
            case UC(HRGN_U):
              send_unicode_string("???");
              break;
            case UC(HRGN_U_SM):
              send_unicode_string("???");
              break;
            default:
              send_unicode_string("???");
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
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            send_unicode_string("???");
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
              send_unicode_string("??????");
              break;
            case UC(HRGN_E):
              send_unicode_string("??????");
              break;
            case UC(HRGN_I):
              send_unicode_string("??????");
              break;
            case UC(HRGN_O):
              send_unicode_string("??????");
              break;
            case UC(HRGN_U):
              send_unicode_string("??????");
              break;
            case KC_H:
              send_unicode_string("??????");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_H) {
            // MATCH SH_
            switch (keycode) {
            case UC(HRGN_A):
              send_unicode_string("??????");
              break;
            case UC(HRGN_I):
              send_unicode_string("???");
              break;
            case UC(HRGN_O):
              send_unicode_string("??????");
              break;
            case UC(HRGN_U):
              send_unicode_string("??????");
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
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            send_unicode_string("???");
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
              send_unicode_string("??????");
              break;
            case UC(HRGN_E):
              send_unicode_string("??????");
              break;
            case UC(HRGN_I):
              send_unicode_string("??????");
              break;
            case UC(HRGN_O):
              send_unicode_string("??????");
              break;
            case UC(HRGN_U):
              send_unicode_string("??????");
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
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            send_unicode_string("???");
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
              send_unicode_string("?????????");
              break;
            case UC(HRGN_O):
              send_unicode_string("?????????");
              break;
            case UC(HRGN_U):
              send_unicode_string("?????????");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH JY_
            switch (keycode) {
            case UC(HRGN_A):
              send_unicode_string("??????");
              break;
            case UC(HRGN_O):
              send_unicode_string("??????");
              break;
            case UC(HRGN_U):
              send_unicode_string("??????");
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
            send_unicode_string("??????");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("??????");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            send_unicode_string("??????");
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
              send_unicode_string("??????");
              break;
            case UC(HRGN_I):
              send_unicode_string("???");
              break;
            case UC(HRGN_O):
              send_unicode_string("??????");
              break;
            case UC(HRGN_U):
              send_unicode_string("??????");
              break;
            }
          }
          // any unmatched c** 3char clears
          unregister_code(keycode);
          clear_recent_keys();
          return false;
        } else if (recent[RECENT_SIZE - 2] == KC_C) {
          // if C isn't 3rd most recent, is it still 2nd most recent?
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
              send_unicode_string("??????");
              break;
            case UC(HRGN_E):
              send_unicode_string("??????");
              break;
            case UC(HRGN_I):
              send_unicode_string("??????");
              break;
            case UC(HRGN_O):
              send_unicode_string("??????");
              break;
            case UC(HRGN_U):
              send_unicode_string("??????");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Z) {
            // MATCH DZ_
            switch (keycode) {
            case UC(HRGN_U):
              send_unicode_string("??????");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_J) {
            // MATCH DJ_
            switch (keycode) {
            case UC(HRGN_I):
              send_unicode_string("???");
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
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            send_unicode_string("???");
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
              send_unicode_string("??????");
              break;
            case UC(HRGN_E):
              tap_code(KC_BSPC);
              send_unicode_string("??????");
              break;
            case UC(HRGN_I):
              tap_code(KC_BSPC);
              send_unicode_string("??????");
              break;
            case UC(HRGN_O):
              tap_code(KC_BSPC);
              send_unicode_string("??????");
              break;
            case UC(HRGN_U):
              tap_code(KC_BSPC);
              send_unicode_string("??????");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH NY_
            switch (keycode) {
            case UC(HRGN_A):
              tap_code(KC_BSPC);
              send_unicode_string("??????");
              break;
            case UC(HRGN_O):
              tap_code(KC_BSPC);
              send_unicode_string("??????");
              break;
            case UC(HRGN_U):
              tap_code(KC_BSPC);
              send_unicode_string("??????");
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
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            tap_code(KC_BSPC);
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            tap_code(KC_BSPC);
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            tap_code(KC_BSPC);
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            tap_code(KC_BSPC);
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_N):
          case KC_Y:
            // UC(HRGN_N),Y exit immediately *without* clear to permit access to above 3 char stanza
            return false;
          default:
            // any unmatched n* 2char clears
            clear_recent_keys();
          }
          return false;
        }

        // H - SERIES
        if (recent[RECENT_SIZE - 3] == KC_H) {
          if (recent[RECENT_SIZE - 2] == KC_H) {
            // MATCH HH_
            switch (keycode) {
            case UC(HRGN_A):
              send_unicode_string("??????");
              break;
            case UC(HRGN_E):
              send_unicode_string("??????");
              break;
            case UC(HRGN_I):
              send_unicode_string("??????");
              break;
            case UC(HRGN_O):
              send_unicode_string("??????");
              break;
            case UC(HRGN_U):
              send_unicode_string("??????");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH HY_
            switch (keycode) {
            case UC(HRGN_A):
              send_unicode_string("??????");
              break;
            case UC(HRGN_O):
              send_unicode_string("??????");
              break;
            case UC(HRGN_U):
              send_unicode_string("??????");
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
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            send_unicode_string("???");
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
              send_unicode_string("??????");
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
            send_unicode_string("???");
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
              send_unicode_string("??????");
              break;
            case UC(HRGN_E):
              send_unicode_string("??????");
              break;
            case UC(HRGN_I):
              send_unicode_string("??????");
              break;
            case UC(HRGN_O):
              send_unicode_string("??????");
              break;
            case UC(HRGN_U):
              send_unicode_string("??????");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH BY_
            switch (keycode) {
            case UC(HRGN_A):
              send_unicode_string("??????");
              break;
            case UC(HRGN_O):
              send_unicode_string("??????");
              break;
            case UC(HRGN_U):
              send_unicode_string("??????");
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
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            send_unicode_string("???");
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
              send_unicode_string("??????");
              break;
            case UC(HRGN_E):
              send_unicode_string("??????");
              break;
            case UC(HRGN_I):
              send_unicode_string("??????");
              break;
            case UC(HRGN_O):
              send_unicode_string("??????");
              break;
            case UC(HRGN_U):
              send_unicode_string("??????");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH PY_
            switch (keycode) {
            case UC(HRGN_A):
              send_unicode_string("??????");
              break;
            case UC(HRGN_O):
              send_unicode_string("??????");
              break;
            case UC(HRGN_U):
              send_unicode_string("??????");
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
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            send_unicode_string("???");
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
              send_unicode_string("??????");
              break;
            case UC(HRGN_E):
              send_unicode_string("??????");
              break;
            case UC(HRGN_I):
              send_unicode_string("??????");
              break;
            case UC(HRGN_O):
              send_unicode_string("??????");
              break;
            case UC(HRGN_U):
              send_unicode_string("??????");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH MY_
            switch (keycode) {
            case UC(HRGN_A):
              send_unicode_string("??????");
              break;
            case UC(HRGN_O):
              send_unicode_string("??????");
              break;
            case UC(HRGN_U):
              send_unicode_string("??????");
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
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            send_unicode_string("???");
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

        // Y - SERIES
        if (recent[RECENT_SIZE - 3] == KC_Y) {
          if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH YY_
            switch (keycode) {
            case UC(HRGN_A):
              send_unicode_string("??????");
              break;
            case UC(HRGN_O):
              send_unicode_string("??????");
              break;
            case UC(HRGN_U):
              send_unicode_string("??????");
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
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            send_unicode_string("???");
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

        // R - SERIES
        if (recent[RECENT_SIZE - 3] == KC_R) {
          if (recent[RECENT_SIZE - 2] == KC_R) {
            // MATCH RR_
            switch (keycode) {
            case UC(HRGN_A):
              send_unicode_string("??????");
              break;
            case UC(HRGN_E):
              send_unicode_string("??????");
              break;
            case UC(HRGN_I):
              send_unicode_string("??????");
              break;
            case UC(HRGN_O):
              send_unicode_string("??????");
              break;
            case UC(HRGN_U):
              send_unicode_string("??????");
              break;
            }
          } else if (recent[RECENT_SIZE - 2] == KC_Y) {
            // MATCH RY_
            switch (keycode) {
            case UC(HRGN_A):
              send_unicode_string("??????");
              break;
            case UC(HRGN_O):
              send_unicode_string("??????");
              break;
            case UC(HRGN_U):
              send_unicode_string("??????");
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
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_E):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_I):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_U):
            send_unicode_string("???");
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
              send_unicode_string("??????");
              break;
            case UC(HRGN_O):
              send_unicode_string("??????");
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
            send_unicode_string("???");
            clear_recent_keys();
            break;
          case UC(HRGN_O):
            send_unicode_string("???");
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

      } // end hiragana layer check
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

[HIRAGANA] = LAYOUT_preonic_grid(
  QK_GESC          , KC_1      , KC_2   , KC_3           , KC_4     , KC_5  , KC_DEL , KC_6      , KC_7      , KC_8      , KC_9      , KC_0      ,
  KC_TAB           , KC_NO     , KC_W   , UC(HRGN_E)     , KC_R     , KC_T  , KC_BSPC, KC_Y      , UC(HRGN_U), UC(HRGN_I), UC(HRGN_O), KC_P      ,
  MO(GUIS)         , UC(HRGN_A), KC_S   , KC_D           , KC_F     , KC_G  , KC_ENT , KC_H      , KC_J      , KC_K      , KC_NO     , UC(0x3099),
  MO(HIRAGANA_SUPP), KC_Z      , KC_NO  , KC_C           , KC_V     , KC_B  , KC_TAB , UC(HRGN_N), KC_M      , KC_COMM   , KC_DOT    , KC_SLSH   ,
  KC_LCTL          , KC_LALT   , KC_LGUI, LT(GUIS,KC_APP), MO(FUNCS), KC_SPC, KC_SPC , KC_EQL    , KC_MINS   , ENG_GO    , UC_NEXT   , KC_ENT)   ,

/* HIRAGANA_SUPP is a pseudoshifted layer; pressing and holding shift provides access
   to size-shifted chars and square/angle brackets. */

[HIRAGANA_SUPP] = LAYOUT_preonic_grid(
  KC_TRNS , KC_NO        , KC_NO  , KC_NO        , KC_NO  , KC_NO          , KC_TRNS, KC_NO     , KC_NO        , KC_NO        , UC(0x300C)   , UC(0x300D),
  KC_TRNS , KC_TRNS      , KC_TRNS, UC(HRGN_E_SM), KC_TRNS, UC(HRGN_TSU_SM), KC_TRNS, KC_TRNS   , UC(HRGN_U_SM), UC(HRGN_I_SM), UC(HRGN_O_SM), KC_TRNS   ,
  KC_NO   , UC(HRGN_A_SM), KC_TRNS, KC_TRNS      , KC_TRNS, KC_TRNS        , KC_TRNS, KC_TRNS   , KC_TRNS      , KC_TRNS      , KC_TRNS      , UC(0x309A),
  KC_TRNS , KC_TRNS      , KC_TRNS, KC_TRNS      , KC_TRNS, KC_TRNS        , KC_TRNS, UC(HRGN_N), KC_TRNS      , UC(0x3008)   , UC(0x3009)   , KC_TRNS   ,
  KC_LCTL , KC_TRNS      , KC_TRNS, KC_TRNS      , KC_TRNS, KC_TRNS        , KC_TRNS, KC_TRNS   , KC_TRNS      , KC_TRNS      , KC_TRNS      , KC_TRNS)  ,

[KATAKANA] = LAYOUT_preonic_grid(
  QK_GESC          , KC_1      , KC_2   , KC_3           , KC_4     , KC_5  , KC_DEL , KC_6      , KC_7      , KC_8      , KC_9      , KC_0      ,
  KC_TAB           , KC_NO     , KC_W   , UC(KTKN_E)     , KC_R     , KC_T  , KC_BSPC, KC_Y      , UC(KTKN_U), UC(KTKN_I), UC(KTKN_O), KC_P      ,
  MO(GUIS)         , UC(KTKN_A), KC_S   , KC_D           , KC_F     , KC_G  , KC_ENT , KC_H      , KC_J      , KC_K      , KC_NO     , UC(0x3099),
  MO(KATAKANA_SUPP), KC_Z      , KC_NO  , KC_C           , KC_V     , KC_B  , KC_TAB , UC(KTKN_N), KC_M      , KC_COMM   , KC_DOT    , KC_SLSH   ,
  KC_LCTL          , KC_LALT   , KC_LGUI, LT(GUIS,KC_APP), MO(FUNCS), KC_SPC, KC_SPC , KC_EQL    , UC(0x30FC), ENG_GO    , UC_NEXT   , KC_ENT)   ,

/* KATAKANA_SUPP is a pseudoshifted layer; pressing and holding shift provides access
   to size-shifted chars and square/angle brackets. */

[KATAKANA_SUPP] = LAYOUT_preonic_grid(
  KC_TRNS , KC_NO        , KC_NO  , KC_NO        , KC_NO  , KC_NO          , KC_TRNS, KC_NO     , KC_NO        , KC_NO        , UC(0x300C)   , UC(0x300D),
  KC_TRNS , KC_TRNS      , KC_TRNS, UC(KTKN_E_SM), KC_TRNS, UC(KTKN_TSU_SM), KC_TRNS, KC_TRNS   , UC(KTKN_U_SM), UC(KTKN_I_SM), UC(KTKN_O_SM), KC_TRNS   ,
  KC_NO   , UC(KTKN_A_SM), KC_TRNS, KC_TRNS      , KC_TRNS, KC_TRNS        , KC_TRNS, KC_TRNS   , KC_TRNS      , KC_TRNS      , KC_TRNS      , UC(0x309A),
  KC_TRNS , KC_TRNS      , KC_TRNS, KC_TRNS      , KC_TRNS, KC_TRNS        , KC_TRNS, UC(KTKN_N), KC_TRNS      , UC(0x3008)   , UC(0x3009)   , KC_TRNS   ,
  KC_LCTL , KC_TRNS      , KC_TRNS, KC_TRNS      , KC_TRNS, KC_TRNS        , KC_TRNS, UC(0x003D), UC(0x2E40)   , KC_TRNS      , KC_TRNS      , KC_TRNS)  ,

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
