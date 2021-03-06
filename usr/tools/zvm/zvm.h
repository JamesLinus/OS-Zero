#ifndef __ZVM_ZVM_H__
#define __ZVM_ZVM_H__

#include <stddef.h>
#include <stdint.h>
#include <zero/param.h>
#include <zero/cdecl.h>
#include <zas/zas.h>

/* number of virtual machine integral registers */
#define ZVMNREG         16
#define ZVMNFREG        16
#define ZVMNCREG        4

/* argument types for instructions */
#define ZVMARGIMMED     0x00
#define ZVMARGREG       0x01
#define ZVMARGADR       0x02
/* indexing flags for registers */
#if (ZVMNREG == 16)
#define ZVMREGINDEX      0x40
#define ZVMREGINDIR      0x80
#elif (ZVMNREG == 32)
#define ZVMREGINDEX      0x80
#define ZVMREGINDIR     0x100
#endif
/* mask of valid register IDs */
#define ZVMREGMASK      0x0f

/* number of units and instructions */
#define ZVMNUNIT        16
#define ZVMNOP          256

/* machine status word */
#define ZVMZF           0x01 // zero
#define ZVMOF           0x02 // overflow
#define ZVMCF           0x04 // carry
#define ZVMIF           0x08 // interrupt pending

/* zvm instruction format */
struct zvmopcode {
    unsigned int code  : 8; // (unit << 4) | inst
    unsigned int arg1t : 4; // argument #1 type
    unsigned int arg2t : 4; // argument #2 type
    unsigned int reg1  : 6; // regist�r #1 + ZVMREGINDIR or ZVMREGINDEX
    unsigned int reg2  : 6; // regist�r #2 + ZVMREGINDIR or ZVMREGINDEX
    unsigned int size  : 4; // instruction size
#if (!ZVM32BIT)
    unsigned int pad   : 32;
#endif
    zasword_t    args[EMPTY]; // optional arguments
};

typedef void zvmopfunc_t(struct zvmopcode *);

/* cregs */
#define ZVMMSWCREG 0
#define ZVMPCCREG  1
#define ZVMFPCREG  2
#define ZVMSPCREG  3
/* zvm configuration and virtual machine structure */
struct zvm {
    zasword_t       regs[ZVMNREG]; // virtual registers
    zasword_t       cregs[ZVMNCREG];
#if !(ZVMVIRTMEM)
    char           *physmem; // memory base address
    size_t          memsize; // memory size in bytes
#endif
    volatile long   shutdown; // shutdown flag
#if (ZVMVIRTMEM)
    void          **pagetab;  // virtual memory structure
#endif
} ALIGNED(CLSIZE);

/* external declarations */
extern struct zasop  zvminsttab[ZVMNOP];
extern struct zasop *zvmoptab[ZVMNOP];
extern const char   *zvmopnametab[ZVMNOP];
extern const char   *zvmopnargtab[ZVMNOP];
extern struct zvm    zvm;

/* function prototypes */
int               zvmmain(int argc, char *argv[]);
void              zvminit(void);
void              zvminitopt(void);
size_t            zvminitmem(void);
void              zvminitio(void);
long              asmaddop(const char *str, struct zasop *op);
struct zasop    * zvmfindasm(const char *str);
struct zastoken * zasprocinst(struct zastoken *token, zasmemadr_t adr,
                              zasmemadr_t *retadr);
int8_t            zvmsigbus(zasmemadr_t adr, long size);
int8_t            zvmsigsegv(zasmemadr_t adr, long reason);
void              asmprintop(struct zvmopcode *op);

#endif /* __ZVM_ZVM_H__ */

