#!/bin/sh
# Host-side tests: stubs the QMK API, replays key sequences through jp_ime.c,
# and syntax-checks keymap.c against a stub of the QMK surface it uses.
set -e
cd "$(dirname "$0")"
gcc -std=gnu11 -Wall -Wextra -Werror -Wno-unused-parameter \
    -DQMK_KEYBOARD_H='"qmk_stub.h"' -I. \
    -o /tmp/test_ime test_ime.c ../jp_ime.c
/tmp/test_ime
gcc -std=gnu11 -Wall -Wextra -Werror -Wno-unused-parameter -fsyntax-only \
    -DQMK_KEYBOARD_H='"keymap_stub.h"' -I. ../keymap.c
echo "keymap.c: syntax ok"
