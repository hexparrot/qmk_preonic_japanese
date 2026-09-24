Multilingual Moonlander Setup
=============================

A ZSA Moonlander keymap with a firmware romaji→kana IME: type hiragana or
katakana straight from the keyboard, with no OS input method.

Default language: ENG-US (QWERTY).

## Switching layers

Bottom row, right hand (HRGA / KTKN are also under MO(6)):

| key      | layer                  |
|----------|------------------------|
| HRGA     | Hiragana               |
| KTKN     | Katakana               |
| ENG_GO   | back to QWERTY         |

On the kana layers the left Shift position is **SUPP** (hold): small kana,
brackets and symbols. On SUPP, HRGA/KTKN become UC_PREV/UC_NEXT to cycle the
Unicode input mode (Linux / Windows).

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

**Dakuten / handakuten** (the key right of L, and SUPP + that key) voice the
kana you just typed: つ ゛ → づ, は ゜ → ぱ, う ゛ → ゔ. After a kana with no
voiced form they type the combining mark.

## Numbers

- The top row types kanji numerals 一 … 九 十.
- Place values use `1 e _`, pressed in sequence: `1e1` → 十, `1e2` → 百,
  `1e3` → 千, `1e4` → 万, `1e8` → 億, and `1ew` → 兆 (10¹² needs two keys).
- The one exception is `1e0` → 〇 (十 is far more common than 〇, so 十 keeps
  the 0 key and 〇 gets the otherwise meaningless 1e0).
- Any other key after 一え leaves 一え as typed.

## Symbols (hold SUPP)

- 〜 ！ ＠ ＃ ¥ on the number row, 「 」 on 9 / 0
- 〈 〉 on 、 / 。, ？ on /
- small ぁ ぃ ぅ ぇ ぉ っ on the vowel keys and T

## Counter

While on a kana layer, the number-row LEDs show how many kana are in the
current word (red: tens / single digit, blue: ones). Space, Enter, Esc,
punctuation and layer switches reset it.

## Tests

`test/run.sh` compiles `jp_ime.c` against a small QMK stub on the host and
replays key sequences (needs only gcc).
