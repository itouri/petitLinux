#ifndef _LINUX_PROCESS_H
#define _LINUX_PROCESS_H

#include "list.h"

//include/asm-i386/thread_info.h
//[p94]現在CPUで動作しているプロセスのthread_infoを取得
/* how to get the thread information struct from C */
static inline struct thread_info *current_thread_info(void)
{
        struct thread_info *ti;
        __asm__("andl %%esp,%0; ":"=r" (ti) : "0" (~8191UL));
        // movl $0xffffe000 , %ecx
        // andl %esp , %ecx
        // movl %ecx , p
        return ti; //なぜ構造体にreturnがあるんだ?
}

//include/linux/sched.h
//task_structのstateの定数たち
#define TASK_RUNNING            0	//実行中
#define TASK_INTERRUPTIBLE      1	//何かを待っている シグナルでRUNNNIGへ
#define TASK_UNINTERRUPTIBLE    2	//何かを待っている　シグナルでもRUNしない
#define TASK_STOPPED            4	//
#define TASK_ZOMBIE             8
#define TASK_DEAD               16

//include/asm-i386/thread_info.h
struct thread_info {
        struct task_struct      *task;          /* main task structure */
//        struct exec_domain      *exec_domain;   /* execution domain */
        unsigned long           flags;          /* low level flags */
        unsigned long           status;         /* thread-synchronous flags */
};

//include/linux/sched.h
//[p94]
union thread_union {
        struct thread_info thread_info;
        //unsigned long stack[INIT_THREAD_SIZE/sizeof(long)];
        unsigned long stack[2048];
};

extern union thread_union init_thread_union;

//include/linux/sched.h
//[p91]指定したプロセスの状態を設定
#define __set_task_state(tsk, state_value)              \
        do { (tsk)->state = (state_value); } while (0)
//TODO #define set_task_state(tsk, state_value)                \
//         set_mb((tsk)->state, (state_value))
        //set_mbがsystem.hに2つある。どっちが呼ばれている?

//[p91]現在実行中のプロセスの状態を設定
#define __set_current_state(state_value)                        \
        do { current->state = (state_value); } while (0)
//TODO #define set_current_state(state_value)          \
//         /set_mb(current->state, (state_value))

//include/linux/sched.h
struct prio_array {
        //unsigned long bitmap[BITMAP_SIZE];
        //struct list_head queue[MAX_PRIO];

        int nr_active; //リンクにリストされたプロセスディスクリプタの数
        unsigned long bitmap[200]; //
        struct list_head queue[3]; //優先度ごとのリスト
};

typedef struct prio_array prio_array_t;

//include/linux/sched.h
//[p89]プロセスディスクリプタ
struct task_struct {
        volatile long state;    /* -1 unrunnable, 0 runnable, >0 stopped */
        struct thread_info *thread_info;

        struct list_head tasks;
        struct list_head ptrace_children;
        struct list_head ptrace_list;

        //対象が1つ→task_struct 対象が複数→list_head
        struct task_struct *real_parent; /* real parent process (when being debugged) */
        struct task_struct *parent;     /* parent process */
        struct list_head children;      /* list of my children */
        struct list_head sibling;       /* linkage in my parent's children list */
        struct task_struct *group_leader;       /* threadgroup leader */

// dequeue_task , enqueue_taskで使用
        struct list_head run_list;
        int prio;
        prio_array_t *array;

        int pid; //pid_t pid;だけどi386では結局int
        
};


typedef struct task_struct task_t;

//[p98]
static inline void dequeue_task(struct task_struct *p, prio_array_t *array)
{
        array->nr_active--;
        list_del(&p->run_list);
        if (list_empty(array->queue + p->prio))
                __clear_bit(p->prio, array->bitmap);
}

//[p98]
static inline void enqueue_task(struct task_struct *p, prio_array_t *array)
{
        list_add_tail(&p->run_list, array->queue + p->prio);
        __set_bit(p->prio, array->bitmap);
        array->nr_active++;
        p->array = array;
}


#endif