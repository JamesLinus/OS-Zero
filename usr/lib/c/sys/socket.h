/* zero c library socket network programming interface */

/* http://pubs.opengroup.org/onlinepubs/009695399/basedefs/sys/types.h.html */

#ifndef __SYS_SOCKET_H__
#define __SYS_SOCKET_H__

#include <features.h>
#include <stddef.h>
#if (USEGNU)
#include <sys/types.h>
#endif
#include <zero/cdecl.h>
#include <bits/sys/socket.h>

typedef long           socklen_t;
typedef unsigned short sa_family_t;

struct sockaddr {
	sa_family_t sa_family;		// address family
#if defined(EMPTY)
	char        sa_data[EMPTY]; // actual address
#endif
};

#if (USEBSD)
struct osockaddr {
	unsigned short sa_family;
	unsigned char  sa_data[14];
};
#endif

#if (USEGNU)
struct ucred {
	pid_t pid;
	uid_t uid;
	gid_t gid;
};
#endif

struct msghdr {
	void         *msg_name;		// optional address
	socklen_t     msg_namelen;	// address length
	struct iovec *msg_iov;		// scatter-gather I/O structures
	int           msg_iovlen;	// number of scatter-gather I/O structures
	void         *msg_control;	// ancillary data
	socklen_t     msg_controllen; // ancillary data size
	int           msg_flags;    // flags
};

struct cmsghdr {
	socklen_t      cmsg_len;
	int            cmsg_level;
	int            cmsg_type;
#if defined(EMPTY)
	unsigned char  cmsg_data[EMPTY];
#endif
};

struct linger {
	int l_onoff;	// on/off
	int l_linger;	// linger time in seconds
};

extern int     accept(int sock, struct sockadr *adr, socklen_t *adrlen);
extern int     bind(int sock, const struct sockadr *adr, socklen_t adrlen);
extern int     connect(int sock, const struct sockadr *adr, socklen_t adrlen);
extern int     getpeername(int sock, struct sockadr *adr, socklen_t *adrlen);
extern int     getsockname(int sock, struct sockadr *adr, socklen_t *adrlen);
extern int     getsockopt(int sock, int level, int name, void *val, socklen_t *len);
extern int     listen(int sock, int backlog);
extern ssize_t recv(int sock, void *buf, size_t len, int flags);
extern ssize_t recvfrom(int sock, void *buf, size_t len, int flags, struct sockadr *adr,
	                    socklen_t *adrlen);
extern ssize_t recvmsg(int sock, struct msghdr *msg, int flags);
extern ssize_t send(int sock, const void *msg, size_t len, int flags);
extern ssize_t sendmsg(int sock, const struct msghdr *msg, int flags);
extern ssize_t sendto(int sock, const void *msg, size_t len, int flags,
                      const struct sockaddr *destadr, socklen_t destlen);
extern int     setsockopt(int sock, int level, int name,
                          const void *val, socklen_t len);
extern int     shutdown(int sock, int how);
extern int     socket(int domain, int type, int proto);
extern int     socketpair(int domain, int type, int proto, int sockvec[2]);
#if (USEXOPEN2K)
/* determine wheter socket is at an out-of-band mark */
extern int     sockatmark(int fd);
#endif

#endif /* __SYS_SOCKET_H__ */
