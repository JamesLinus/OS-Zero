#ifndef __ALLOCA_H__
#define __ALLOCA_H__

#if (!__KERNEL__)

#include <stddef.h>

#if defined(__GNUC__)
#define alloca(size) __builtin_alloca(size)
#elif defined(_MSC_VER)
#define alloca(size) _alloca(size)
#else
void * alloca(size_t size);
#endif

#endif

#endif /* __ALLOCA_H__ */

