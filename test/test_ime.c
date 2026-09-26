// Replays key sequences through jp_ime.c and checks what reaches the "screen".
#include <stdio.h>
#include <string.h>
#include "../jp_ime.h"

/* ---------------- fake host ---------------- */

uint32_t layer_state;
static uint8_t  mods;
static uint16_t now;
static char     screen[512];

void    layer_clear(void)       { layer_state = 0; }
void    layer_on(uint8_t l)     { layer_state |= 1u << l; }
uint8_t get_mods(void)          { return mods; }
uint8_t get_oneshot_mods(void)  { return 0; }
uint16_t timer_read(void)       { return now; }
bool    timer_expired(uint16_t t, uint16_t d) { return (int16_t)(t - d) >= 0; }

static void backspace(void) {
  size_t n = strlen(screen);
  if (!n) return;
  do n--; while (n && ((uint8_t)screen[n] & 0xC0) == 0x80);
  screen[n] = 0;
}

void tap_code(uint8_t kc) {
  if (kc == KC_BSPC) backspace();
}

void send_unicode_string(const char *s) { strcat(screen, s); }

// What QMK would do with a key the IME returned true for.
static void host_key(uint16_t kc) {
  char buf[4] = {0};
  if (kc >= QK_UNICODE) {
    uint32_t cp = kc - QK_UNICODE;
    buf[0] = 0xE0 | (cp >> 12); buf[1] = 0x80 | ((cp >> 6) & 0x3F); buf[2] = 0x80 | (cp & 0x3F);
    strcat(screen, buf);
  } else if (kc == KC_BSPC) {
    backspace();
  } else if (kc == KC_SPC) {
    strcat(screen, " ");
  } else if (kc == KC_ENT) {
    strcat(screen, "⏎");
  } else if (kc >= KC_A && kc <= KC_Z) {
    buf[0] = 'a' + (kc - KC_A);
    strcat(screen, buf);
  }
}

static void press(uint16_t kc) {
  keyrecord_t r = { .event = { .pressed = true, .time = now } };
  if (ime_process_record(kc, &r)) host_key(kc);
  r.event.pressed = false;
  ime_process_record(kc, &r);
  now += 10;
  ime_matrix_scan();
}

/* ---------------- romaji-ish input language ----------------
 * Lowercase consonants are KC_ letters, aiueo are the layer's vowel keys,
 * 'n' is the ん key. Escapes: _ space, < bspc, ^ enter, ! dakuten,
 * ? handakuten, 0-9 kanji numerals, ~ ー, A I U E O small vowels, T small tsu,
 * = wait out the timeout.
 */
static void type(const char *s, bool kata) {
  for (; *s; s++) {
    switch (*s) {
    case 'a': press(kata ? UC(KTKN_A) : UC(HRGN_A)); break;
    case 'i': press(kata ? UC(KTKN_I) : UC(HRGN_I)); break;
    case 'u': press(kata ? UC(KTKN_U) : UC(HRGN_U)); break;
    case 'e': press(kata ? UC(KTKN_E) : UC(HRGN_E)); break;
    case 'o': press(kata ? UC(KTKN_O) : UC(HRGN_O)); break;
    case 'n': press(kata ? UC(KTKN_N) : UC(HRGN_N)); break;
    case 'A': press(kata ? UC(KTKN_A_SM) : UC(HRGN_A_SM)); break;
    case 'I': press(kata ? UC(KTKN_I_SM) : UC(HRGN_I_SM)); break;
    case 'U': press(kata ? UC(KTKN_U_SM) : UC(HRGN_U_SM)); break;
    case 'E': press(kata ? UC(KTKN_E_SM) : UC(HRGN_E_SM)); break;
    case 'O': press(kata ? UC(KTKN_O_SM) : UC(HRGN_O_SM)); break;
    case 'T': press(kata ? UC(KTKN_TSU_SM) : UC(HRGN_TSU_SM)); break;
    case '_': press(KC_SPC); break;
    case '<': press(KC_BSPC); break;
    case '^': press(KC_ENT); break;
    case '!': press(UC(SYM_DAKUTEN)); break;
    case '?': press(UC(SYM_HANDAKUTEN)); break;
    case '~': press(UC(SYM_LONGVOW)); break;
    case '=': now += TIMEOUT_MS + 1; ime_matrix_scan(); break;
    case '1': press(UC(JP_NUM_1)); break;
    case '2': press(UC(JP_NUM_2)); break;
    case '3': press(UC(JP_NUM_3)); break;
    case '4': press(UC(JP_NUM_4)); break;
    case '5': press(UC(JP_NUM_5)); break;
    case '8': press(UC(JP_NUM_8)); break;
    case '0': press(UC(JP_NUM_10)); break;
    default:  press(KC_A + (*s - 'a')); break;
    }
  }
}

