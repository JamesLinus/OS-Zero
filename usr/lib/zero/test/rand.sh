#! /bin/sh

gcc -fno-builtin -DRANDMT32TEST=1 -DRANDMT32PROF=1 -g -I.. -I../.. -O -o rand ../randmt32.c ../randk.c ../randlfg.c ../randlfg2.c
gcc -g -O -o test mt19937ar.c
gcc -DRANDMT64TEST=1 -g -Wall -O0 -I../.. -o randmt64 ../randmt64.c
