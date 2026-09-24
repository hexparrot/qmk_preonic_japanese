#include "jp_ime.h"
#include <string.h>

/* Romaji -> kana IME.
 *
 * Vowel keys (あいうえお / アイウエオ) and ん / ン are UC() keycodes that QMK
 * types on its own. Consonant keys are plain KC_ letters and are held here as
 * pending romaji until a vowel completes them; the completed kana is looked up
 * in romaji_table (hiragana) and converted to katakana on the katakana layer.
 *
 *   - doubled consonant (kka, ssha, ttsu) and "tch" prefix -> っ + kana
 *   - ん is typed immediately; a following vowel or y rewrites it (ん + あ -> な,
 *     ん + y + あ -> にゃ). ん ん confirms the first ん and types nothing more.
 *   - an invalid consonant drops the stale pending romaji and starts over
 *   - Backspace with romaji pending removes the last pending letter
 *   - any other key drops pending romaji and is passed through untouched
 *   - dakuten / handakuten after a kana with a voiced form replace it
 *     (つ + ゛ -> づ); otherwise the combining mark is typed
 *   - 1 e _ : 一 え then 0/1/2/3/4/8/w -> 〇 十 百 千 万 億 兆
 */

typedef struct {
  const char *romaji;
  const char *kana;  // hiragana, UTF-8
} romaji_entry_t;

