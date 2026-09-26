Multilingual Moonlander Setup
=============================

A ZSA Moonlander keymap with a firmware romaji→kana IME: type hiragana or
katakana straight from the keyboard, with no OS input method.

Default language: ENG-US (QWERTY).

## Layers

| layer         | how to reach it                                          |
|---------------|----------------------------------------------------------|
| QWERTY        | default; `ENG_GO` (bottom row, right) returns here        |
| HIRAGANA      | `HRGA_GO` (bottom row, right; also on GUIS)               |
| KATAKANA      | `KTKN_GO` (bottom row, right; also on GUIS)               |
| HIRAGANA_SUPP | hold the left Shift position while in hiragana            |
| KATAKANA_SUPP | hold the left Shift position while in katakana            |
| FUNCS         | hold `MO(FUNCS)`, bottom row left: F-keys, ESDF arrows, numpad |
| GUIS          | hold `MO(GUIS)`, left home-row outer key or bottom row: GUI+key window-manager chords, plus `` ` [ ] \ `` |

**SUPP** (the two `_SUPP` layers) holds small kana, brackets and symbols. On
SUPP, `HRGA_GO`/`KTKN_GO` become `UC_PREV`/`UC_NEXT` to cycle the Unicode
input mode (Linux / Windows).

QWERTY-only keys: the right outer column has `()` and `[]` (typed as a pair,
cursor left), the fans are GUI+Del / GUI+Ins (hold for Shift+GUI+…), and the
thumbs carry Copy, Ctrl+Shift+C, Ctrl+Shift+V and Paste.

## Kana layer map

Everything not listed is the same key as QWERTY.

- Number row: kanji numerals 一 … 九 十
- A E I O U: あ い う え お; N: ん; Q: nothing
- `;` → ゛ (dakuten); `'` position → ゜ (handakuten)
- `,` `.` → 、 。; `/` stays `/`
- `-` → ー; `=`, the fans and right Shift are off
- Right outer column, rows 1–2 → 「 」 (where QWERTY has `()` `[]`)
- Left Shift position → SUPP (hold)

## Typing kana

Vowels (あ い う え お) and ん are their own keys and type immediately.
Consonants wait for a vowel; the LEDs show which keys continue the pending
sequence (bright) or can start one (dim). Romaji follows the usual Google /
Microsoft IME rules, the same in both layers:

- `ka` → か, `kya` → きゃ, `shi`/`si` → し, `chi`/`ti` → ち, `tsu`/`tu` → つ,
  `ji`/`zi` → じ, `fu`/`hu` → ふ, `di` → ぢ, `du` → づ (`dzu`, `dji` also work)
- doubled consonant → small っ: `kka` → っか, `ssha` → っしゃ, `tcha` → っちゃ
- loanword forms: `thi` → ティ, `dhi` → ディ, `thu` → テュ, `dhu` → デュ,
  `twu` → トゥ, `dwu` → ドゥ, `fa` → ファ, `she` → シェ, `che` → チェ,
  `je` → ジェ, `wi` → ウィ, `we` → ウェ, `who` → ウォ, `ye` → イェ, `va` → ヴァ
- `wo` → を / ヲ, `wyi` → ゐ, `wye` → ゑ
- small kana with the X or L prefix: `xa` → ぁ, `xya` → ゃ, `xtu` → っ,
  `xwa` → ゎ, `xka` → ゕ, `xke` → ゖ (or hold SUPP; `y` + SUPP-ぁ → ゃ)

**ん** types immediately. A following vowel or `y` turns it into the な row
(ん あ → な, ん y あ → にゃ). Press ん twice to confirm it and keep it:
ん ん あ → んあ, so こんな is こ ん ん ん あ.

**Backspace** while a consonant is pending deletes the pending letter, not
the text on screen. Any other key (Space, Enter, punctuation…) drops pending
consonants and types normally. Pending consonants also expire after 3 s.

**Dakuten / handakuten** (`;` and the `'` position; ゜ is also SUPP + `;`)
voice the kana you just typed: つ ゛ → づ, は ゜ → ぱ, う ゛ → ゔ. After a kana with no
voiced form they type the combining mark.

## Numbers

- The top row types kanji numerals 一 … 九 十.
- Place values use `1 e _`, pressed in sequence: `1e1` → 十, `1e2` → 百,
  `1e3` → 千, `1e4` → 万, `1e8` → 億, and `1ew` → 兆 (10¹² needs two keys).
- The one exception is `1e0` → 〇 (十 is far more common than 〇, so 十 keeps
  the 0 key and 〇 gets the otherwise meaningless 1e0).
- Any other key after 一え leaves 一え as typed.

## Symbols (hold SUPP)

- 〜 on Esc, ！ ＠ ＃ ¥ on 1–4, 「 」 on 9 / 0 (「 」 are also on the right
  outer column without SUPP)
- 〈 〉 on 、 / 。, ？ on /
- small ぁ ぃ ぅ ぇ ぉ っ on the vowel keys and T

## Lights

- Keys pressed since the last Space, Enter or Esc glow blue. Punctuation and
  layer switches restart the trail.
- On a kana layer, keys that would start a kana are dim orange; while romaji
  is pending, only the keys that continue or complete it are bright orange.
  This comes from the IME's own table, so it can't disagree with the output.

## Tests

`test/run.sh` compiles `jp_ime.c` against a small QMK stub on the host and
replays key sequences, then syntax-checks `keymap.c` against a stub of the
QMK surface it uses (needs only gcc).
