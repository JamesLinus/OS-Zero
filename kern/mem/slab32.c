#include <stddef.h>
#include <zero/trix.h>
#include <kern/util.h>
#include <kern/mem/slab.h>
#if defined(__i386__) && !defined(__x86_64__) && !defined(__amd64__) && !defined(__x86_64__)
#include <kern/unit/ia32/vm.h>
#elif defined(__arm__)
#include <kern/unit/arm/vm.h>
#endif
#if (MEMTEST)
#include <stdio.h>
#endif

extern unsigned long npagefree;
extern long          virtlktab[PTRBITS];

/*
 * zero slab allocator
 *
 * slabs are power-of-two-sizes.
 * slabs are combined to and split from bigger ones on demand; free regions are
 * kept as big as possible.
 */

void
slabinit(struct slabhdr **zone, struct slabhdr *hdrtab,
         unsigned long base, unsigned long nb)
{
    unsigned long   adr = (base & (SLABMIN - 1))
        ? roundup2(base, SLABMIN)
        : base;
    unsigned long   bkt = PTRBITS - 1;
    unsigned long   ul = 1UL << bkt;
    struct slabhdr *hdr;

    if (base != adr) {
        nb -= adr - base;
        nb = rounddown2(nb, SLABMINLOG2);
    }
    kprintf("%ul kilobytes kernel virtual memory free @ %lx\n", nb >> 10, adr);
#if (MEMTEST)
    printf("VM: %lu bytes @ %lu\n", nb, adr);
#endif
    while ((nb) && bkt >= SLABMINLOG2) {
        if (nb & ul) {
            kprintf("%lx bytes @ %lx - %lx\n", 1L << bkt, adr, adr + ul - 1);
            hdr = &hdrtab[slabnum(adr)];
            slabsetbkt(hdr, bkt);
            slabsetfree(hdr);
            slabclrlink(hdr);
            zone[bkt] = hdr;
            nb -= 1UL << bkt;
            adr += 1UL << bkt;
        }
        bkt--;
        ul >>= 1;
    }

    return;
}

/*
 * combine slab with bigger [previous- or next-address] ones if possible.
 */
long
slabcomb(struct slabhdr **zone, struct slabhdr *hdrtab, struct slabhdr *hdr)
{
    unsigned long   bkt1 = slabgetbkt(hdr);
    unsigned long   bkt2 = bkt1;
    long            ret  = 0;
    long            prev = 1;
    long            next = 1;
    struct slabhdr *hdr1 = NULL;
    struct slabhdr *hdr2;
    struct slabhdr *hdr3;
    struct slabhdr *hdr4;
    unsigned long   ofs;

    while ((prev) || (next)) {
#if 0
        prev = 0;
        next = 0;
#endif
        ofs = 1UL << (bkt2 - SLABMINLOG2);
        if (hdr - hdrtab >= ofs) {
            hdr1 = hdr - ofs;
            bkt2 = slabgetbkt(hdr1);
#if (MEMTEST)
            fprintf(stderr, "PREV: %x (%x/%x) - %s - ",
                    slabadr(hdr1, hdrtab),
                    slabadr(hdr, hdrtab) - slabadr(hdr1, hdrtab),
                    1UL << bkt2,
                    slabisfree(hdr1) ? "FREE - " : "USED - ");
#endif
            if (bkt2) {
                slablk(bkt2);
                if (bkt2 == bkt1 && slabisfree(hdr1)) {
#if (MEMTEST)
                    fprintf(stderr, "MATCH\n");
#endif
                    ret++;
                    hdr3 = slabgetprev(hdr1, hdrtab);
                    hdr4 = slabgetnext(hdr1, hdrtab);
                    if (hdr3) {
                        if (hdr4) {
                            slabsetprev(hdr4, hdr3, hdrtab);
                            slabsetnext(hdr3, hdr4, hdrtab);
                        } else {
                            slabclrnext(hdr3);
                        }
                    } else {
                        if (hdr4) {
                            slabclrlink(hdr4);
                            slabsetnext(hdr4, zone[bkt2], hdrtab);
                        }
                        if (zone[bkt2]) {
                            slabsetprev(zone[bkt2], hdr4, hdrtab);
                        }
                        zone[bkt2] = hdr4;
                    }
                    slabunlk(bkt2);
                    bkt2++;
                    slabsetbkt(hdr1, bkt2);
                    bkt1 = bkt2;
                    hdr = hdr1;
                } else {
#if (MEMTEST)
                    fprintf(stderr, "NO MATCH\n");
#endif
                    slabunlk(bkt2);
                    prev = 0;
                }
            }
#if 0
            if (!prev) {
                hdr1 = hdr;
            }
#endif
        } else {
            prev = 0;
        }
        hdr1 = hdr;
        if (hdr1 + ofs < hdrtab + SLABNHDR) {
            hdr2 = hdr1 + ofs;
#if (MEMTEST)
            fprintf(stderr, "NEXT: %x (%x/%x) - %s - ",
                    slabadr(hdr2, hdrtab),
                    slabadr(hdr2, hdrtab) - slabadr(hdr1, hdrtab),
                    1UL << bkt2,
                    slabisfree(hdr2) ? "FREE - " : "USED - ");
#endif
            if (bkt2) {
                bkt2 = slabgetbkt(hdr2);
                slablk(bkt2);
                if (bkt2 == bkt1 && slabisfree(hdr2)) {
#if (MEMTEST)
                    fprintf(stderr, "MATCH\n");
#endif
                    ret++;
                    hdr3 = slabgetprev(hdr2, hdrtab);
                    hdr4 = slabgetnext(hdr2, hdrtab);
                    if (hdr3) {
                        if (hdr4) {
                            slabsetprev(hdr4, hdr3, hdrtab);
                            slabsetnext(hdr3, hdr4, hdrtab);
                        } else {
                            slabclrnext(hdr3);
                        }
                    } else {
                        if (hdr4) {
                            slabclrlink(hdr4);
                            slabsetnext(hdr4, zone[bkt2], hdrtab);
                        }
                        if (zone[bkt2]) {
                            slabsetprev(zone[bkt2], hdr4, hdrtab);
                        }
                        zone[bkt2] = hdr4;
                    }
                    slabunlk(bkt2);
                    bkt2++;
                    slabsetbkt(hdr1, bkt2);
                    bkt1 = bkt2;
                } else {
#if (MEMTEST)
                    fprintf(stderr, "NO MATCH\n");
#endif
                    slabunlk(bkt2);
                    next = 0;
                }
            }
        } else {
            next = 0;
        }
        hdr = hdr1;
    }
    if (ret) {
        slabsetbkt(hdr1, bkt1);
        slabclrfree(hdr1);
        slabclrlink(hdr1);
        if (zone[bkt1]) {
            slabsetprev(zone[bkt1], hdr1, hdrtab);
            slabsetnext(hdr1, zone[bkt1], hdrtab);
        }
        zone[bkt1] = hdr1;
    }
                
    return ret;
}