static const romaji_entry_t romaji_table[] = {
  {"ka","か"},{"ki","き"},{"ku","く"},{"ke","け"},{"ko","こ"},
  {"kya","きゃ"},{"kyi","きぃ"},{"kyu","きゅ"},{"kye","きぇ"},{"kyo","きょ"},
  {"ga","が"},{"gi","ぎ"},{"gu","ぐ"},{"ge","げ"},{"go","ご"},
  {"gya","ぎゃ"},{"gyi","ぎぃ"},{"gyu","ぎゅ"},{"gye","ぎぇ"},{"gyo","ぎょ"},
  {"sa","さ"},{"si","し"},{"su","す"},{"se","せ"},{"so","そ"},
  {"sha","しゃ"},{"shi","し"},{"shu","しゅ"},{"she","しぇ"},{"sho","しょ"},
  {"sya","しゃ"},{"syi","しぃ"},{"syu","しゅ"},{"sye","しぇ"},{"syo","しょ"},
  {"za","ざ"},{"zi","じ"},{"zu","ず"},{"ze","ぜ"},{"zo","ぞ"},
  {"zya","じゃ"},{"zyi","じぃ"},{"zyu","じゅ"},{"zye","じぇ"},{"zyo","じょ"},
  {"ja","じゃ"},{"ji","じ"},{"ju","じゅ"},{"je","じぇ"},{"jo","じょ"},
  {"jya","じゃ"},{"jyi","じぃ"},{"jyu","じゅ"},{"jye","じぇ"},{"jyo","じょ"},
  {"ta","た"},{"ti","ち"},{"tu","つ"},{"te","て"},{"to","と"},
  {"tya","ちゃ"},{"tyi","ちぃ"},{"tyu","ちゅ"},{"tye","ちぇ"},{"tyo","ちょ"},
  {"tsa","つぁ"},{"tsi","つぃ"},{"tsu","つ"},{"tse","つぇ"},{"tso","つぉ"},
  {"tha","てゃ"},{"thi","てぃ"},{"thu","てゅ"},{"the","てぇ"},{"tho","てょ"},
  {"twa","とぁ"},{"twi","とぃ"},{"twu","とぅ"},{"twe","とぇ"},{"two","とぉ"},
  {"ca","か"},{"ci","し"},{"cu","く"},{"ce","せ"},{"co","こ"},
  {"cha","ちゃ"},{"chi","ち"},{"chu","ちゅ"},{"che","ちぇ"},{"cho","ちょ"},
  {"cya","ちゃ"},{"cyi","ちぃ"},{"cyu","ちゅ"},{"cye","ちぇ"},{"cyo","ちょ"},
  {"da","だ"},{"di","ぢ"},{"du","づ"},{"de","で"},{"do","ど"},
  {"dya","ぢゃ"},{"dyi","ぢぃ"},{"dyu","ぢゅ"},{"dye","ぢぇ"},{"dyo","ぢょ"},
  {"dha","でゃ"},{"dhi","でぃ"},{"dhu","でゅ"},{"dhe","でぇ"},{"dho","でょ"},
  {"dwa","どぁ"},{"dwi","どぃ"},{"dwu","どぅ"},{"dwe","どぇ"},{"dwo","どぉ"},
  {"dzu","づ"},{"dji","ぢ"},
  // n* is only reachable after the ん key (see handle_n)
  {"na","な"},{"ni","に"},{"nu","ぬ"},{"ne","ね"},{"no","の"},
  {"nya","にゃ"},{"nyi","にぃ"},{"nyu","にゅ"},{"nye","にぇ"},{"nyo","にょ"},
  {"ha","は"},{"hi","ひ"},{"hu","ふ"},{"he","へ"},{"ho","ほ"},
  {"hya","ひゃ"},{"hyi","ひぃ"},{"hyu","ひゅ"},{"hye","ひぇ"},{"hyo","ひょ"},
  {"fa","ふぁ"},{"fi","ふぃ"},{"fu","ふ"},{"fe","ふぇ"},{"fo","ふぉ"},
  {"fya","ふゃ"},{"fyu","ふゅ"},{"fyo","ふょ"},
  {"ba","ば"},{"bi","び"},{"bu","ぶ"},{"be","べ"},{"bo","ぼ"},
  {"bya","びゃ"},{"byi","びぃ"},{"byu","びゅ"},{"bye","びぇ"},{"byo","びょ"},
  {"pa","ぱ"},{"pi","ぴ"},{"pu","ぷ"},{"pe","ぺ"},{"po","ぽ"},
  {"pya","ぴゃ"},{"pyi","ぴぃ"},{"pyu","ぴゅ"},{"pye","ぴぇ"},{"pyo","ぴょ"},
  {"ma","ま"},{"mi","み"},{"mu","む"},{"me","め"},{"mo","も"},
  {"mya","みゃ"},{"myi","みぃ"},{"myu","みゅ"},{"mye","みぇ"},{"myo","みょ"},
  {"ya","や"},{"yi","い"},{"yu","ゆ"},{"ye","いぇ"},{"yo","よ"},
  {"ra","ら"},{"ri","り"},{"ru","る"},{"re","れ"},{"ro","ろ"},
  {"rya","りゃ"},{"ryi","りぃ"},{"ryu","りゅ"},{"rye","りぇ"},{"ryo","りょ"},
  {"wa","わ"},{"wi","うぃ"},{"wu","う"},{"we","うぇ"},{"wo","を"},
  {"wha","うぁ"},{"whi","うぃ"},{"whu","う"},{"whe","うぇ"},{"who","うぉ"},
  {"wyi","ゐ"},{"wye","ゑ"},
  {"va","ゔぁ"},{"vi","ゔぃ"},{"vu","ゔ"},{"ve","ゔぇ"},{"vo","ゔぉ"},
  {"vya","ゔゃ"},{"vyu","ゔゅ"},{"vyo","ゔょ"},
  {"xa","ぁ"},{"xi","ぃ"},{"xu","ぅ"},{"xe","ぇ"},{"xo","ぉ"},
  {"xya","ゃ"},{"xyu","ゅ"},{"xyo","ょ"},
  {"xtu","っ"},{"xtsu","っ"},{"xwa","ゎ"},{"xka","ゕ"},{"xke","ゖ"},
  {"la","ぁ"},{"li","ぃ"},{"lu","ぅ"},{"le","ぇ"},{"lo","ぉ"},
  {"lya","ゃ"},{"lyu","ゅ"},{"lyo","ょ"},
  {"ltu","っ"},{"ltsu","っ"},{"lwa","ゎ"},{"lka","ゕ"},{"lke","ゖ"},
};
#define ROMAJI_TABLE_LEN (sizeof(romaji_table) / sizeof(romaji_table[0]))

#define PENDING_MAX 3  // longest consonant run before a vowel: "xts", "kky", "tth"

static char     pending[PENDING_MAX + 1] = {0};
static uint8_t  plen       = 0;
static bool     n_shown    = false;  // pending[0] == 'n' is an ん already on screen
static uint8_t  num_state  = 0;      // 1 after 一, 2 after 一え
static uint32_t last_kana  = 0;      // last kana on screen, for dakuten
static uint16_t deadline   = 0;
static uint8_t  ime_char_count = 0;

/* ---------------- counter ---------------- */

static void count_add(uint8_t n) {
  ime_char_count = (ime_char_count > 255 - n) ? 255 : ime_char_count + n;
}
static void count_sub(uint8_t n) {
  ime_char_count = (ime_char_count > n) ? ime_char_count - n : 0;
}
void    ime_reset_word_count(void) { ime_char_count = 0; }
uint8_t ime_get_word_count(void)   { return ime_char_count; }

/* ---------------- state ---------------- */

