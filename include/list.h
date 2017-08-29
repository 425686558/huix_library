#ifndef __LIST_H__
#define __LIST_H__
#include "common.h"
#include <stdlib.h>
#include <errno.h>
typedef void * list_object;
struct list;
typedef void* (*list_iter)(struct list *item,void *data);
typedef int (*list_search)(struct list *item,void *data);

typedef struct list {
  struct list *next;
  struct list *prev;
  list_object val;
} list_t;

#define list_for_each(pos, head) \
  for (pos = (head)->next; prefetch(pos->next), pos != (head); \
    pos = pos->next)

#define list_for_each_safe(pos, n, head) \
  for (pos = (head)->next, n = pos->next; pos != (head); \
    pos = n, n = pos->next)

extern int list_set(struct list *entry, list_object val);
extern list_object list_get(struct list *entry);
extern void list_init(struct list *list);
extern void list_add_next_point(struct list *new, struct list *point);
extern void list_add_prev_point(struct list *new, struct list *point);
extern int list_add_next_val(struct list *entry, list_object val);
extern int list_add_prev_val(struct list *entry, list_object val);
extern list_object list_del(struct list *entry);
extern void list_iter_func(struct list *point, list_iter func);
extern void *list_search_func(struct list *point, list_search func, void *data);
extern list_object list_replace_point(struct list *dst_entry, struct list *src_entry);
#endif
