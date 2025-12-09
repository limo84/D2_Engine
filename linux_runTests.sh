#!/bin/sh

gcc -g -o test.out test.c src/*.c vendor/*.c -I src -I vendor/ -lSDL2 -lSDL2_mixer -lm \
&& ./test.out