#ifndef __KERN_IO_BUF_H__
#define __KERN_IO_BUF_H__

#include <stdint.h>
#include <kern/conf.h>
#include <kern/perm.h>

#define NEWBUFBLK 1

#if (!BUFMULTITAB)
#define BUFNHASHBIT 16
#define BUFNHASH    (1UL << BUFNHASHBIT)
#endif

/*
 * 64-bit off_t
 * 50 significant off_t bits for buffers
 * 2^16-byte i.e. 64KB default buffer size optimized for TCP/IP v4
 */

#define BUFMINSIZE     (1UL << BUFMINSIZELOG2)
#define BUFMINSIZELOG2 14
#define BUFSIZE        (1UL << BUFSIZELOG2)
#define BUFSIZELOG2    16
/* size of buffer cache */
#define BUFNBYTE       (BUFNMEG * 1024 * 1024)
/* max # of cached blocks */
#define BUFNBLK        (BUFNBYTE >> BUFMINSIZELOG2)
#define BUFNIDBIT      (64 - BUFMINSIZELOG2)
#define BUFNDEV        256
#define BUFDEVMASK     (BUFNDEV - 1)
#define BUFNL1BIT      18
#define BUFNL2BIT      10
#define BUFNL3BIT      10
#define BUFNL4BIT      12
#define BUFNL1ITEM     (1UL << BUFNL1BIT)
#define BUFNL2ITEM     (1UL << BUFNL2BIT)
#define BUFNL3ITEM     (1UL << BUFNL3BIT)
#define BUFNL4ITEM     (1UL << BUFNL4BIT)
#define BUFL1SHIFT     (BUFMINSIZELOG2 + BUFNL4BIT + BUFNL3BIT + BUFNL2BIT)
#define BUFL2SHIFT     (BUFMINSIZELOG2 + BUFNL4BIT + BUFNL3BIT)
#define BUFL3SHIFT     (BUFMINSIZELOG2 + BUFNL4BIT)
#define BUFL1MASK      (BUFNL1ITEM - 1)
#define BUFL2MASK      (BUFNL2ITEM - 1)
#define BUFL3MASK      (BUFNL3ITEM - 1)
#define BUFL4MASK      (BUFNL4ITEM - 1)

#define bufkey(num) (((num) >> BUFNOFSBIT) & ((UINT64_C(1) << BUFNIDBIT) - 1))
#define bufclr(blk)                                                     \
    do {                                                                \
        void *_tmp = NULL;                                              \
                                                                        \
        blk->data &= BUFADRMASK;                                        \
        blk->prev = _tmp;                                               \
        blk->next = _tmp;                                               \
        blk->tabprev = _tmp;                                            \
        blk->tabnext = _tmp;                                            \
    } while (0)
/* data values */
#define BUFSIZEBITS  6          // shift count 0..63
#define BUFHASDATA   (1 << 6)   // buffer has valid data
#define BUFDIRTY     (1 << 7)   // kernel must write before reassigning
#define BUFDOINGIO   (1 << 8)   // kernel is reading or writing data
#define BUFADRMASK   (~((1UL << BUFMINSIZELOG2) - 1))
#define BUFINFOMASK  (~BUFADRMASK)
#if (NEWBUFBLK)
struct bufblk {
    uintptr_t      data;        // address, flags, and size-shift
    int64_t        dev;         // major and minor device IDs
    int64_t        nref;        // # of items in subtables
    int32_t        num;         // per-device block ID
    int32_t        chksum;      // checksum such as IPv4
    struct bufblk *prev;        // previous block on free-list or LRU
    struct bufblk *next;        // next block on free-list or LRU
    struct bufblk *tabprev;     // previous block in table chain
    struct bufblk *tabnext;     // next block in table chain
};
#else
struct bufblk {
    int64_t        dev;         // device #
    int64_t        num;         // per-device block #
    int64_t        chksum;      // checksum such as IPv4 or IPv6
    long           status;      // status flags
    long           nb;          // # of bytes
    long           nref;        // # of items in subtables
    void          *data;        // in-core block data (kernel virtual address)
    struct bufblk *prev;        // previous block on free list or LRU
    struct bufblk *next;        // next block on free list or LRU
    struct bufblk *tabprev;     // previous block in table chain
    struct bufblk *tabnext;     // next block in table chain
};
#endif

struct bufblkqueue {
    volatile long  lk;
    struct bufblk *head;
    uint8_t        _pad[CLSIZE - sizeof(long) - sizeof(void *)];
};

long            bufinit(void);
struct bufblk * bufalloc(void);
void            bufaddblk(struct bufblk *blk);
struct bufblk * buffindblk(dev_t dev, off_t num, long rel);

#endif /* __KERN_IO_BUF_H__ */

