#ifndef __SYS_TYPES_H__
#define __SYS_TYPES_H__

#include <features.h>
#include <stdint.h>
#include <stddef.h>
//#include <limits.h>
//#include <sys/types.h>
#if (_BSD_SOURCE)
#include <endian.h>
//#include <sys/select.h>
#include <sys/sysmacros.h>
#endif
#if !defined(_POSIX_SOURCE) && (USEBSD) && !defined(NFDBITS)
#include <kern/conf.h>
#endif

typedef long            register_t;
typedef uint8_t         u_int8_t;
typedef uint16_t        u_int16_t;
typedef uint32_t        u_int32_t;
typedef uint64_t        u_int64_t;
#if (_BSD_SOURCE)
typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;
typedef unsigned long   u_long;
typedef int64_t         quad_t;
typedef uint64_t        u_quad_t;
typedef quad_t         *qaddr_t;
#endif
typedef unsigned int    uint;
typedef unsigned short  ushort;
typedef unsigned long   ulong;
#if 0
typedef int64_t         blkcnt_t;
typedef int64_t         clock_t;
#endif
typedef long            blksize_t;
typedef long            blksize_cnt;
typedef short           cnt_t;
typedef char           *caddr_t;        // core address
typedef long            daddr_t;        // disk address
typedef long            clock_t;
typedef long            clockid_t;
typedef int32_t         dev_t;          // device number
typedef dev_t           major_t;
typedef dev_t           minor_t;
//typedef uint32_t        fsblkcnt_t;     // filesystem block count
//typedef uint32_t        fsfilcnt_t;     // filesystem file count
typedef uint32_t        uid_t;
typedef uint32_t        gid_t;          // group ID
typedef uintptr_t       key_t;          // IPC key
typedef unsigned long   mode_t;         // file attributes
typedef int32_t         nlink_t;        // link count
typedef int64_t         loff_t;
#if (_FILE_OFFSET_BITS == 32)
typedef int32_t         off_t;          // 32-bit file offset
typedef int32_t         blkcnt_t;
typedef uint32_t        ino_t;          // inode number
typedef uint32_t        fsblkcnt_t;
typedef uint32_t        fsfilcnt;
#else
typedef int64_t         off_t;          // 64-bit file offset
typedef int64_t         blkcnt_t;
typedef uint64_t        ino_t;
typedef uint64_t        fsblkcnt_t;
typedef uint64_t        fsfilcnt_t;
#endif
#if !defined(__pid_t_defined)
typedef long            pid_t;          // process ID
#define __pid_t_defined
#endif
#if defined(_LARGEFILE64_SOURCE)
typedef int64_t         off64_t;
typedef uint64_t        ino64_t;
typedef int64_t         blkcnt64_t;
typedef uint64_t        fsblkcnt64_t;
typedef uint64_t        fsfilcnt64_t;
#endif
typedef uintptr_t       id_t;
#if defined(_MSC_VER)
typedef long long       ssize_t;
#else
typedef long            ssize_t;
#endif
typedef unsigned long   useconds_t;
typedef long            suseconds_t;
typedef int64_t         time_t;
typedef long            timer_t;
typedef int64_t         offset_t;
typedef uint64_t        u_offset_t;
typedef uint64_t        len_t;
typedef int64_t         diskaddr_t;
typedef unsigned long   fsid_t;

/* virtual memory related types */
typedef uintptr_t       pfn_t;          // page frame #
typedef uintptr_t       pgcnt_t;        // # of pages
typedef intptr_t        spgcnt_t;       // signed # of pages
typedef unsigned char   use_t;          // use count for swap
typedef short           sysid_t;
typedef short           index_t;
typedef void           *timeout_id_t;   // opaque handle from timeout()
typedef void           *bufcall_id_t;   // opaque handle from bufcall()

typedef id_t            taskid_t;
typedef id_t            projid_t;

typedef id_t            ctid_t;
typedef id_t            zoneid_t;

/* FIXME: <trace.h> stuff */
typedef struct {
    uint64_t flg;
}                trace_attr_t;
typedef uint64_t trace_id_t;
typedef uint64_t trace_event_id_t;
typedef uint64_t trace_event_set_t;

/* POSIX threads */
/*
 * TODO
 * ----
 * - http://pubs.opengroup.org/onlinepubs/009696699/basedefs/sys/types.h.html
 */

/* FIXME: this file should #include <time.h> (?) :) */
#include <time.h>
#if (_BSD_SOURCE)
#include <sys/param.h>
#endif

#define P_MYPID         ((pid_t)0)
#define P_MYID          (-1)
#define NOPID           ((pid_t)-1)

#define PFN_INVALID     ((pfn_t)-1)

#define NODEV           ((dev_t)-1L)

#if (_ZERO_SOURCE)
#include <kern/conf.h>
#endif
#if defined(NPROCFD)
#define FD_SETSIZE NPROCFD
#elif (_POSIX_SOURCE)
#define FD_SETSIZE _POSIX_FD_SETSIZE
#endif

#if (USEBSD) && !defined(NFDBITS)

#include <limits.h>

typedef long       fd_mask;
#define NFDBITS    (sizeof(fd_mask) * CHAR_BIT)

struct fd_set {
#if (USEXOPEN)
    fd_mask fds_bits[FD_SETSIZE / NFDBITS];
#else
    fd_mask __fds_bits[FD_SETSIZE / NFDBITS];
#endif
};
typedef struct fd_set fd_set;

#define FD_SET(fd, set)    setbit(set->fd_bits, fd)
#define FD_CLR(fd, set)    clrbit(set->fd_bits, fd)
#define FD_ISSET(fd, set)  bitset(set->fd_bits, fd)
#define FD_ZERO(set)       memset(set->fd_bits, 0, FD_SETSIZE / CHAR_BIT)
#if (USEBSD)
#define FD_COPY(src, dest) memcpy(dest, src, sizeof(fd_set))
#endif

#endif /* !defined(NFDBITS) */

#if (USEBSD) && !defined(__struct_timeval_defined)
#define __struct_timeval_defined 1
struct timeval {
    time_t      tv_sec;
    suseconds_t tv_usec;
};
#endif

#endif /* __SYS_TYPES_H__ */

