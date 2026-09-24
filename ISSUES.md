# IME issues — resolution log

The audit of commit `9cc027e` (romaji matcher in `jp_ime.c`, keymap in
`keymap.c`) found the issues below. Everything has now been addressed.
`jp_ime.c` was rewritten from ~2,500 lines of per-series switch blocks into
one romaji table plus a small matcher. The same table drives both layers and
the RGB predictor. `test/run.sh` replays 150 key sequences covering every
item here.

Conventions chosen:
- Standard IME romaji in both layers (`ti` → ち/チ); loanword forms come from
  `th`/`dh`/`tw`/`dw`.
- Pressing ん a second time confirms the first one.
- SUPP symbols are fullwidth.

## Still open

- **Not built on hardware.** No QMK tree is installed on this machine.
  `jp_ime.c` is tested on the host against a stub, and `keymap.c` is only
  syntax-checked against a generated stub. Run `qmk compile` and flash
  before trusting it. These QMK names are assumed rather than verified:
  `QK_MOMENTARY`/`QK_MOMENTARY_MAX`, `QK_UNICODE`, `UC_PREV`,
  `layer_state_is`, `get_highest_layer`.

## 1. Wrong output — fixed

| input | was | now |
|---|---|---|
| `gga` | っぎ / ッギ | っが / ッガ |
| katakana `ga` | が | ガ |
| katakana `wwa` / `wwo` | ッわ / ッを | ッワ / ッヲ |
| katakana `dy…` | ドュ (commits on y) | `dyu` → ヂュ; デュ is `dhu` |
| katakana `ty…` | テュ (commits on y) | `tyu` → チュ; テュ is `thu` |
| れんあい | れない | ん ん あ → んあ (ん ん confirms) |
| `nna` | っな | ん ん あ → んあ; んな is ん ん ん あ |
| `tsa`, `ts`+Space | っ | つぁ; Space just drops the pending `ts` |
| `ttsu`, `ssha` | っつう, っしあ | っつ, っしゃ (any 3-consonant run: `kkya`, `tcha`, `rryo`) |
| 一 え + unmapped key | deleted 一え | 一え stays, and the key types normally |

The `1e_` place values are kept as documented in the README (`1e1` → 十 and
`1e0` → 〇 are intentional). `1ew` → 兆 is now documented too.

## 2. Dead code — removed or made live

- `dzu` → づ and `dji` → ぢ (ヅ/ヂ in katakana) now work. The old DZ_/DJ_
  branches were shadowed by the Z and J series.
- `c` is a real series: `ca`/`cu`/`co` → か/く/こ, `ci`/`ce` → し/せ,
  `ch*`, `cy*`. A lone `c` no longer poisons the next key.
- Removed: `KC_N` cases, the redundant pressed check, the duplicated
  hiragana/katakana branches, and the per-branch `unregister_code(UC(...))`
  calls.
- Correction to the audit: the `unregister_code` calls in the old final
  consonant switch were not no-ops. They ran on key release, and they released
  a consonant that had gone out as a Ctrl chord. The new code gets the same
  effect by passing every release through.
- `keymap.c`: `case KC_ESC` changed to `QK_GESC`, so Esc now resets the
  counter.

## 3. Swallowed keys — fixed

- ん is never followed by an eaten key. Space, Enter, Bspc, ー, small kana and
  numerals all type normally, and Bspc deletes the ん.
- A stray consonant no longer eats the next key: `kta` → た.
- A key that can't continue the pending romaji drops it and types normally.
  This now applies the same way to Space, Enter, 、 and 。.
- Backspace with romaji pending removes the last pending letter.
- `CT_SF_C`/`CT_SF_V`, `PAREN_IN`, `SBRACK_IN`, `GUI_DEL`/`GUI_INS` and the
  new fullwidth keys call `ime_clear()`.
- Holding SUPP (`MO()`) no longer clears pending romaji, so `y` + SUPP-ぁ → ゃ
  works. It was previously unreachable because pressing SUPP cleared the `y`.
- Dakuten/handakuten after a kana with a voiced form replace it with the
  precomposed character (つ゛ → づ, は゜ → ぱ, う゛ → ゔ). Otherwise they type
  the combining mark as before.
- `！ ？ ＠ ＃` are fullwidth custom keycodes: `FW_EXLM`, `FW_QUES`,
  `FW_AT`, `FW_HASH`. `UC()` can't encode anything above U+7FFF.

## 4. Hiragana ↔ katakana — consistent

Both layers use one table; katakana output is the hiragana output shifted by
+0x60. `ti`/`tu`/`di`/`du` → チ/ツ/ヂ/ヅ, `tti` → ッチ, `wo` → ヲ.
`she`/`che`/`je`/`fa`/`wi`/`we`/`ye` work in hiragana, and V works in
hiragana too (ゔ).

## 5. Unreachable kana — reachable

- ヲ, ヂ and ヅ are now typeable.
- ゔ, ゎ/ヮ, ゕゖ/ヵヶ and ゐゑ/ヰヱ come from `vu`, `xwa`, `xka`/`xke`,
  `wyi`/`wye`.
- The ぢゃ row, `tya`, `zya`, っ+yoon, っファ, っティ, the つぁ family,
  てぃ/でぃ/とぅ/どぅ, the ふぁ family, しぇ/ちぇ/じぇ and いぇ/うぃ/うぇ/うぉ
  all have direct romaji now.
- X and L are on both kana layers as small-kana prefixes.
- 〇 is still only `1e0`, by design.

## 6. RGB predictor — derived from the table

`compute_predict` and its hand-maintained NEXT_* rules are gone. The LEDs ask
`ime_accepts(kc)`, which runs the matcher's own table. Keys are looked up
through KC_TRNS, so predictions stay on while SUPP is held.

## 7. Kana counter — fixed

- When ん turns into a な-row kana, the ん is subtracted, so な counts 1 and
  にゃ counts 2.
- 1e subtracts the え.
- Combining marks aren't counted, and precomposed voicing doesn't change the
  count.
- Bspc decrements.
- Space and Enter are never eaten any more, so the reset in `keymap.c`
  matches what the host sees.

## 8. Keymap / docs — fixed

- The katakana right-inner column now has Bspc/Enter, like hiragana.
- The HIRAGANA diagram and the header comment are corrected.
- The SUPP layers match: explicit ー, and `UC_PREV`/`UC_NEXT` instead of two
  `UC_NEXT` keys.
- README rewritten for the Moonlander and the new romaji rules.
