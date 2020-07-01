#!/bin/sh

mkdir -p qmk/keyboards/planck/keymaps/rcw

ln -sf $(pwd)/planck/config.h qmk/keyboards/planck/keymaps/rcw/
ln -sf $(pwd)/planck/keymap.c qmk/keyboards/planck/keymaps/rcw/
ln -sf $(pwd)/planck/rules.mk qmk/keyboards/planck/keymaps/rcw/

mkdir -p qmk/keyboards/crkbd/keymaps/rcw

ln -sf $(pwd)/crkbd/config.h qmk/keyboards/crkbd/keymaps/rcw/
ln -sf $(pwd)/crkbd/keymap.c qmk/keyboards/crkbd/keymaps/rcw/
ln -sf $(pwd)/crkbd/rules.mk qmk/keyboards/crkbd/keymaps/rcw/
