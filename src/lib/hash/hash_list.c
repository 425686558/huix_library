#include "hash.h"
#include "list.h"
#include "hash_list.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#define REHASH_DELETE_RATE 0.2
struct hash_bucket {
  int num;
  list_t head;
};

struct list_item {
  union {
    struct {
      char *str;
    };
    u64 num;
  };
  void *val;
};

struct hash_list {
  hash_t table;
  struct hash_bucket *bucket_table;
  void *(*search)(struct hash_list *,struct hash_bucket *, hash_key);
  struct list_item *(*alloc_item)(struct hash_list *,hash_key,hash_val);
  void *(*free_item)(struct hash_list *,struct list_item *);
  int bucket_cnt;
  int rehash_bucket_base;
  int rehash_list_triger;
  int rehash_upper_rate_num;
};

void hash_list_bucket_string_info(struct hash_bucket *bucket)
{
  void *string_list_item_show(struct list *item,void *data)
  {
    struct list_item *str_item = item->val;
    if (pointer_check((void *)item->val)) {
      data = (void *)0;
      return data;
    }
    printf("%lx %s\n",(unsigned long)data,str_item->str);
    return ((char *)data + 1);
  }
  printf("^^^^^^^^^^^^^^^^^^^^\n");
  list_iter_func(&bucket->head, string_list_item_show);
  printf("vvvvvvvvvvvvvvvvvvvv\n");
}

void *hash_list_getvals(hash_t *self, int idx)
{
  struct hash_list *hl = container_of(self, struct hash_list, table);
  return &hl->bucket_table[idx];
}

static int hash_list_get_string_item(struct list *item,void *data)
{
  struct list_item *str_item = item->val;
  char *str = data;
  if (pointer_check((void *)item->val))
    return 1;
  return strcmp(str_item->str,str);
}

void *hash_list_search_string_item(struct hash_list *hl, struct hash_bucket *bucket,
  hash_key key)
{
  return list_search_func(&bucket->head, hash_list_get_string_item, (void *)key);
}

struct list_item *hash_list_alloc_item_string(struct hash_list *hl,
  hash_key key, hash_val val)
{
  struct list_item *item = malloc(sizeof(struct list_item));
  if (!item) {
    return NULL;
  }
  char *str_key = (char *) key;
    item->str = strdup(str_key);
  item->val = (void *)val;
  return item;
}

void *hash_list_free_string(struct hash_list *hl,
  struct list_item *item)
{
  free(item->str);
  free(item);
  return NULL;
}

void *hash_list_insert(hash_t *self, hash_key key, hash_val val, int *ret)
{
  struct hash_list *hl = container_of(self, struct hash_list, table);
  struct hash_bucket *bucket = NULL;
  list_t *item;
  void *retval;
  struct list_item *insert_item;
  u64 hashval = hl->table.hash(self,key);
  int index = hashval % hl->bucket_cnt;
//  printf("insert index:%d hashvale:hex(%llx) digit(%lld)\n",index,hashval,hashval);
  bucket = hl->table.getvals(self, index);
  if (bucket->num == 0) {
    insert_item = hl->alloc_item(hl, key, val);
    if (!insert_item) {
      *ret = -HASH_LIST_NOMEM;
      return NULL;
    }
    list_add_prev_val(&bucket->head, insert_item);
    bucket->num++;
    self->cnt++;
    *ret = HASH_LIST_OK;
    return NULL;
  }
  item = hl->search(hl, bucket, key);
  if (!item) {
    insert_item = hl->alloc_item(hl, key, val);
    if (!insert_item) {
      *ret = -HASH_LIST_NOMEM;
      return NULL;
    }
    list_add_prev_val(&bucket->head, insert_item);
    bucket->num++;
    if (bucket->num == ((int)(hl->rehash_list_triger * REHASH_DELETE_RATE)))
      hl->rehash_upper_rate_num++;
    if (bucket->num >= hl->rehash_list_triger) {
      int err = 0;
      hl->table.rehash(&hl->table, &err);
      if (err < 0) {
        /*debug
	  printf("rehash failed\n");*/
        *ret = -HASH_LIST_REHASH_FAILED;
	return NULL;
      }
    }
    retval = NULL;
  } else {
    insert_item = list_get(item);
    retval = insert_item->val;
    insert_item->val = (void *)val;
    list_set(item, insert_item);
  }
  *ret = HASH_LIST_OK;
  return retval;
}