static void clear_pending(void) {
  plen = 0;
  pending[0] = 0;
  n_shown = false;
}

void ime_clear(void) {
  clear_pending();
  num_state = 0;
}

void ime_matrix_scan(void) {
  if ((plen || num_state) && timer_expired(timer_read(), deadline)) {
    ime_clear();
  }
}

/* ---------------- UTF-8 / kana helpers ---------------- */

static uint32_t utf8_next(const char **s) {
  const uint8_t *p = (const uint8_t *)*s;
  uint32_t cp;
  if (p[0] < 0x80)      { cp = p[0];                                                    *s += 1; }
  else if (p[0] < 0xE0) { cp = ((p[0] & 0x1F) << 6)  |  (p[1] & 0x3F);                  *s += 2; }
  else                  { cp = ((p[0] & 0x0F) << 12) | ((p[1] & 0x3F) << 6) | (p[2] & 0x3F); *s += 3; }
  return cp;
}

static char *utf8_put(char *o, uint32_t cp) {
  if (cp < 0x80)       { *o++ = cp; }
  else if (cp < 0x800) { *o++ = 0xC0 | (cp >> 6);  *o++ = 0x80 | (cp & 0x3F); }
  else                 { *o++ = 0xE0 | (cp >> 12); *o++ = 0x80 | ((cp >> 6) & 0x3F); *o++ = 0x80 | (cp & 0x3F); }
  return o;
}

static bool is_katakana_layer(void) {
  return !IS_LAYER_ON(HIRAGANA) && IS_LAYER_ON(KATAKANA);
}

// Types `hira` (converted to katakana on the katakana layer), optionally
// prefixed with a small tsu and optionally replacing the ん on screen.
static void emit_kana(const char *hira, bool sokuon, bool replace_n) {
  char     buf[16];
  char    *o    = buf;
  bool     kata = is_katakana_layer();
  uint8_t  n    = 0;
  uint32_t cp   = 0;

  if (sokuon) { o = utf8_put(o, kata ? 0x30C3 : 0x3063); n++; }
  for (const char *s = hira; *s; n++) {
    cp = utf8_next(&s);
    if (kata && cp >= 0x3041 && cp <= 0x3096) cp += 0x60;
    o = utf8_put(o, cp);
  }
  *o = 0;

  if (replace_n) {
    tap_code(KC_BSPC);
    count_sub(1);
  }
  send_unicode_string(buf);
  count_add(n);
  last_kana = cp;
}

static void send_codepoint(uint32_t cp) {
  char buf[4];
  *utf8_put(buf, cp) = 0;
  send_unicode_string(buf);
}

// Precomposed voiced (handaku=false) or semi-voiced form of cp, or 0.
static uint32_t voiced_form(uint32_t cp, bool handaku) {
  uint32_t off = (cp >= 0x30A1 && cp <= 0x30F6) ? 0x60 : 0;
  uint32_t h   = cp - off;
  if (handaku) {
    switch (h) {
    case 0x306F: case 0x3072: case 0x3075: case 0x3078: case 0x307B:  // はひふへほ
      return h + 2 + off;
    }
    return 0;
  }
  switch (h) {
  case 0x3046:  // う -> ゔ
    return 0x3094 + off;
  case 0x304B: case 0x304D: case 0x304F: case 0x3051: case 0x3053:  // かきくけこ
  case 0x3055: case 0x3057: case 0x3059: case 0x305B: case 0x305D:  // さしすせそ
  case 0x305F: case 0x3061: case 0x3064: case 0x3066: case 0x3068:  // たちつてと
  case 0x306F: case 0x3072: case 0x3075: case 0x3078: case 0x307B:  // はひふへほ
    return h + 1 + off;
  }
  return 0;
}

/* ---------------- romaji lookup ---------------- */

// Can `c` be doubled into a small tsu (kka -> っか)?
static bool doubles(char c) {
  return c && !strchr("aiueonxl", c);
}

// Strips a sokuon-forming prefix ("kk..." or "tc...") and reports it.
static const char *strip_sokuon(const char *r, bool *sokuon) {
  *sokuon = false;
  if ((r[0] == r[1] && doubles(r[0])) || (r[0] == 't' && r[1] == 'c')) {
    *sokuon = true;
    return r + 1;
  }
  return r;
}

static const char *lookup(const char *romaji, bool *sokuon) {
  const char *r = strip_sokuon(romaji, sokuon);
  for (uint8_t i = 0; i < ROMAJI_TABLE_LEN; i++) {
    if (strcmp(romaji_table[i].romaji, r) == 0) return romaji_table[i].kana;
  }
  return NULL;
}

