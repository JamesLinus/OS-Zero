/*
 * X86 page tables are accessed as a flat 4-megabyte set of page table entries.
 * The table is mapped to the address PAGETAB declared in vm.h.
 */

#define PAGEDEV 0

#include <stddef.h>
#include <stdint.h>
#include <zero/trix.h>
#include <zero/cdecl.h>
#include <zero/param.h>
#include <kern/conf.h>
#include <kern/task.h>
#include <kern/buf.h>
#include <kern/dev.h>
#include <kern/util.h>
#include <kern/mem/page.h>
#if (SMP)
#include <kern/unit/ia32/mp.h>
#endif
#include <kern/unit/ia32/link.h>
#include <kern/unit/ia32/vm.h>
#include <kern/unit/ia32/dma.h>

#define HICORE (1024 * 1024)

extern void pginit(void);

extern uint32_t     kernpagedir[NPTE];
extern uint32_t    *lapic;
extern struct page  physptab[NPAGEMAX];

static struct vmpage vmpagetab[NPAGEMAX] ALIGNED(PAGESIZE);
#if (PAGEDEV)
static struct dev    vmdevtab[NPAGEDEV];
static volatile long vmdevlktab[NPAGEDEV];
#endif
static struct vmbuf  vmbuftab[1L << (PTRBITS - BUFSIZELOG2)];
struct vmpageq      vmpagelruq;
//struct pageq         vmpagelruq;
struct vmbufq        vmbufq;

unsigned long vmnphyspages;
unsigned long vmnmappedpages;
unsigned long vmnbufpages;
unsigned long vmnwiredpages;

/*
 * virt == 0 -> identity-map
 *
 * 32-bit page directory is flat 4-megabyte table of page-tables.
 * for virtual address virt,
 *
 *     vmphysadr = pagetab[vmpagenum(virt)]; // physical address
 */
void
vmmapseg(uint32_t *pagetab, uint32_t virt, uint32_t phys, uint32_t lim,
         uint32_t flg)
{
    uint32_t  *pte;
    long       n;

#if 0
    if (!virt) {
        virt = phys;
    }
#endif
    n = roundup2(lim - virt, PAGESIZE) >> PAGESIZELOG2;
    pte = pagetab + vmpagenum(virt);
    while (n--) {
        *pte = phys | flg;
        phys += PAGESIZE;
        pte++;
    }

    return;
}

/*
 * initialise virtual memory
 * - zero page tables
 * - initialize page directory
 * - map page directory index page
 * - map our segments
 * - initialise paging
 */
void
vminit(void *pagetab)
{
    uint32_t *pde;
    uint32_t  adr;
    long      n;

    /* initialize page directory index page */
    pde = kernpagedir;
    adr = (uint32_t)pagetab;
    n = NPDE;
    while (n--) {
        *pde = adr | PAGEPRES | PAGEWRITE;
        adr += NPTE * sizeof(uint32_t);
        pde++;
    }

    /* map page directory index page */
    pde = pagetab + vmpagenum(kernpagedir);
    adr = (uint32_t)&kernpagedir;
    *pde = adr | PAGEPRES | PAGEWRITE;

    /* zero page tables */
    bzero(pagetab, PAGETABSIZE);

    /* identity map 3.5G..4G for devices */
    pde = pagetab + vmpagenum(3584UL * 1024 * 1024);
    adr = 3584UL * 1024 * 1024;
    n = (512 * 1024 * 1024) >> PAGESIZELOG2;
    while (n--) {
        *pde = adr | PAGEPRES | PAGEWRITE;
        adr += PAGESIZE;
        pde++;
    }

    /* identity-map 0..1M */
    vmmapseg(pagetab, 0, 0,
           HICORE,
           PAGEPRES | PAGEWRITE);

#if (SMP)
#if 0
    vmmapseg(pagetab, (uint32_t)KERNMPENTRY, (uint32_t)&_mpentry,
           (uint32_t)&_emp - (uint32_t)&_mpentry + KERNMPENTRY,
           PAGEPRES);
#endif
    vmmapseg(pagetab, (uint32_t)&_mpentry, (uint32_t)&_mpentry,
           (uint32_t)&_emp, 
           PAGEPRES);
#endif

    /* identity-map kernel boot segment */
    vmmapseg(pagetab, HICORE, HICORE,
           (uint32_t)&_eboot,
           PAGEPRES | PAGEWRITE);

    /* map kernel DMA buffers */
    vmmapseg(pagetab, (uint32_t)&_dmabuf, DMABUFBASE,
           (uint32_t)&_dmabuf + DMABUFSIZE,
           PAGEPRES | PAGEWRITE);

    /* identity map page tables */
    vmmapseg(pagetab, (uint32_t)pagetab, (uint32_t)pagetab,
           (uint32_t)pagetab + PAGETABSIZE,
           PAGEPRES | PAGEWRITE);

    /* map kernel text/read-only segments */
    vmmapseg(pagetab, (uint32_t)&_text, vmphysadr((uint32_t)&_textvirt),
           (uint32_t)&_etextvirt,
           PAGEPRES);

    /* map kernel DATA and BSS segments */
    vmmapseg(pagetab, (uint32_t)&_data, vmphysadr((uint32_t)&_datavirt),
           (uint32_t)&_ebssvirt,
           PAGEPRES | PAGEWRITE);

    /* identity-map 3.5G..4G */
//    devmap(pagetab, DEVMEM, 512 * 1024 * 1024);

    /* initialize paging */
    pginit();
    kprintf("VM page tables at 0x%ul\n", (unsigned long)pagetab);
    
    return;
}

