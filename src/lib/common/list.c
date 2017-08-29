#include "list.h"
int list_set(struct list *entry, list_object val)
{
	entry->val = val;
	return 0;
}

list_object list_get(struct list *entry)
{
	return entry->val;
}

void list_init(struct list *list)
{
	list->next = list;
	list->prev = list;
}

list_object list_replace_point(struct list *dst_entry, struct list *src_entry)
{
  list_t *src_next = src_entry->next;
  list_t *src_prev = src_entry->prev;
  dst_entry->next = src_next;
  dst_entry->prev = src_prev;
  src_next->prev = dst_entry;
  src_prev->next = dst_entry;
  dst_entry->val = src_entry->val;
  src_entry->next = NULL;
  src_entry->prev = NULL;
  return src_entry->val;
}

void list_add_next_point(struct list *new, struct list *point)
{
	struct list *next_point = point->next;
	new->next = next_point;
	new->prev = point;
	next_point->prev = new;
	point->next = new;
}

void list_add_prev_point(struct list *new, struct list *point)
{
	struct list *prev_point = point->prev;
	new->next = point;
	new->prev = prev_point;
	prev_point->next = new;
	point->prev = new;
}

int list_add_next_val(struct list *entry, list_object val)
{
	struct list *next = malloc(sizeof(struct list));
	if (next == NULL)
		return -ENOMEM;
	next->val = val;
	list_add_next_point(next, entry);
	return 0;
}

int list_add_prev_val(struct list *entry, list_object val)
{
	struct list *prev = malloc(sizeof(struct list));
	if (prev == NULL)
		return -ENOMEM;
	prev->val = val;
	list_add_prev_point(prev, entry);
	return 0;
}

list_object list_del(struct list *entry)
{
	struct list *next = entry->next;
	struct list *prev = entry->prev;
	next->prev = prev;
	prev->next = next;
	entry->next = NULL;
	entry->prev = NULL;
	return entry->val;
}

void list_iter_func(struct list *point, list_iter func)
{
  struct list *pos = point->next;
  struct list *next = pos->next;
  void *ret = NULL;
  ret = func(point,ret);
  for (;pos != point;pos = next, next = next->next) {
    ret = func(pos,ret);
  }
}

void *list_search_func(struct list *point, list_search func, void *data)
{
  struct list *pos = point->next;
  struct list *next = pos->next;
  if (!func(point, data))
    return point;
  
  for (;pos != point;pos = next, next = next->next) {
    if(!func(pos, data))
      return pos;
  }
  return NULL;
}
