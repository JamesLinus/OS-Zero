#! /bin/sh

CC=gcc

$CC -DZERONEWMTX -Wextra -Wundef -rdynamic -DGNUTRACE=1 -DMALLOCDEBUG=1 -DX11VIS=0 -DMALLOCMULTITAB=1 -DMALLOCSTAT=0 -DZEROHASH=1 -DZEROFUTEX=0 -DZEROMALLOC=1 -DPTHREAD=1 -D_REENTRANT -g -Wall -fno-builtin -O -I. -I../../../.. -I../../../lib -o zheap2 heap.c ../malloc2.c ../../zero/hash.c -pthread
$CC -DZERONEWMTX -Wextra -Wundef -rdynamic -DGNUTRACE=1 -DMALLOCSTAT=0 -D_ZERO_SOURCE=1 -DMALLOCDEBUG=1 -DX11VIS=0 -DMALLOCMULTITAB=1 -DZEROHASH=1 -DZEROFUTEX=0 -DZEROMALLOC=1 -DPTHREAD=1 -D_REENTRANT -DDEVEL=0 -g -Wall -fno-builtin -O -I. -I/usr/include -I../../../.. -I../../../lib -fPIC -shared -o zmalloc2.so ../malloc2.c ../../zero/hash.c -pthread


