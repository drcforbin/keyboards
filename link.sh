#!/bin/sh

mkdir -p qmk/keyboards/planck/keymaps/rcw

ln -sf $(pwd)/planck/config.h qmk/keyboards/planck/keymaps/rcw/
ln -sf $(pwd)/planck/keymap.c qmk/keyboards/planck/keymaps/rcw/
ln -sf $(pwd)/planck/rules.mk qmk/keyboards/planck/keymaps/rcw/