// Is `romaji` (consonants only) the start of some table entry?
static bool is_prefix(const char *romaji) {
  bool sokuon;
  const char *r = strip_sokuon(romaji, &sokuon);
  size_t len = strlen(r);
  if (sokuon && romaji[0] == romaji[1] && len == 1) return true;  // "kk"
  for (uint8_t i = 0; i < ROMAJI_TABLE_LEN; i++) {
    if (strncmp(romaji_table[i].romaji, r, len) == 0 && romaji_table[i].romaji[len]) return true;
  }
  return false;
}

// pending + c into out; returns false if it would overflow.
static bool with_char(char *out, char c) {
  if (plen >= PENDING_MAX + 1) return false;
  memcpy(out, pending, plen);
  out[plen] = c;
  out[plen + 1] = 0;
  return true;
}

/* ---------------- key classification ---------------- */

static char consonant_of(uint16_t kc) {
  if (kc < KC_A || kc > KC_Z) return 0;
  char c = 'a' + (kc - KC_A);
  return strchr("aiueonq", c) ? 0 : c;
}

static char vowel_of(uint16_t kc) {
  switch (kc) {
  case UC(HRGN_A): case UC(KTKN_A): return 'a';
  case UC(HRGN_I): case UC(KTKN_I): return 'i';
  case UC(HRGN_U): case UC(KTKN_U): return 'u';
  case UC(HRGN_E): case UC(KTKN_E): return 'e';
  case UC(HRGN_O): case UC(KTKN_O): return 'o';
  }
  return 0;
}

static bool is_n_key(uint16_t kc) {
  return kc == UC(HRGN_N) || kc == UC(KTKN_N);
}

// y + small ぁぅぉ -> ゃゅょ
static const char *small_y_of(uint16_t kc) {
  switch (kc) {
  case UC(HRGN_A_SM): case UC(KTKN_A_SM): return "ゃ";
  case UC(HRGN_U_SM): case UC(KTKN_U_SM): return "ゅ";
  case UC(HRGN_O_SM): case UC(KTKN_O_SM): return "ょ";
  }
  return NULL;
}

static bool is_uc(uint16_t kc) {
  return kc >= QK_UNICODE;
}
static uint32_t uc_codepoint(uint16_t kc) {
  return kc - QK_UNICODE;
}
static bool is_counted_kana(uint32_t cp) {
  return cp >= 0x3041 && cp <= 0x30FF && cp != 0x3099 && cp != 0x309A;
}

// 1 e _ place-value kanji
static const char *num_suffix_of(uint16_t kc) {
  switch (kc) {
  case UC(JP_NUM_10): return "〇";  // 1e0: 〇, see README
  case UC(JP_NUM_1):  return "十";
  case UC(JP_NUM_2):  return "百";
  case UC(JP_NUM_3):  return "千";
  case UC(JP_NUM_4):  return "万";
  case UC(JP_NUM_8):  return "億";
  case KC_W:          return "兆";  // 1e12 would need two keys
  }
  return NULL;
}

/* ---------------- prediction (RGB) ---------------- */

bool ime_has_pending(void) {
  return plen > 0 || num_state == 2;
}

bool ime_accepts(uint16_t kc) {
  char tmp[PENDING_MAX + 2];
  bool sokuon;

  if (num_state == 2 && plen == 0) return num_suffix_of(kc) != NULL;

  if (plen == 0) {
    char c = consonant_of(kc);
    if (c) {
      tmp[0] = c;
      tmp[1] = 0;
      return is_prefix(tmp);
    }
    return vowel_of(kc) || is_n_key(kc);
  }
  if (n_shown && plen == 1 && is_n_key(kc)) return true;
  if (plen == 1 && pending[0] == 'y' && small_y_of(kc)) return true;
  char c = consonant_of(kc);
  if (c) return plen < PENDING_MAX && with_char(tmp, c) && is_prefix(tmp);
  char v = vowel_of(kc);
  if (v) return with_char(tmp, v) && lookup(tmp, &sokuon);
  return false;
}

/* ---------------- key handling ---------------- */

// Passes a key through to QMK, keeping the counter and dakuten state right.
static bool pass_through(uint16_t kc) {
  if (is_uc(kc) && is_counted_kana(uc_codepoint(kc))) {
    count_add(1);
    last_kana = uc_codepoint(kc);
  } else {
    last_kana = 0;
  }
  return true;
}