void *
vmmapvirt(uint32_t *pagetab, void *virt, uint32_t size, uint32_t flg)
{
    uint32_t    adr;
    uint32_t   *pte;
    long        n;

    adr = (uint32_t)virt & PFPAGEMASK;
    n = roundup2(size, PAGESIZE) >> PAGESIZELOG2;
    pte = pagetab + vmpagenum(virt);
    while (n--) {
        *pte = PAGEWRITE | flg;
        pte++;
    }
    
    return (void *)adr;
}

void
vmfreephys(void *virt, uint32_t size)
{
    struct vmbuf *buf;
    uint32_t      adr;
    uint32_t     *pte;
    long          n;
    long          nref;

    n = roundup2(size, PAGESIZE) >> PAGESIZELOG2;
    pte = (uint32_t *)((uint8_t *)&_pagetab + vmpagenum(virt));
    while (n--) {
        adr = *pte;
        adr &= PFPAGEMASK;
        if (*pte & PAGEBUF) {
            buf = &vmbuftab[vmbufid(adr)];
            nref = vmgetbufnref(buf);
            vmsetbufnref(buf, --nref);
            if (!nref) {
                vmrmbuf(adr);
            }
        } else if (*pte & PAGESWAPPED) {
//            swapfree(adr);
        } else if (adr) {
//            kprintf("PHYSFREE: %lx\n", (long)adr);
            if (!(*pte & PAGEWIRED)) {
                vmrmpage(adr);
                vmnmappedpages--;
            } else {
//                kprintf("UNWIRE\n");
                vmnwiredpages--;
            }
            pagefree((void *)adr);
        }
        *pte = 0;
        pte++;
    }

    return;
}

void
vmpagefault(uint32_t adr, uint32_t flags)
{
    uint32_t      *pte = (uint32_t *)&_pagetab + vmpagenum(adr);
    uint32_t       flg = *pte & (PFFLGMASK | PAGESYSFLAGS);
    uint32_t       page = *pte;

    if (!page) {
        page = (uint32_t)pagealloc(adr);
        if (!page) {
            page = (uint32_t)pagealloc(adr);
        }
        if (page) {
            if (flg & PAGEBUF) {
                vmnbufpages++;
                if (vmisbufadr(adr)) {
                    vmaddbuf(adr);
                }
            } else if (flg & PAGEWIRED) {
                vmnwiredpages++;
            } else {
                vmnmappedpages++;
                vmaddpage(adr);
            }
            *pte = page | flg | PAGEPRES;
        }
#if (PAGEDEV)
    } else if (!(page & PAGEPRES)) {
        // pageout();
        page = vmpagein(page);
#endif
    }

    return;
}

#if 0
void
vmseekdev(uint32_t dev, uint64_t ofs)
{
    devseek(vmdevtab[dev], ofs, SEEK_SET);
}

uint32_t
vmpagein(uint32_t page)
{
    uint32_t  pgid = pagefind(page);
    uint32_t  blk = vmblkid(pgid);
    void     *data;

    mtxlk(&vmdevlktab[dev], MEMPID);
    vmseekdev(dev, blk * PAGESIZE);
//    data = pageread(dev, PAGESIZE);
    mtxunlk(&vmdevlktab[pagedev], MEMPID);
}
#endif /* 0 */

