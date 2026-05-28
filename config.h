/* Copyright 2020 ZSA Technology Labs, Inc <@zsa>
 * Copyright 2020 Jack Humbert <jack.humb@gmail.com>
 * Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
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

#pragma once

#define RGB_MATRIX_KEYPRESSES

#define ORYX_CONFIGURATOR

#define UNICODE_SELECTED_MODES UNICODE_MODE_LINUX, UNICODE_MODE_WINDOWS

// Hiragana vowels
#define HRGN_A 0x3042
#define HRGN_E 0x3048
#define HRGN_I 0x3044
#define HRGN_O 0x304A
#define HRGN_U 0x3046
#define HRGN_N 0x3093

// Hiragana small vowels + small tsu
#define HRGN_A_SM   0x3041
#define HRGN_E_SM   0x3047
#define HRGN_I_SM   0x3043
#define HRGN_O_SM   0x3049
#define HRGN_U_SM   0x3045
#define HRGN_TSU_SM 0x3063

// Katakana vowels
#define KTKN_A 0x30A2
#define KTKN_E 0x30A8
#define KTKN_I 0x30A4
#define KTKN_O 0x30AA
#define KTKN_U 0x30A6
#define KTKN_N 0x30F3

// Katakana small vowels + small tsu
#define KTKN_A_SM   0x30A1
#define KTKN_E_SM   0x30A7
#define KTKN_I_SM   0x30A3
#define KTKN_O_SM   0x30A9
#define KTKN_U_SM   0x30A5
#define KTKN_TSU_SM 0x30C3

// Japanese numerals (kanji)
#define JP_NUM_1  0x4E00
#define JP_NUM_2  0x4E8C
#define JP_NUM_3  0x4E09
#define JP_NUM_4  0x56DB
#define JP_NUM_5  0x4E94
#define JP_NUM_6  0x516D
#define JP_NUM_7  0x4E03
#define JP_NUM_8  0x516B
#define JP_NUM_9  0x4E5D
#define JP_NUM_10 0x5341

// Symbols
#define SYM_TILDE      0x301C
#define SYM_BANG       0x0021
#define SYM_INTERRO    0x003F
#define SYM_AT         0x0040
#define SYM_HASH       0x0023
#define SYM_YEN        0x00A5
#define SYM_KAKKO1     0x300C
#define SYM_KAKKO2     0x300D
#define SYM_KAKKO3     0x3008
#define SYM_KAKKO4     0x3009
#define SYM_DAKUTEN    0x3099
#define SYM_HANDAKUTEN 0x309A
#define SYM_COMMA      0x3001
#define SYM_PERIOD     0x3002
#define SYM_LONGVOW    0x30FC