/*
 * split slab into smaller ones to satisfy allocation request.
 * split of N to M gives us one free slab in each of M to N-1 and one to
 * allocate in M.
 */    
void
slabsplit(struct slabhdr **zone, struct slabhdr *hdrtab,
          struct slabhdr *hdr, unsigned long dest)
{
    unsigned long   bkt = slabgetbkt(hdr);
    uint8_t        *ptr = slabadr(hdr, hdrtab);
    struct slabhdr *hdr1;
    unsigned long   sz = 1UL << bkt;
    
    hdr1 = slabgetnext(hdr, hdrtab);
    if (hdr1) {
        slabclrprev(hdr1);
    }
    zone[bkt] = hdr1;
    while (--bkt >= dest) {
        sz >>= 1;
        hdr1 = slabhdr(ptr, hdrtab);
        slabsetbkt(hdr1, bkt);
        slabsetfree(hdr1);
        slabclrlink(hdr1);
        if (bkt != dest) {
            slablk(bkt);
        }
        if (zone[bkt]) {
            slabsetprev(zone[bkt], hdr1, hdrtab);
            slabsetnext(hdr1, zone[bkt], hdrtab);
        }
        zone[bkt] = hdr1;
        if (bkt != dest) {
            slabunlk(bkt);
        }
        ptr += sz;
    }
    hdr1 = slabhdr(ptr, hdrtab);
    slabsetbkt(hdr1, dest);
    slabsetfree(hdr1);
    slabclrlink(hdr1);
    if (zone[dest]) {
        slabsetprev(zone[dest], hdr1, hdrtab);
        slabsetnext(hdr1, zone[dest], hdrtab);
    }
    zone[dest] = hdr1;
        
    return;
}

/*
 * allocate slab; split from larger ones if necessary.
 */
void *
slaballoc(struct slabhdr **zone, struct slabhdr *hdrtab,
          unsigned long nb, unsigned long flg)
{
    unsigned long   bkt1 = max(SLABMINLOG2, slabbkt(nb));
    unsigned long   bkt2 = bkt1;
    struct slabhdr *hdr1;
    uint8_t        *ptr = NULL;
    struct slabhdr *hdr2;

    slablk(bkt1);
    hdr1 = zone[bkt1];
    if (!hdr1) {
        while (!hdr1 && ++bkt2 < PTRBITS) {
            hdr1 = zone[bkt2];
            if (hdr1) {
                slabsplit(zone, hdrtab, hdr1, bkt1);
                hdr1 = zone[bkt1];
            }
        }
    }
    if (hdr1) {
        hdr2 = slabgetnext(hdr1, hdrtab);
        if (hdr2) {
            slabclrprev(hdr2);
        }
        zone[bkt1] = hdr2;
        slabclrfree(hdr1);
        slabclrlink(hdr1);
        slabsetflg(hdr1, flg);
        ptr = slabadr(hdr1, hdrtab);
    }
    slabunlk(bkt1);
    if (flg & SLABZERO) {
        bzero(ptr, 1UL << bkt1);
    }

    return ptr;
}

/*
 * free slab; combine to form bigger ones if possible.
 */
void
slabfree(struct slabhdr **zone, struct slabhdr *hdrtab, void *ptr)
{
    struct slabhdr *hdr  = slabhdr(ptr, hdrtab);
    unsigned long   bkt = slabgetbkt(hdr);

#if (!MEMTEST)
    vmfreephys(ptr, 1UL << bkt);
#endif
    slabsetfree(hdr);
    if (!slabcomb(zone, hdrtab, hdr)) {
        bkt = slabgetbkt(hdr);
        if (zone[bkt]) {
            slabsetprev(zone[bkt], hdr, hdrtab);
            slabsetnext(hdr, zone[bkt], hdrtab);
        } else {
            slabclrnext(hdr);
        }
        zone[bkt] = hdr;
    }

    return;
}