static bool handle_consonant(char c) {
  char tmp[PENDING_MAX + 2];
  if (plen < PENDING_MAX && with_char(tmp, c) && is_prefix(tmp)) {
    memcpy(pending, tmp, plen + 2);
    plen++;
    return false;
  }
  // Invalid continuation: keep any ん on screen, drop stale romaji, start over.
  clear_pending();
  tmp[0] = c;
  tmp[1] = 0;
  if (is_prefix(tmp)) {
    pending[0] = c;
    pending[1] = 0;
    plen = 1;
  }
  return false;
}

static bool handle_vowel(uint16_t kc, char v) {
  char tmp[PENDING_MAX + 2];
  bool sokuon;

  if (plen == 0) return pass_through(kc);
  const char *kana = with_char(tmp, v) ? lookup(tmp, &sokuon) : NULL;
  bool replace_n = n_shown;
  clear_pending();
  if (!kana) return pass_through(kc);
  emit_kana(kana, sokuon, replace_n);
  return false;
}

static bool handle_n(uint16_t kc) {
  if (n_shown && plen == 1) {  // ん ん: confirm the first, type nothing
    clear_pending();
    return false;
  }
  clear_pending();
  n_shown = true;
  pending[0] = 'n';
  pending[1] = 0;
  plen = 1;
  return pass_through(kc);
}

static bool handle_backspace(void) {
  if (plen > 0 && !(n_shown && plen == 1)) {
    pending[--plen] = 0;
    return false;
  }
  clear_pending();
  count_sub(1);
  last_kana = 0;
  return true;
}

static bool handle_dakuten(uint16_t kc) {
  clear_pending();
  uint32_t v = voiced_form(last_kana, uc_codepoint(kc) == SYM_HANDAKUTEN);
  if (!v) {
    last_kana = 0;
    return true;  // no precomposed form: type the combining mark
  }
  tap_code(KC_BSPC);
  send_codepoint(v);
  last_kana = v;
  return false;
}

static bool on_kana_layer(void) {
  return IS_LAYER_ON(HIRAGANA) || IS_LAYER_ON(KATAKANA);
}

bool ime_process_record(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
  case HRGA_GO:
  case KTKN_GO:
    if (record->event.pressed) {
      ime_clear();
      layer_clear();
      layer_on(keycode == HRGA_GO ? HIRAGANA : KATAKANA);
      return false;
    }
    return true;
  case ENG_GO:
    if (record->event.pressed) {
      ime_clear();
      layer_clear();
      return true;
    }
    layer_on(QWERTY);
    return false;
  }

  // Releases always pass: a consonant press that was swallowed has nothing
  // to release, and one that went out as a chord (Ctrl+K) must be released.
  if (!record->event.pressed) return true;

  if (!on_kana_layer()) {
    ime_clear();
    return true;
  }

  // Chords (Ctrl/Alt/GUI) go straight through and end any sequence.
  if ((get_mods() | get_oneshot_mods()) & ~MOD_MASK_SHIFT) {
    ime_clear();
    return true;
  }

  switch (keycode) {
  case KC_LSFT:
  case KC_RSFT:
  case QK_ONE_SHOT_MOD ... QK_ONE_SHOT_MOD_MAX:
  case QK_MOMENTARY ... QK_MOMENTARY_MAX:
    // Don't interrupt a sequence: y + (hold SUPP) + small ぁ -> ゃ.
    return true;
  }

  deadline = record->event.time + TIMEOUT_MS;

  // 1 e _
  uint8_t ns = num_state;
  num_state = 0;
  if (ns == 2 && plen == 0) {
    const char *suffix = num_suffix_of(keycode);
    if (suffix) {
      tap_code(KC_BSPC);
      tap_code(KC_BSPC);
      count_sub(1);  // え (一 is not counted)
      send_unicode_string(suffix);
      last_kana = 0;
      return false;
    }
  }

  char c = consonant_of(keycode);
  if (c) return handle_consonant(c);

  char v = vowel_of(keycode);
  if (v) {
    if (ns == 1 && v == 'e' && plen == 0) num_state = 2;
    return handle_vowel(keycode, v);
  }

  if (is_n_key(keycode)) return handle_n(keycode);

  const char *small_y = small_y_of(keycode);
  if (small_y && plen == 1 && pending[0] == 'y') {
    clear_pending();
    emit_kana(small_y, false, false);
    return false;
  }

  if (keycode == KC_BSPC) return handle_backspace();

  if (keycode == UC(SYM_DAKUTEN) || keycode == UC(SYM_HANDAKUTEN)) return handle_dakuten(keycode);

  // Anything else ends the sequence and is typed as-is.
  clear_pending();
  if (keycode == UC(JP_NUM_1)) num_state = 1;
  return pass_through(keycode);
}
