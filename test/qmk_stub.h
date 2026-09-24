// Minimal QMK surface for compiling jp_ime.c on the host (see test/run.sh).
#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "../config.h"

enum {
  KC_NO = 0x00, KC_TRNS = 0x01,
  KC_A = 0x04, KC_B, KC_C, KC_D, KC_E, KC_F, KC_G, KC_H, KC_I, KC_J, KC_K, KC_L, KC_M,
  KC_N, KC_O, KC_P, KC_Q, KC_R, KC_S, KC_T, KC_U, KC_V, KC_W, KC_X, KC_Y, KC_Z,
  KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0,
  KC_ENT = 0x28, KC_ESC, KC_BSPC, KC_TAB, KC_SPC,
  KC_SLSH = 0x38,
  KC_DEL = 0x4C,
  KC_LCTL = 0xE0, KC_LSFT = 0xE1, KC_RSFT = 0xE5,
};

#define QK_ONE_SHOT_MOD     0x52A0
#define QK_ONE_SHOT_MOD_MAX 0x52BF
#define QK_MOMENTARY        0x5220
#define QK_MOMENTARY_MAX    0x523F
#define MO(l)               (QK_MOMENTARY | (l))
#define QK_UNICODE          0x8000
#define UC(c)               (QK_UNICODE | (c))
#define SAFE_RANGE          0x7E40

#define MOD_BIT_LCTRL  0x01
#define MOD_MASK_CTRL  0x11
#define MOD_MASK_SHIFT 0x22

typedef struct { uint8_t row, col; } keypos_t;
typedef struct { keypos_t key; bool pressed; uint16_t time; } keyevent_t;
typedef struct { keyevent_t event; } keyrecord_t;

extern uint32_t layer_state;
#define IS_LAYER_ON(l) ((layer_state >> (l)) & 1)
void layer_clear(void);
void layer_on(uint8_t l);

uint8_t get_mods(void);
uint8_t get_oneshot_mods(void);

uint16_t timer_read(void);
bool     timer_expired(uint16_t now, uint16_t deadline);

void tap_code(uint8_t kc);
void send_unicode_string(const char *s);
