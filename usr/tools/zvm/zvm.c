#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#if !(ZVMVIRTMEM) || (ZASDEBUG)
#include <assert.h>
#endif
#if (ZVMXORG)
#include <unistd.h>
#include <zvm/xorg.h>
#elif (ZVMXCB)
#include <unistd.h>
#include <xcb/xcb.h>
#endif
#include <zero/param.h>
#include <zero/cdecl.h>
//#include <zero/trix.h>
#include <zas/zas.h>
#include <zvm/zvm.h>
#include <zvm/op.h>
#include <zvm/mem.h>
#if (ZVMXCB)
#include <zvm/xcb.h>
#endif

extern struct zastoken *zastokenqueue;
extern unsigned long    zasinputread;
extern zasmemadr_t      _startadr;

zvmopfunc_t            *zvmfunctab[ZVMNOP] ALIGNED(PAGESIZE);
struct zvm              zvm;

void
zvminitasmop(uint8_t unit, uint8_t inst, uint8_t *str, uint8_t narg,
             zvmopfunc_t *func)
{
    uint8_t       id = (unit << 4) | inst;
    struct zasop *op = &zvminsttab[id];
    
    op->name = str;
    op->code = id;
    op->narg = narg;
    if (!asmaddop(str, op)) {
        fprintf(stderr, "failed to initialise assembly operation:\n");
        fprintf(stderr, "unit == %d, inst == %d, str == %s, narg = %d\n",
                (int)unit, (int)inst, str, (int)narg);
        
        exit(1);
    }
    zvmfunctab[id] = func;
}

