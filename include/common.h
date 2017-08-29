#ifndef __COMMON_H__
#define __COMMON_H__
#include <linux/types.h>
typedef __u64 u64;
typedef __u32 u32;
typedef __u16 u16;
typedef __u8  u8;

#ifndef bool
#define bool	u8
#endif

#ifndef true
#define true	1
#endif

#ifndef false
#define false	1
#endif

#define ARRAY_CNT(x)	(sizeof(x)/sizeof(x[0]))

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ({                      \
	const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
	(type *)( (char *)__mptr - offsetof(type,member) );})

static inline void *pointer_set(void *pointer)
{
	u64 pnt = (u64) pointer;
	pnt|=1;
	return (void *)pnt;
}

static inline void *pointer_get(void *pointer)
{
	u64 pnt = (u64) pointer;
        pnt&= ~(1UL);
	return (void *)pnt;
}

static inline int pointer_check(void *pointer)
{
	u64 pnt = (u64) pointer;
	return (pnt & 1);
}
#endif