void *hash_list_delete(hash_t *self, hash_key key)
{
  struct hash_list *hl = container_of(self, struct hash_list, table);
  struct hash_bucket *bucket= NULL;
  list_t *item;
  void *retval;
  int err = 0;
  struct list_item *delete_item;
  u64 hashval = hl->table.hash(self,key);
  int index = hashval % hl->bucket_cnt;
  bucket = hl->table.getvals(self, index);
  item = hl->search(hl, bucket, key);
  if (!item) {
    return NULL;
  } else {
    delete_item = list_del(item);
    free(item);
    retval = delete_item->val;
    hl->free_item(hl, delete_item);
    if (bucket->num == ((int)(hl->rehash_list_triger * REHASH_DELETE_RATE)))
      hl->rehash_upper_rate_num--;
    bucket->num--;
    if ((hl->rehash_bucket_base < hl->bucket_cnt) &&
      hl->rehash_upper_rate_num == 0) {
      hl->table.rehash(self, &err);
      if (err < 0) {
        printf("func:%s error\n",__func__);
        return NULL;
      }
    }
  }
  return retval;
}

static inline int hash_list_rehash_expand(struct hash_list *hl,hash_t *self)
{
  struct list_item *rehash_item;
  list_t *list_tmp,*list_pos;
  struct hash_bucket *new_bucket_table;
  hash_key key;
  int src_index,i;
  u64 hashval;
  int new_bucket_cnt = hl->bucket_cnt * 2;
  /*debug
  printf("before------------\n");
  for (i = 0;i < hl->bucket_cnt;i++) {
    printf("bkt:%p n:%p p:%p np:%p pn:%p\n",&hl->bucket_table[i].head,
      hl->bucket_table[i].head.next,hl->bucket_table[i].head.prev,
      hl->bucket_table[i].head.next->prev,hl->bucket_table[i].head.prev->next);
  }*/
  new_bucket_table = realloc(hl->bucket_table,
    new_bucket_cnt * sizeof(struct hash_bucket));
  if (new_bucket_table == NULL) {
    return -ENOMEM;
  }
  /*debug
  printf("after+++++++++++++++++\n");*/
  
  for (i = 0;i < hl->bucket_cnt;i++) {
    /*debug
    printf("bkt:%p n:%p p:%p np:%p pn:%p\n",&new_bucket_table[i].head,
      new_bucket_table[i].head.next,new_bucket_table[i].head.prev,
      new_bucket_table[i].head.next->prev,new_bucket_table[i].head.prev->next);*/
    if (new_bucket_table[i].num == 0) {
      list_init(&new_bucket_table[i].head);
    }
    new_bucket_table[i].head.next->prev = &new_bucket_table[i].head;
    new_bucket_table[i].head.prev->next = &new_bucket_table[i].head;
  }
  for (i = hl->bucket_cnt;i < new_bucket_cnt;i++) {
    list_init(&new_bucket_table[i].head);
    new_bucket_table[i].num = 0;
    list_set(&new_bucket_table[i].head,pointer_set(new_bucket_table[i].head.val));
  }
  hl->bucket_table = new_bucket_table;
  hl->bucket_cnt = new_bucket_cnt;
  for (src_index = 0;src_index < hl->bucket_cnt;src_index++) {
    struct hash_bucket *src_bucket = hl->table.getvals(self,src_index);
    if (src_bucket->num == 0) {
      continue;
    }
    //debug
    //hash_list_bucket_string_info(src_bucket);
    
    list_for_each_safe(list_pos, list_tmp, &src_bucket->head) {
      struct hash_bucket *dst_bucket;
      int dst_index;
      if (pointer_check((void *)list_pos->val))
        continue;
      rehash_item = list_get(list_pos);
      key = (hash_key) rehash_item->str;
      hashval = self->hash(self, key);
      dst_index = hashval % hl->bucket_cnt;
      if (dst_index == src_index)
        continue;
      dst_bucket = hl->table.getvals(self,dst_index);
      list_del(list_pos);
      list_add_prev_point(list_pos,&dst_bucket->head);
      if (src_bucket->num == ((int)(hl->rehash_list_triger * REHASH_DELETE_RATE)))
        hl->rehash_upper_rate_num--;
      src_bucket->num--;
      dst_bucket->num++;
      if (dst_bucket->num == ((int)(hl->rehash_list_triger * REHASH_DELETE_RATE)))
        hl->rehash_upper_rate_num++;
    }
  }
  return 0;
}