void
zvminitasm(void)
{
    /* logical operations */
    zvminitasmop(ZVMOPLOGIC, ZVMOPNOT, (uint8_t *)"not", 1, zvmopnot);
    zvminitasmop(ZVMOPLOGIC, ZVMOPAND, (uint8_t *)"and", 2, zvmopand);
    zvminitasmop(ZVMOPLOGIC, ZVMOPOR, (uint8_t *)"or", 2, zvmopor);
    zvminitasmop(ZVMOPLOGIC, ZVMOPXOR, (uint8_t *)"xor", 2, zvmopxor);
    /* shift operations */
    zvminitasmop(ZVMOPSHIFT, ZVMOPSHR, (uint8_t *)"shr", 2, zvmopshr);
    zvminitasmop(ZVMOPSHIFT, ZVMOPSAR, (uint8_t *)"sar", 2, zvmopsar);
    zvminitasmop(ZVMOPSHIFT, ZVMOPSHL, (uint8_t *)"shl", 2, zvmopshl);
    zvminitasmop(ZVMOPSHIFT, ZVMOPROR, (uint8_t *)"ror", 2, zvmopror);
    zvminitasmop(ZVMOPSHIFT, ZVMOPROL, (uint8_t *)"rol", 2, zvmoprol);
    /* arithmetic operations */
    zvminitasmop(ZVMOPARITH, ZVMOPINC, (uint8_t *)"inc", 1, zvmopinc);
    zvminitasmop(ZVMOPARITH, ZVMOPDEC, (uint8_t *)"dec", 1, zvmopdec);
    zvminitasmop(ZVMOPARITH, ZVMOPADD, (uint8_t *)"add", 2, zvmopadd);
    zvminitasmop(ZVMOPARITH, ZVMOPSUB, (uint8_t *)"sub", 2, zvmopsub);
    zvminitasmop(ZVMOPARITH, ZVMOPCMP, (uint8_t *)"cmp", 2, zvmopcmp);
    zvminitasmop(ZVMOPARITH, ZVMOPMUL, (uint8_t *)"mul", 2, zvmopmul);
    zvminitasmop(ZVMOPARITH, ZVMOPDIV, (uint8_t *)"div", 2, zvmopdiv);
    zvminitasmop(ZVMOPARITH, ZVMOPMOD, (uint8_t *)"mod", 2, zvmopmod);
    /* branch operations */
    zvminitasmop(ZVMOPBRANCH, ZVMOPJMP, (uint8_t *)"jmp", 1, zvmopjmp);
    zvminitasmop(ZVMOPBRANCH, ZVMOPBZ, (uint8_t *)"bz", 1, zvmopbz);
    zvminitasmop(ZVMOPBRANCH, ZVMOPBNZ, (uint8_t *)"bnz", 1, zvmopbnz);
    zvminitasmop(ZVMOPBRANCH, ZVMOPBLT, (uint8_t *)"blt", 1, zvmopblt);
    zvminitasmop(ZVMOPBRANCH, ZVMOPBLE, (uint8_t *)"ble", 1, zvmopble);
    zvminitasmop(ZVMOPBRANCH, ZVMOPBGT, (uint8_t *)"bgt", 1, zvmopbgt);
    zvminitasmop(ZVMOPBRANCH, ZVMOPBGE, (uint8_t *)"bge", 1, zvmopbge);
    zvminitasmop(ZVMOPBRANCH, ZVMOPBO, (uint8_t *)"bo", 1, zvmopbo);
    zvminitasmop(ZVMOPBRANCH, ZVMOPBNO, (uint8_t *)"bno", 1, zvmopbno);
    zvminitasmop(ZVMOPBRANCH, ZVMOPBC, (uint8_t *)"bc", 1, zvmopbc);
    zvminitasmop(ZVMOPBRANCH, ZVMOPBNC, (uint8_t *)"bnc", 1, zvmopbnc);
    /* stack operations */
    zvminitasmop(ZVMOPSTACK, ZVMOPPOP, (uint8_t *)"pop", 1, zvmoppop);
    zvminitasmop(ZVMOPSTACK, ZVMOPPUSH, (uint8_t *)"push", 1, zvmoppush);
    zvminitasmop(ZVMOPSTACK, ZVMOPPUSHA, (uint8_t *)"pusha", 0, zvmoppush);
    /* load/store */
    zvminitasmop(ZVMOPMOV, ZVMOPMOVL, (uint8_t *)"mov", 2, zvmopmovl);
//    zvminitasmop(ZVMOPMOV, ZVMOPMOVL, (uint8_t *)"movl", 2, zvmopmovl);
    zvminitasmop(ZVMOPMOV, ZVMOPMOVB, (uint8_t *)"movb", 2, zvmopmovb);
    zvminitasmop(ZVMOPMOV, ZVMOPMOVW, (uint8_t *)"movw", 2, zvmopmovw);
#if (!ZAS32BIT)
    zvminitasmop(ZVMOPMOV, ZVMOPMOVQ, (uint8_t *)"movq", 2, zvmopmovq);
#endif
    /* function calls */
    zvminitasmop(ZVMOPFUNC, ZVMOPCALL, (uint8_t *)"call", 1, zvmopcall);
    zvminitasmop(ZVMOPFUNC, ZVMOPENTER, (uint8_t *)"enter", 1, zvmopenter);
    zvminitasmop(ZVMOPFUNC, ZVMOPLEAVE, (uint8_t *)"leave", 1, zvmopleave);
    zvminitasmop(ZVMOPFUNC, ZVMOPRET, (uint8_t *)"ret", 1, zvmopret);
#if 0
    /* thread interface */
    zvminitasmop(ZVMOPFUNC, ZVMOPTHR, (uint8_t *)"thr", 1, zvmopthr);
#endif
    /* machine status word */
    zvminitasmop(ZVMOPMSW, ZVMOPLMSW, (uint8_t *)"lmsw", 1, zvmoplmsw);
    zvminitasmop(ZVMOPMSW, ZVMOPSMSW, (uint8_t *)"smsw", 1, zvmopsmsw);
    /* machine state */
    zvminitasmop(ZVMOPMACH, ZVMOPRESET, (uint8_t *)"reset", 0, zvmopreset);
    zvminitasmop(ZVMOPMACH, ZVMOPHLT, (uint8_t *)"hlt", 0, zvmophlt);
#if 0
    /* I/O operations */
    zvminitasmop(ZVMOPIO, ZVMINB, (uint8_t *)"inb", 2, zvmopinb);
    zvminitasmop(ZVMOPIO, ZVMINW, (uint8_t *)"inw", 2, zvmopinw);
    zvminitasmop(ZVMOPIO, ZVMINL, (uint8_t *)"inl", 2, zvmopinl);
#if (!ZAS32BIT)
    zvminitasmop(ZVMOPIO, ZVMINQ, (uint8_t *)"inq", 2, zvmopinq);
#endif
    zvminitasmop(ZVMOPIO, ZVMOUTB, (uint8_t *)"outb", 2, zvmopoutb);
    zvminitasmop(ZVMOPIO, ZVMOUTW, (uint8_t *)"outw", 2, zvmopoutw);
    zvminitasmop(ZVMOPIO, ZVMOUTL, (uint8_t *)"outl", 2, zvmopoutl);
#if (!ZAS32BIT)
    zvminitasmop(ZVMOPIO, ZVMOUTQ, (uint8_t *)"outq", 2, zvmopoutq);
#endif
#endif /* 0 */
    
    return;
};

void
zvminit(void)
{
    size_t memsize;
    
    memsize = zvminitmem();
    if (!memsize) {
        fprintf(stderr, "zvm: FAILED to allocate machine memory\n");

        exit(1);
    }
    zvminitasm();
#if (!ZVMVIRTMEM)
    zvm.sp = memsize;
#endif
    zvm.pc = ZVMTEXTBASE;
    zvminitio();
#if (ZVMEFL) || (ZVMXORG) || (ZVMXCB)
    zvminitui();
#endif

    return;
}

