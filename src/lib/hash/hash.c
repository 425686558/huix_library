#include "hash.h"
#include <string.h>
u64 string_hash(hash_t *self, hash_key key)
{
  char *str = (char *)key;
  int str_len = strlen(str);
  int i;
  u64 ret = 0;
  for (i = 0;i < str_len;i++) {
    ret=(ret << 1)+1;
    ret+= str[i];
  }
  return ret;
}

u64 X31_string_hash(hash_t *self, hash_key key)
{
  char *s = (char *)key;
  u32 h = *(int *)s;
  if (h) for (++s ; *s; ++s) h = (h << 5) - h + (int)*s;
  return h;
}

u64 fnv1a_64string_hash(hash_t *self, hash_key key)
{
  char *s = (char *)key;
  u64 h = *(int *)s;
  if (h)
    for (++s;*s;++s) {
      h^= (int)*s;
      h+= (h << 1) + (h << 4) + (h << 5) +
           (h << 7) + (h << 8) + (h << 40);
    }
    return h;;
}
