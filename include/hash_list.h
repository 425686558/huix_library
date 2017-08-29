#ifndef __HASH_LIST_H__
#define __HASH_LIST_H__
#include "list.h"
#include "hash.h"
enum hash_list_errno {
  HASH_LIST_OK,
  HASH_LIST_NOT_FOUND,
  HASH_LIST_NOMEM,
  HASH_LIST_REHASH_FAILED,
};

enum hash_list_search_type {
  HASH_LIST_SEARCH_INT = 1,
  HASH_LIST_SEARCH_STRING,
};

extern hash_t *hash_list_init(int rbt,int rlt, int *retval,
  enum hash_list_search_type type);
extern void hash_list_info(hash_t *self);
#endif