void *
zvmloop(zasmemadr_t _startadr)
{
    zvmopfunc_t         *func;
    struct zvmopcode    *op;
#if (ZVMTRACE)
    int                  i;
#endif
#if (ZVMDB)
    struct zasline      *line;
#endif
#if (ZVMXCB)
    xcb_void_cookie_t    cookie;
#endif

#if (ZVMTRACE)
    fprintf(stderr, "memory\n");
    fprintf(stderr, "------\n");
    for (i = ZVMTEXTBASE ; i < ZVMTEXTBASE + 256 ; i++) {
        fprintf(stderr, "%02x ", (int8_t)(zvmgetmemt(i, int8_t)) & 0xff);
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "registers\n");
    fprintf(stderr, "---------\n");
    fprintf(stderr, "---------\n");
    for (i = 0 ; i < ZASNREG ; i++) {
        fprintf(stderr, "r%d:\t0x%lx\n", i, (long)zvm.regs[i]);
    }
#endif
    zvm.shutdown = 0;
    zvm.pc = _startadr;
//    memcpy(&zvm.cpustat, cpustat, sizeof(struct zvmcpustate));
//    free(cpustat);
    while (!zvm.shutdown) {
#if (ZVMXCB)
        xcbdoevent();
#endif
//        op = (struct zvmopcode *)&zvm.physmem[zvm.pc];
        op = (struct zvmopcode *)&zvm.physmem[zvm.pc];
        if (op->code == ZVMOPNOP) {
            zvm.pc += sizeof(struct zvmopcode);
        } else {
//            zvm.cpustat.pc = rounduppow2(zvm.pc, sizeof(zasword_t));
//            op = (struct zvmopcode *)&zvm.physmem[zvm.pc];
//            op = &zvm.physmem[zvm.pc];
            func = zvmfunctab[op->code];
#if (ZVMTRACE)
            asmprintop(op);
#endif
            if (func) {
#if (ZVMDB)
                line = zasfindline(zvm.pc);
                if (line) {
                    fprintf(stderr, "%s:%ld:\t%s\n", line->file, line->num, line->data);
                }
#endif
                func(op);
            } else {
                fprintf(stderr, "illegal instruction, PC == %lx\n",
                        (long)zvm.pc);
#if (ZVM)
//                asmprintop(op);
#endif
                
                exit(1);
            }
        }
    }
#if (ZVMTRACE)
    fprintf(stderr, "memory\n");
    fprintf(stderr, "------\n");
    for (i = ZVMTEXTBASE ; i < ZVMTEXTBASE + 256 ; i++) {
        fprintf(stderr, "%02x ", (int8_t)(zvmgetmemt(i, int8_t)) & 0xff);
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "registers\n");
    fprintf(stderr, "---------\n");
    for (i = 0 ; i < ZASNREG ; i++) {
        fprintf(stderr, "r%d:\t0x%lx\n", i, (long)zvm.regs[i]);
    }
#endif
#if (ZVMXORG) || (ZVMXCB)
    while (1) {
        zvmdouievent();
    }
#endif

    return NULL;
}

int
zvmmain(int argc, char *argv[])
{
    long        l;
    zasmemadr_t adr = ZVMTEXTBASE;
#if (ZASPROF)
    PROFDECLCLK(clk);
#endif

    if (argc < 2) {
        fprintf(stderr, "usage: zvm <file1> ...\n");

        exit(1);
    }
    zasinit();
    zvminitopt();
    zvminit();
#if (!ZVMVIRTMEM)
#if (ZVMDEBUG)
    assert(zvm.physmem != NULL);
#endif
    if (ZVMTEXTBASE) {
        memset(zvm.physmem, 0, ZVMTEXTBASE);
    }
#endif
#if (ZASPROF)
    profstartclk(clk);
#endif
    for (l = 1 ; l < argc ; l++) {
#if (ZASBUF)
        zasreadfile(argv[l], adr, readbufcur);
#else
        zasreadfile(argv[l], adr);
#endif
        if (!zastokenqueue) {
            fprintf(stderr, "WARNING: no input in %s\n", argv[l]);
        } else {
            zasinputread = 1;
            adr = zastranslate(adr);
            zasresolve(ZVMTEXTBASE);
            zasremovesyms();
#if (ZASPROF)
            profstopclk(clk);
            fprintf(stderr, "%ld microseconds to process %s\n",
                    profclkdiff(clk), argv[l]);
#endif        
        }
    }
    if (!zasinputread) {
        fprintf(stderr, "empty input\n");

        exit(1);
    }
    fprintf(stderr, "START: %lx\n", (long)_startadr);
    zvmloop(_startadr);
#if (ZVMEFL)
    sleep(5);
#endif

    /* NOTREACHED */
    exit(0);
}

int
main(int argc, char *argv[])
{
    exit(zvmmain(argc, argv));
}

