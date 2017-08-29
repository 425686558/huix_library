#ifndef __LIBDICT_H__
#define __LIBDICT_H__
#include <linux/types.h>
typedef u64 hash_key;
typedef u64 hash_val;

struct hash_operation {
	int (*conflict)();
	int (*insert)();
	int (*delete)();
}

struct hash {
	hash_key *keys;
	hash_val *val;
}
#endif
