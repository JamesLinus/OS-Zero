#ifndef __FCNTL_H__
#define __FCNTL_H__

#include <features.h>
#include <sys/types.h>
#if (_XOPEN_SOURCE)
#include <sys/stat.h>
#endif
#include <sys/uio.h>
#if (_XOPEN_SOURCE)
#include <kern/io.h>
#endif

#define O_ACCMODE   0003
#define O_RDONLY    00
#define O_WRONLY    01
#define O_RDWR      02
#define O_CREAT     0100
#define O_EXCL      0200
#define O_NOCTTY    0400
#define O_TRUNC     01000
#define O_APPEND    02000
#define O_NONBLOCK  04000
#define O_NDELAY    O_NONBLOCK
#define O_SYNC      010000
#define O_FSYNC     O_SYNC
#define O_ASYNC     020000
#if (_GNU_SOURCE)
#define O_DIRECT    040000
#define O_DIRECTORY 0200000
#define O_NOFOLLOW  0400000
#define O_NOATIME   01000000
#endif
#if (USEPOSIX199309) || (USEUNIX98)
#define O_DSYNC     O_SYNC  // synchronise data
#define O_RSYNC     O_SYNC  // synchronise read operations
#endif
#define O_LARGEFILE 0100000

/* cmd arguments for fcntl() */
#define F_DUPFD     0
#define F_GETFD     1
#define F_SETFD     2
#define F_GETFL     3
#define F_SETFL     4
#if (_FILE_OFFSET_BITS == 64)
#define F_GETLK     5
#define F_SETLK     6
#define F_SETLKW    7
#else
#define F_GETLK     F_GETLK64
#define F_SETLK     F_SETLK64
#define F_SETLKW    F_SETLKW64
#endif
#define F_GETLK64   12
#define F_SETLK64   13
#define F_SETLKW64  14
#if (_BSD_SOURCE) || (USEUNIX98)
#define F_SETOWN    8
#define F_GETOWN    9
#endif
#if (_GNU_SOURCE)
#define F_SETSIG    10
#define F_GETSIG    11
#define F_SETLEASE  1024
#define F_GETLEASE  1025
#define F_NOTIFY    1026
#endif
#define FD_CLOEXEC  0x01
/* lockf() */
#define F_RDLCK     0
#define F_WRLCK     1
#define F_UNLCK     2
/* old BSD flock() */
#define F_EXLCK     4
#define F_SHLCK     8
#if (_BSD_SOURCE)
#define LOCK_SH     1
#define LOCK_EX     2
#define LOCK_NB     4
#define LOCK_UN     8
#endif
#if (_GNU_SOURCE)
#define LOCK_MAND   0x00000020
#define LOCK_READ   0x00000040
#define LOCK_WRITE  0x00000080
#define LOCK_RW     (LOCK_READ | LOCK_WRITE)
#endif

#if (_GNU_SOURCE)
#define DN_ACCESS    0x00000001
#define DN_MODIFY    0x00000002
#define DN_CREATE    0x00000004
#define DN_DELETE    0x00000008
#define DN_RENAME    0x00000010
#define DN_ATTRIB    0x00000020
#define DN_MULTISHOT 0x80000000
#endif

/* these constants are in <unistd.h> as well */
#if !defined(R_OK)
#define R_OK 4
#define W_OK 2
#define X_OK 1
#define F_OK 0
#endif

#if !defined(F_LOCK) && (USEXOPENEXT) && !(_POSIX_SOURCE)
/* these constants also appear in <unistd.h> */
#define F_ULOCK 0
#define F_LOCK  1
#define F_TLOCK 2
#define F_TEST  3

extern int lockf(int fd, int cmd, off_t len);
#endif

#if (_GNU_SOURCE)
#define AT_FDCWD            -100
#define AT_SYMLINK_NOFOLLOW 0x100
#define AT_REMOVEDIR        0x200
#define AT_SYMLINK_FOLLOW   0x400
#define AT_EACCESS          0x200
#endif

struct flock {
    short   l_type;
    short   l_whence;
#if (_FILE_OFFSET_BITS == 64)
    off_t   l_start;
    off_t   l_len;
#else
    off64_t l_start;
    off64_t l_len;
#endif
    pid_t   l_pid;
};

#if (_LARGEFILE64_SOURCE)
struct flock64 {
    short   l_type;
    short   l_whence;
    off64_t l_start;
    off64_t l_len;
    pid_t   l_pid;
};
#endif

#if (_BSD_SOURCE)
#define FAPPEND   O_APPEND
#define FFSYNC    O_FSYNC
#define FASYNC    O_ASYNC
#define FNONBLOCK O_NONBLOCK
#define FNDELAY   O_NDELAY
#endif

#if (USEXOPEN2K)
#define POSIX_FADV_NORMAL     0
#define POSIX_FADV_RANDOM     1
#define POSIX_FADV_SEQUENTIAL 2
#define POSIX_FADV_WILLNEED   3
#define POSIX_FADV_DONTNEED   4
#define POSIX_FADV_NOREUSE    5
#endif

#if (_GNU_SOURCE)
#define SYNC_FILE_RANGE_WAIT_BEFORE 1
#define SYNC_FILE_RANGE_WRITE       2
#define SYNC_FILE_RANGE_WAIT_AFTER  4

#define SPLICE_F_MOVE               1
#define SPLICE_F_NONBLOCK           2
#define SPLICE_F_MORE               4
#define SPLICE_F_GIFT               8
#endif

extern int     creat(const char *file, mode_t mode);
extern int     fcntl(int fd, int cmd, ...);
#if (_XOPEN_SOURCE >= 600 || _POSIX_C_SOURCE >= 200112L || (USEXOPEN2K))
extern int     posix_fallocate(int fd, off_t ofs, off_t len);
#endif
#if (_POSIX_C_SOURCE >= 200112L) || (USEXOPEN2K)
extern int     posix_fadvise(int fd, off_t ofs, size_t len, int advice);
#endif
#if (_GNU_SOURCE)
//typedef int64_t off64_t;
extern ssize_t readahead(int fd, off64_t ofs, size_t count);
extern int     sync_file_range(int fd, off64_t from, off64_t to,
                               unsigned int flags);
extern int     vmsplice(int outfd, const struct iovec *iov, size_t cnt,
                        unsigned int flags);
extern int     splice(int infd, off64_t inofs, int outfd, off64_t outofs,
                      size_t len, unsigned int flags);
extern int     tee(int infd, int outfd, size_t len, unsigned int flags);
#endif

#if (_GNU_SOURCE)
extern int     openat(int fd, const char *file, int flg);
#endif

#endif /* __FCNTL_H__ */

