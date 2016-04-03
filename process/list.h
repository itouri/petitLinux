#ifndef _LINUX_LIST_H
#define _LINUX_LIST_H

//listに関連することはプロセスに入れると関心の純度がさがる

//include/linux/list.h
//[p94]探索する上でとっても大事
struct list_head {
        struct list_head *next, *prev;
};

//[p95]リスト操作のマクロ
//add , add_tail , del , empty

//原文はこんな感じ
//なぜこんな書き方になっているか分かった。
//staticの中で変数宣言したくないからだ

static inline void __list_add(struct list_head *new,
                              struct list_head *prev,
                              struct list_head *next)
{
        next->prev = new;
        new->next = next;
        new->prev = prev;
        prev->next = new;
}

static inline void list_add(struct list_head *new, struct list_head *head)
{
        __list_add(new, head, head->next);
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
        __list_add(new, head->prev, head);
}

/* ????????
 * These are non-NULL pointers that will result in page faults
 * under normal circumstances, used to verify(確かめる) that nobody uses
 * non-initialized list entries.
 */
 //どこかでこの値を使ってないかチェックしてる? POISONは毒
#define LIST_POISON1  ((void *) 0x00100100)
#define LIST_POISON2  ((void *) 0x00200200)

static inline void __list_del(struct list_head * prev, struct list_head * next)
{
        next->prev = prev;
        prev->next = next;
}

static inline void list_del(struct list_head *entry)
{
        __list_del(entry->prev, entry->next);
        entry->next = LIST_POISON1;
        entry->prev = LIST_POISON2;
}

//わかりやすーい
static inline int list_empty(const struct list_head *head)
{
        return head->next == head;
}

#endif
