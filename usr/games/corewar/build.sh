#! /bin/sh

gcc -I../../lib -I.. -O0 -Wall -g -o cw cw.c rc.c
gcc -DCWRANDMT32=1 -DZEUS=1 -I../../lib -I.. -O0 -Wall -g -o zeus cw.c rc.c zeus.c x11.c ../../lib/c/malloc.c ../../lib/zero/randmt32.c -lX11 -pthread