static int fails, total;

static void reset(bool kata) {
  layer_state = 0;
  layer_on(kata ? KATAKANA : HIRAGANA);
  ime_clear();
  mods = 0;
  screen[0] = 0;
}

static void check(const char *keys, const char *want, bool kata) {
  reset(kata);
  type(keys, kata);
  total++;
  if (strcmp(screen, want) != 0) {
    fails++;
    printf("FAIL %s %-12s got \"%s\" want \"%s\"\n", kata ? "kata" : "hira", keys, screen, want);
  }
}
#define H(k, w)       check(k, w, false)
#define K(k, w)       check(k, w, true)

int main(void) {
  // §1 wrong output
  H("gga", "っが");      K("gga", "ッガ");
  K("ga", "ガ");
  K("wwa", "ッワ");      K("wwo", "ッヲ");
  K("dhu", "デュ");      K("thu", "テュ");
  K("tyu", "チュ");      K("dyu", "ヂュ");
  // ん
  H("rennai", "れんあい");   // ん ん confirms, then あ stands alone
  H("rennnai", "れんない");
  H("renai", "れない");
  H("konnnichiha", "こんにちは");
  H("konya", "こにゃ");       // ん y a is にゃ, as in any IME
  H("konnya", "こんや");
  H("nka", "んか");
  H("nn", "ん");  H("nnn", "んん");
  H("n_", "ん ");  H("n^", "ん⏎");  H("n<", "");
  // ts / early commits
  H("tsa", "つぁ");  H("ts_", " ");  H("tsu", "つ");
  H("ttsu", "っつ");  H("ssha", "っしゃ");  H("kkya", "っきゃ");  H("tcha", "っちゃ");
  H("ccha", "っちゃ");  H("rryo", "っりょ");
  // 1e
  H("1e0", "〇");  H("1e1", "十");  H("1e2", "百");  H("1e3", "千");
  H("1e4", "万");  H("1e8", "億");  H("1ew", "兆");
  H("1e5", "一え五");  H("1ea", "一えあ");  H("1eka", "一えか");  H("1e_", "一え ");
  K("1e2", "百");  H("1=e0", "一え十");
  // §2 dead paths now live
  H("dzu", "づ");  K("dzu", "ヅ");  H("dji", "ぢ");  K("dji", "ヂ");
  K("di", "ヂ");  K("du", "ヅ");
  H("caka", "かか");  H("cai", "かい");  H("ca_", "か ");  H("ci", "し");
  // §3 swallowing
  H("kta", "た");  H("k_", " ");  H("k^", "⏎");  H("ka<", "");
  H("k<a", "あ");  H("ky<a", "か");  H("ky<<a", "あ");
  H("k~", "ー");  H("n~", "んー");  H("nT", "んっ");
  H("tu!", "づ");  H("ha?", "ぱ");  H("ha!", "ば");  H("u!", "ゔ");  K("u!", "ヴ");
  H("a!", "あ\xe3\x82\x99");  K("tu!", "ヅ");
  H("yA", "ゃ");  H("yU", "ゅ");  H("yO", "ょ");  H("kA", "ぁ");
  // §4 table
  H("ti", "ち");  K("ti", "チ");  H("tu", "つ");  K("tu", "ツ");
  H("di", "ぢ");  H("du", "づ");
  K("thi", "ティ");  K("twu", "トゥ");  K("dhi", "ディ");  K("dwu", "ドゥ");
  K("tti", "ッチ");  K("ttu", "ッツ");  K("tthi", "ッティ");
  K("ddi", "ッヂ");  K("ddu", "ッヅ");
  H("she", "しぇ");  H("che", "ちぇ");  H("je", "じぇ");
  K("she", "シェ");  K("che", "チェ");  K("je", "ジェ");
  H("fa", "ふぁ");  H("fi", "ふぃ");  H("fe", "ふぇ");  H("fo", "ふぉ");  K("fa", "ファ");
  H("wi", "うぃ");  H("we", "うぇ");  H("wo", "を");  K("wi", "ウィ");  K("wo", "ヲ");  K("who", "ウォ");
  H("ye", "いぇ");  K("ye", "イェ");
  H("jji", "っじ");  H("jja", "っじゃ");
  H("vu", "ゔ");  K("vu", "ヴ");  K("va", "ヴァ");  H("va", "ゔぁ");
  // §5 unreachable
  H("xwa", "ゎ");  K("xwa", "ヮ");  H("xka", "ゕ");  K("xke", "ヶ");
  H("wyi", "ゐ");  K("wye", "ヱ");  H("xtsu", "っ");  H("ltu", "っ");  H("xya", "ゃ");
  H("dya", "ぢゃ");  H("tya", "ちゃ");  H("zya", "じゃ");
  K("ffa", "ッファ");  K("tthi", "ッティ");  H("tsa", "つぁ");
  H("dhi", "でぃ");  H("twu", "とぅ");
  // timeout
  H("k=a", "あ");
  // formerly counter cases; still useful as output checks
  H("kka", "っか");  H("tu!", "づ");  H("nn", "ん");  H("ka~", "かー");

  // Ctrl chord ends a sequence
  reset(false); type("k", false); mods = MOD_MASK_CTRL; press(KC_C); mods = 0; type("a", false);
  total++; if (strcmp(screen, "cあ")) { fails++; printf("FAIL ctrl chord got \"%s\"\n", screen); }

  // y + held SUPP + small ぁ -> ゃ (MO() must not clear)
  reset(false); type("y", false); press(MO(HIRAGANA_SUPP)); type("A", false);
  total++; if (strcmp(screen, "ゃ")) { fails++; printf("FAIL y+SUPP got \"%s\"\n", screen); }

  // prediction
  reset(false);
  total++; if (!ime_accepts(KC_K) || !ime_accepts(UC(HRGN_A)) || ime_has_pending()) { fails++; puts("FAIL predict fresh"); }
  type("k", false);
  total++; if (!ime_has_pending() || !ime_accepts(KC_Y) || !ime_accepts(KC_K) || ime_accepts(KC_T)) { fails++; puts("FAIL predict k"); }
  type("ky", false);  // pending "k" -> "kk"? no: this pushes k,y after k: "kky"
  total++; if (!ime_accepts(UC(HRGN_A)) || !ime_accepts(UC(HRGN_E))) { fails++; puts("FAIL predict kky"); }
  reset(true); type("f", true);
  total++; if (!ime_accepts(UC(KTKN_A)) || !ime_accepts(UC(KTKN_E))) { fails++; puts("FAIL predict f kata"); }
  reset(false); type("dz", false);
  total++; if (!ime_accepts(UC(HRGN_U)) || ime_accepts(UC(HRGN_A))) { fails++; puts("FAIL predict dz"); }
  reset(false); type("1e", false);
  total++; if (!ime_has_pending() || !ime_accepts(UC(JP_NUM_2)) || ime_accepts(UC(JP_NUM_5))) { fails++; puts("FAIL predict 1e"); }
  reset(false); type("n", false);
  total++; if (!ime_accepts(KC_Y) || !ime_accepts(UC(HRGN_N)) || !ime_accepts(UC(HRGN_A))) { fails++; puts("FAIL predict n"); }

  printf("%d/%d passed\n", total - fails, total);
  return fails != 0;
}
