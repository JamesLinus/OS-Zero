#include <stdint.h>
#include <zero/param.h>
#include <kern/mem/slab.h>
#if defined(__i386__) && !defined(__x86_64__) && !defined(__amd64__)
#include <kern/mem/slab32.h>
#include <kern/unit/ia32/link.h>
#include <kern/unit/ia32/vm.h>
#elif defined(__arm__)
#include <kern/unit/arm/link.h>
#endif

extern void pageinit(uintptr_t, unsigned long);

/* TODO: this will _not_ work on 64-bit */
struct slabhdr  virthdrtab[1U << (PTRBITS - SLABMINLOG2)] ALIGNED(PAGESIZE);
struct slabhdr *virtslabtab[PTRBITS] ALIGNED(PAGESIZE);

#if (!MEMTEST)
void
meminit(uintptr_t base, unsigned long nbphys)
{
    pageinit(base, nbphys);
#if defined(__x86_64__) || defined(__amd64__)
#elif (defined(__i386__) && !defined(__x86_64__) && !defined(__amd64__))  \
    || defined(__arm__)
    slabinit(virtslabtab, virthdrtab,
             (unsigned long)&_epagetab, (char *)KERNVIRTBASE - &_epagetab);
#endif

//    swapinit(0, 0x00000000, 1024);

    return;
}
#endif

