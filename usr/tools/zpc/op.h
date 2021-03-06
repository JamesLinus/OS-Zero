#ifndef __ZPC_OP_H__
#define __ZPC_OP_H__

#define ZPCOPNOP  0             // no operation
#define ZPCOPNOT  1             // 2's complement; reverse all bits
#define ZPCOPAND  2             // logical AND
#define ZPCOPOR   3             // logical OR
#define ZPCOPXOR  4             // logical XOR
#define ZPCOPSHL  5             // shift left
#define ZPCOPSHR  6             // logical shift right
#define ZPCOPSAR  7             // arithmetic shift right
#define ZPCOPROL  8             // rotate left
#define ZPCOPROR  9             // rotate right
#define ZPCOPINC  10            // increment by one
#define ZPCOPDEC  11            // decrement by one
#define ZPCOPADD  12            // addition
#define ZPCOPSUB  13            // subtraction
#define ZPCOPCMP  14            // compare
#define ZPCOPMUL  15            // multiplication
#define ZPCOPDIV  16            // division
#define ZPCOPMOD  17            // modulus
#define ZPCOPLEA  18            // load effective address
#define ZPCOPLDA  19            // load accumulator (register)
#define ZPCOPSTA  20            // store accumulator (register)
#define ZPCOPPSH  21            // push to stack
#define ZPCOPPOP  22            // pop from stack
#define ZPCOPJMP  23            // jump (branch unconditionally)
#define ZPCOPBE   24            // branch if equal
#define ZPCOPBZ   ZPCOPBE       // branch if zero
#define ZPCOPBNE  25            // branch if not zero
#define ZPCOPBNZ  ZPCOPBNE      // branch if equal
#define ZPCOPBLT  25            // branch if less than
#define ZPCOPBLE  26            // branch if less than or equal
#define ZPCOPBGT  27            // branch if greater than
#define ZPCOPBGE  28            // branch if greater than or equal
#define ZPCOPCALL 29            // call subroutine
#define ZPCOPRET  30            // return from subroutine
#define ZPCOPIN   31            // read from I/O port
#define ZPCOPOUT  32            // write to I/O port
/* privileged mode instructions */
#define ZPCOPLDB  62            // set or clear bit in machine register
#define ZPCOPLDR  63            // load machine-register

#define zpcsetzf(zpc, val)                                              \
    (!(val)                                                             \
     ? ((zpc)->mregs[ZPCMREGSW] |= ZPCMSWZF, 1)                         \
     : ((zpc)->mregs[ZPCMREGSW] &= ~ZPCMSWZF), 0)

#endif /* __ZPC_OP_H__ */

