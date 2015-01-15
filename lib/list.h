//
// list.h
// vm-persistence
//
// Jinglei Ren <jinglei@ren.systems>
//

#ifndef _CPP_LINUX_LIST_H
#define _CPP_LINUX_LIST_H

#include <assert.h>

/*
 * Architectures might want to move the poison pointer offset
 * into some well-recognized area such as 0xdead000000000000,
 * that is also not mappable by user-space exploits:
 */
#ifdef CONFIG_ILLEGAL_POINTER_VALUE
# define POISON_POINTER_DELTA _AC(CONFIG_ILLEGAL_POINTER_VALUE, UL)
#else
# define POISON_POINTER_DELTA 0
#endif

/*
 * These are non-NULL pointers that will result in page faults
 * under normal circumstances, used to verify that nobody uses
 * non-initialized list entries.
 */
#define LIST_POISON1  ((void *)(0x00100100 + POISON_POINTER_DELTA))
#define LIST_POISON2  ((void *)(0x00200200 + POISON_POINTER_DELTA))

namespace slib {

struct list_head {
  struct list_head *next, *prev;
};

struct hlist_head {
  struct hlist_node *first;
};

struct hlist_node {
  struct hlist_node *next, **pprev;
};

template<class P, class M>
size_t offset_of(const M P::*member)
{
  return (size_t) &(reinterpret_cast<P*>(0)->*member);
}

template<class P, class M>
P* container_of(M* ptr, const M P::*member)
{
  return (P*)((char*)ptr - offset_of(member));
}

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct list_head *list)
{
  list->next = list;
  list->prev = list;
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_add(struct list_head *ne,
                              struct list_head *prev,
                              struct list_head *next)
{
  next->prev = ne;
  ne->next = next;
  ne->prev = prev;
  prev->next = ne;
}

/**
 * list_add - add a new entry
 * @ne: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void list_add(struct list_head *ne, struct list_head *head)
{
  __list_add(ne, head, head->next);
}


/**
 * list_add_tail - add a new entry
 * @ne: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void list_add_tail(struct list_head *ne, struct list_head *head)
{
  __list_add(ne, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_del(struct list_head *prev, struct list_head *next)
{
  // Prevents write skew
  assert(prev->next == next->prev);
  prev->next->prev = NULL;
  prev->next->next = NULL;

  next->prev = prev;
  prev->next = next;
}

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */
static inline void __list_del_entry(struct list_head *entry)
{
  __list_del(entry->prev, entry->next);
}

static inline void list_del(struct list_head *entry)
{
  __list_del(entry->prev, entry->next);
  entry->next = (struct list_head *)LIST_POISON1;
  entry->prev = (struct list_head *)LIST_POISON2;
}

/**
 * list_replace - replace old entry by new one
 * @old : the element to be replaced
 * @ne : the new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static inline void list_replace(struct list_head *old,
                                struct list_head *ne)
{
  ne->next = old->next;
  ne->next->prev = ne;
  ne->prev = old->prev;
  ne->prev->next = ne;

  old->prev = old->next = NULL; // Prevents write skew
}

static inline void list_replace_init(struct list_head *old,
                                     struct list_head *ne)
{
  list_replace(old, ne);
  INIT_LIST_HEAD(old);
}

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static inline void list_del_init(struct list_head *entry)
{
  __list_del_entry(entry);
  INIT_LIST_HEAD(entry);
}

/**
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void list_move(struct list_head *list, struct list_head *head)
{
  __list_del_entry(list);
  list_add(list, head);
}

/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static inline void list_move_tail(struct list_head *list,
                                  struct list_head *head)
{
  __list_del_entry(list);
  list_add_tail(list, head);
}

/**
 * list_is_last - tests whether @list is the last entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
static inline int list_is_last(const struct list_head *list,
                               const struct list_head *head)
{
  return list->next == head;
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int list_empty(const struct list_head *head)
{
  return head->next == head;
}

/**
 * list_empty_careful - tests whether a list is empty and not being modified
 * @head: the list to test
 *
 * Description:
 * tests whether a list is empty _and_ checks that no other CPU might be
 * in the process of modifying either member (next or prev)
 *
 * NOTE: using list_empty_careful() without synchronization
 * can only be safe if the only activity that can happen
 * to the list entry is list_del_init(). Eg. it cannot be used
 * if another CPU could re-list_add() it.
 */
static inline int list_empty_careful(const struct list_head *head)
{
  struct list_head *next = head->next;
  return (next == head) && (next == head->prev);
}

/**
 * list_rotate_left - rotate the list to the left
 * @head: the head of the list
 */
static inline void list_rotate_left(struct list_head *head)
{
  struct list_head *first;
  
  if (!list_empty(head)) {
    first = head->next;
    list_move_tail(first, head);
  }
}

/**
 * list_is_singular - tests whether a list has just one entry.
 * @head: the list to test.
 */
static inline int list_is_singular(const struct list_head *head)
{
  return !list_empty(head) && (head->next == head->prev);
}

static inline void __list_cut_position(struct list_head *list,
                                       struct list_head *head, struct list_head *entry)
{
  struct list_head *new_first = entry->next;
  list->next = head->next;
  list->next->prev = list;
  list->prev = entry;
  entry->next = list;
  head->next = new_first;
  new_first->prev = head;
}

/**
 * list_cut_position - cut a list into two
 * @list: a new list to add all removed entries
 * @head: a list with entries
 * @entry: an entry within head, could be the head itself
 *	and if so we won't cut the list
 *
 * This helper moves the initial part of @head, up to and
 * including @entry, from @head to @list. You should
 * pass on @entry an element you know is on @head. @list
 * should be an empty list or a list you do not care about
 * losing its data.
 *
 */
static inline void list_cut_position(struct list_head *list,
                                     struct list_head *head, struct list_head *entry)
{
  if (list_empty(head))
    return;
  if (list_is_singular(head) &&
      (head->next != entry && head != entry))
    return;
  if (entry == head)
    INIT_LIST_HEAD(list);
  else
    __list_cut_position(list, head, entry);
}

static inline void __list_splice(const struct list_head *list,
                                 struct list_head *prev,
                                 struct list_head *next)
{
  struct list_head *first = list->next;
  struct list_head *last = list->prev;
  
  first->prev = prev;
  prev->next = first;
  
  last->next = next;
  next->prev = last;
}

/**
 * list_splice - join two lists, this is designed for stacks
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void list_splice(const struct list_head *list,
                               struct list_head *head)
{
  if (!list_empty(list))
    __list_splice(list, head, head->next);
}

/**
 * list_splice_tail - join two lists, each list being a queue
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void list_splice_tail(struct list_head *list,
                                    struct list_head *head)
{
  if (!list_empty(list))
    __list_splice(list, head->prev, head);
}

/**
 * list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
static inline void list_splice_init(struct list_head *list,
                                    struct list_head *head)
{
  if (!list_empty(list)) {
    __list_splice(list, head, head->next);
    INIT_LIST_HEAD(list);
  }
}

/**
 * list_splice_tail_init - join two lists and reinitialise the emptied list
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * Each of the lists is a queue.
 * The list at @list is reinitialised
 */
static inline void list_splice_tail_init(struct list_head *list,
                                         struct list_head *head)
{
  if (!list_empty(list)) {
    __list_splice(list, head->prev, head);
    INIT_LIST_HEAD(list);
  }
}

/**
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head) \
for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * list_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each_prev(pos, head) \
for (pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_safe(pos, n, head) \
for (pos = (head)->next, n = pos->next; pos != (head); \
pos = n, n = pos->next)

/**
 * list_for_each_prev_safe - iterate over a list backwards safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_prev_safe(pos, n, head) \
for (pos = (head)->prev, n = pos->prev; \
pos != (head); \
pos = n, n = pos->prev)

/*
 * Double linked lists with a single pointer list head.
 * Mostly useful for hash tables where the two pointer list head is
 * too wasteful.
 * You lose the ability to access the tail in O(1).
 */

#define HLIST_HEAD_INIT { .first = NULL }
#define HLIST_HEAD(name) struct hlist_head name = {  .first = NULL }
#define INIT_HLIST_HEAD(ptr) ((ptr)->first = NULL)
static inline void INIT_HLIST_NODE(struct hlist_node *h)
{
  h->next = NULL;
  h->pprev = NULL;
}

static inline int hlist_unhashed(const struct hlist_node *h)
{
  return !h->pprev;
}

static inline int hlist_empty(const struct hlist_head *h)
{
  return !h->first;
}

static inline void __hlist_del(struct hlist_node *n)
{
  struct hlist_node *next = n->next;
  struct hlist_node **pprev = n->pprev;
  *pprev = next;
  if (next)
    next->pprev = pprev;
  // Prevents write skew
  n->next = NULL;
  n->pprev = NULL;
}

static inline void hlist_del(struct hlist_node *n)
{
  __hlist_del(n);
  n->next = (struct hlist_node *)LIST_POISON1;
  n->pprev = (struct hlist_node **)LIST_POISON2;
}

static inline void hlist_del_init(struct hlist_node *n)
{
  if (!hlist_unhashed(n)) {
    __hlist_del(n);
    INIT_HLIST_NODE(n);
  }
}

static inline void hlist_add_head(struct hlist_node *n, struct hlist_head *h)
{
  struct hlist_node *first = h->first;
  n->next = first;
  if (first)
    first->pprev = &n->next;
  h->first = n;
  n->pprev = &h->first;
}

/* next must be != NULL */
static inline void hlist_add_before(struct hlist_node *n,
                                    struct hlist_node *next)
{
  n->pprev = next->pprev;
  n->next = next;
  next->pprev = &n->next;
  *(n->pprev) = n;
}

static inline void hlist_add_behind(struct hlist_node *n,
                                    struct hlist_node *prev)
{
  n->next = prev->next;
  prev->next = n;
  n->pprev = &prev->next;
  
  if (n->next)
    n->next->pprev  = &n->next;
}

/* after that we'll appear to be on some hlist and hlist_del will work */
static inline void hlist_add_fake(struct hlist_node *n)
{
  n->pprev = &n->next;
}

/*
 * Move a list from one list head to another. Fixup the pprev
 * reference of the first entry if it exists.
 */
static inline void hlist_move_list(struct hlist_head *old,
                                   struct hlist_head *ne)
{
  ne->first = old->first;
  if (ne->first)
    ne->first->pprev = &ne->first;
  old->first = NULL;
}

#define hlist_for_each(pos, head) \
for (pos = (head)->first; pos ; pos = pos->next)

#define hlist_for_each_safe(pos, n, head) \
for (pos = (head)->first; pos && ({ n = pos->next; 1; }); \
pos = n)

} // namespace slib

#endif // _CPP_LINUX_LIST_H

