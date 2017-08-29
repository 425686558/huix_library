#ifndef __HASH_H__
#define __HASH_H__
#include "common.h"
#include <errno.h>
#include <stdlib.h>
typedef void* hash_key;
typedef void* hash_val;
typedef struct hash hash_t;
struct hash {
  hash_key *keys;
  hash_val *vals;
  int cnt;
  u64 (*hash)(hash_t *self, hash_key key);
  void *(*getvals)(hash_t *self,int idx);
  void *(*setvals)(hash_t *self,int idx,void *data);
  void *(*insert)(hash_t *self, hash_key key, hash_key val,int *ret);
  void *(*delete)(hash_t *self, hash_key key);
  void *(*rehash)(hash_t *self, int *ret);
  void *(*query)(hash_t *self, hash_key key, int *ret);
};

extern u64 string_hash(hash_t *self, hash_key key);
extern u64 X31_string_hash(hash_t *self, hash_key key);
extern u64 fnv1a_64string_hash(hash_t *self, hash_key key);
static inline int hash_init(hash_t *hash, int num)
{
  hash->keys = calloc(num, sizeof(hash_key));
  if (hash->keys == NULL)
    return -ENOMEM;
  hash->vals = calloc(num, sizeof(hash_val));
  if (hash->vals == NULL) {
    free(hash->keys);
    return -ENOMEM;
  }
  return 0;
}
#endif
