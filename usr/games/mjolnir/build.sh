#! /bin/sh

CC=gcc

$CC -Wextra -Wundef -Wall -D_REENTRANT=1 -O0 -DMJOLHACKS=1 -DMJOLDEBUG=1 -DTEST=0 -Wall -g -I.. -I../../lib -o mjolnir *.c ../../lib/zero/randmt32.c ../../lib/zero/hash.c -lncurses