int hash_list_rehash_shrink(struct hash_list *hl,hash_t *self)
{
  struct list_item *rehash_item;
  struct hash_bucket *new_bucket_table;
  list_t *list_tmp,*list_pos;
  int new_bucket_cnt = hl->bucket_cnt / 2;
  int src_index;
  hash_key key;
  u64 hashval;
  for (src_index = 0;src_index < hl->bucket_cnt;src_index++) {
    struct hash_bucket *src_bucket = hl->table.getvals(self,src_index);
    struct hash_bucket *dst_bucket = NULL;
    int dst_index = 0;
    if (src_bucket->num == 0) {
      continue;
    }
    list_for_each_safe(list_pos, list_tmp, &src_bucket->head) {
      if (pointer_check((void *)list_pos->val))
        continue;
      rehash_item = list_get(list_pos);
      key = (hash_key) rehash_item->str;
      hashval = self->hash(self, key);
      dst_index = hashval % new_bucket_cnt;
      if (dst_index == src_index)
        continue;
      dst_bucket = hl->table.getvals(self,dst_index);
      list_del(list_pos);
      list_add_prev_point(list_pos,&dst_bucket->head);
      if (src_bucket->num == ((int)(hl->rehash_list_triger * REHASH_DELETE_RATE))) {
        hl->rehash_upper_rate_num--;
      }
      src_bucket->num--;
      dst_bucket->num++;
      if (dst_bucket->num == ((int)(hl->rehash_list_triger * REHASH_DELETE_RATE))) {
        hl->rehash_upper_rate_num++;
      }
    }
  }
  hl->bucket_cnt = new_bucket_cnt;
  new_bucket_table = realloc(hl->bucket_table, hl->bucket_cnt * sizeof(struct hash_bucket));
  if (new_bucket_table) {
    hl->bucket_table = new_bucket_table;
  } else {
    printf("func:%s line:%d error\n",__func__,__LINE__);
    return -1;
  }
  return 0;
}

void *hash_list_rehash(hash_t *self,int *ret)
{
  struct hash_list *hl = container_of(self, struct hash_list, table);
  if ((hl->rehash_bucket_base < hl->bucket_cnt) &&
    hl->rehash_upper_rate_num == 0) {
    *ret = hash_list_rehash_shrink(hl,self);
  } else {
    *ret = hash_list_rehash_expand(hl,self);
  }
  return NULL;
}

void *hash_list_query(hash_t *self, hash_key key, int *ret)
{
  struct hash_list *hl = container_of(self, struct hash_list, table);
  struct hash_bucket *bucket= NULL;
  list_t *item;
  void *retval;
  struct list_item *query_item;
  u64 hashval = hl->table.hash(self,key);
  int index = hashval % hl->bucket_cnt;
//  printf("query index:%d hashvale:%lx\n",index,hashval);
  bucket = hl->table.getvals(self, index);
  if (bucket == NULL) {
    *ret = -HASH_LIST_NOT_FOUND;
    return NULL;
  }
  *ret = 0;
  item = hl->search(hl, bucket, key);
  if (!item) {
    return NULL;
  } else {
    query_item = list_get(item);
    retval = query_item->val;
    return retval;
  }
}

void hash_list_info(hash_t *self)
{
  struct hash_list *hl = container_of(self, struct hash_list, table);
  int i;
  printf("bucket_cnt:%d\n",hl->bucket_cnt);
  printf("------------------------------\n");
  for (i = 0;i < hl->bucket_cnt;i++) {
      struct hash_bucket *bucket;
      bucket = hl->table.getvals(self,i);
      printf("BKTNR:%d BKT:%p NUM:%d\n",i,bucket,bucket->num);
      //hash_list_bucket_string_info(bucket);
  }
}

hash_t *hash_list_init(int rbt,int rlt, int *retval,
  enum hash_list_search_type type)
{
  struct hash_list *hl = malloc(sizeof(struct hash_list));
  struct hash_bucket *bucket;
  int i;
  if (hl == NULL) {
    *retval = -ENOMEM;
    return NULL;
  }
  if (rlt < 8) {
    *retval = -EINVAL;
    return NULL;
  }
  hl->rehash_bucket_base = rbt;
  hl->rehash_list_triger = rlt;
  hl->rehash_upper_rate_num = 0;
  hl->bucket_cnt = rbt;
  hl->table.cnt = 0;
  hl->table.insert = hash_list_insert;
  hl->table.delete = hash_list_delete;
  hl->table.query = hash_list_query;
  switch (type) {
    case HASH_LIST_SEARCH_INT:
      hl->table.getvals = NULL;
      hl->table.setvals = NULL;
      hl->table.hash = NULL;
      hl->search = NULL;
      hl->alloc_item = NULL;
      hl->free_item = NULL;
      break;
    case HASH_LIST_SEARCH_STRING:
      hl->table.getvals = hash_list_getvals;
      hl->table.setvals = NULL;
      hl->table.hash = X31_string_hash;
      hl->table.rehash = hash_list_rehash;
      hl->search = hash_list_search_string_item;
      hl->alloc_item = hash_list_alloc_item_string;
      hl->free_item = hash_list_free_string;
      break;
  }
  bucket = malloc(hl->bucket_cnt * sizeof(struct hash_bucket));
  if (bucket == NULL) {
    *retval = -ENOMEM;
    goto alloc_bucket_failed;
  }
  hl->bucket_table = bucket;
  for (i = 0;i < hl->bucket_cnt;i++) {
    list_init(&bucket[i].head);
    bucket[i].num = 0;
    list_set(&bucket[i].head,pointer_set(bucket[i].head.val));
  }
  *retval = 0;
  return &hl->table;
alloc_bucket_failed:
  free(hl);
  return NULL;
}
