// Minimal QMK surface for syntax-checking keymap.c on the host (see run.sh).
// Everything here is a stand-in: types and macros only have to be shaped
// like QMK's, not behave like them.
#pragma once
#include "qmk_stub.h"
#include <string.h>

#define MATRIX_ROWS 12
#define MATRIX_COLS 7
#define PROGMEM
#define NO_LED 255

// 72-key Moonlander LAYOUT: any fixed mapping into [12][7] will do here.
#define LAYOUT( \
  k00,k01,k02,k03,k04,k05,k06,  k07,k08,k09,k0a,k0b,k0c,k0d, \
  k10,k11,k12,k13,k14,k15,k16,  k17,k18,k19,k1a,k1b,k1c,k1d, \
  k20,k21,k22,k23,k24,k25,k26,  k27,k28,k29,k2a,k2b,k2c,k2d, \
  k30,k31,k32,k33,k34,k35,          k38,k39,k3a,k3b,k3c,k3d, \
  k40,k41,k42,k43,k44,    k46,  k47,    k49,k4a,k4b,k4c,k4d, \
                  k53,k54,k55,  k58,k59,k5a ) \
{ {k00,k01,k02,k03,k04,k05,k06}, {k10,k11,k12,k13,k14,k15,k16}, \
  {k20,k21,k22,k23,k24,k25,k26}, {k30,k31,k32,k33,k34,k35,KC_NO}, \
  {k40,k41,k42,k43,k44,KC_NO,k46}, {KC_NO,KC_NO,KC_NO,k53,k54,k55,KC_NO}, \
  {k0d,k0c,k0b,k0a,k09,k08,k07}, {k1d,k1c,k1b,k1a,k19,k18,k17}, \
  {k2d,k2c,k2b,k2a,k29,k28,k27}, {k3d,k3c,k3b,k3a,k39,k38,KC_NO}, \
  {k4d,k4c,k4b,k4a,k49,KC_NO,k47}, {KC_NO,KC_NO,KC_NO,k5a,k59,k58,KC_NO} }

enum {
  KC_MINS = 0x2D, KC_EQL, KC_LBRC, KC_RBRC, KC_BSLS, KC_NUHS, KC_SCLN, KC_QUOT, KC_GRV, KC_COMM, KC_DOT,
  KC_CAPS = 0x39, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12,
  KC_INS = 0x49, KC_END = 0x4D, KC_RGHT = 0x4F, KC_LEFT, KC_DOWN, KC_UP,
  KC_PDOT = 0x63, KC_P1 = 0x59, KC_P2, KC_P3, KC_P4, KC_P5, KC_P6, KC_P7, KC_P8, KC_P9, KC_P0,
  KC_LALT = 0xE2, KC_LGUI = 0xE3,
  KC_COPY = 0x7C, KC_PSTE = 0x7D,
  QK_GESC = 0x7C16, UC_NEXT = 0x7E30, UC_PREV,
};
#define LGUI(kc) (0x0800 | (kc))
#define QK_UNICODE_MAX 0xFFFF

typedef struct { uint8_t matrix_co[MATRIX_ROWS][MATRIX_COLS]; } led_config_t;
extern led_config_t g_led_config;

#define HSV_OFF 0, 0, 0
#define RGB_OFF 0x00, 0x00, 0x00
#define RGB_MATRIX_SOLID_COLOR 1
void rgb_matrix_mode_noeeprom(uint8_t mode);
void rgb_matrix_sethsv_noeeprom(uint8_t h, uint8_t s, uint8_t v);
void rgb_matrix_set_color(int index, uint8_t r, uint8_t g, uint8_t b);

uint16_t timer_elapsed(uint16_t last);
void register_code(uint8_t kc);
void unregister_code(uint8_t kc);
#define SEND_STRING(s) send_string(s)
void send_string(const char *s);
#define SS_TAP(k)  "\1"
#define SS_LCTL(s) "\2" s
#define SS_LSFT(s) "\3" s
#define X_LEFT 0

uint8_t  layer_switch_get_layer(keypos_t key);
uint16_t keymap_key_to_keycode(uint8_t layer, keypos_t key);
#define pgm_read_word(p) (*(p))
