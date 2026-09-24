#!/bin/sh
# Host-side tests for jp_ime.c: stubs the QMK API and replays key sequences.
set -e
cd "$(dirname "$0")"
gcc -std=gnu11 -Wall -Wextra -Werror -Wno-unused-parameter \
    -DQMK_KEYBOARD_H='"qmk_stub.h"' -I. \
    -o /tmp/test_ime test_ime.c ../jp_ime.c
/tmp/test_ime
