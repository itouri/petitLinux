#include "fork.h"

//arch/i386/kernel/process.c
int sys_clone(struct pt_regs regs)
{
        unsigned long clone_flags;
        unsigned long newsp;
        int __user *parent_tidptr, *child_tidptr;

        clone_flags = regs.ebx;
        newsp = regs.ecx;
        parent_tidptr = (int __user *)regs.edx;
        child_tidptr = (int __user *)regs.edi;
        if (!newsp)
                newsp = regs.esp;
//#define CLONE_IDLETASK  0x00001000      /* set if new pid should be 0 (kernel only)*/ /include/linux/sched.h
        return do_fork(clone_flags & ~CLONE_IDLETASK, newsp, &regs, 0, parent_tidptr, child_tidptr);
        //             EBX                            ECX    ?         EDX            EDI
}

int sys_fork(struct pt_regs regs)
{                     //17
        return do_fork(SIGCHLD, regs.esp, &regs, 0, NULL, NULL);
}

//kernel/fork.c
//[p124]
long do_fork(unsigned long clone_flags,
              unsigned long stack_start,//子プロセスのユーザモードスタックポインタを指定
              struct pt_regs *regs,
              unsigned long stack_size, //常に0だってさ
              int __user *parent_tidptr,
              int __user *child_tidptr)
{
        struct task_struct *p;
        int trace = 0;
        long pid;

        if (unlikely(current->ptrace)) {
                trace = fork_traceflag (clone_flags);
                if (trace)
                        clone_flags |= CLONE_PTRACE;
        }

        p = copy_process(clone_flags, stack_start, regs, stack_size, parent_tidptr, child_tidptr);
        /*
         * Do this prior waking up the new thread - the thread pointer
         * might get invalid after that point, if the thread exits quickly.
         */
        pid = IS_ERR(p) ? PTR_ERR(p) : p->pid;

        if (!IS_ERR(p)) {
                struct completion vfork;

                if (clone_flags & CLONE_VFORK) {
                        p->vfork_done = &vfork;
                        init_completion(&vfork);
                }

                if ((p->ptrace & PT_PTRACED) || (clone_flags & CLONE_STOPPED)) {
                        /*
                         * We'll start up with an immediate SIGSTOP.
                         */
                        sigaddset(&p->pending.signal, SIGSTOP);
                        set_tsk_thread_flag(p, TIF_SIGPENDING);
                }

                p->state = TASK_STOPPED;
                if (!(clone_flags & CLONE_STOPPED))
                        wake_up_forked_process(p);      /* do this last */
                ++total_forks;

                if (unlikely (trace)) {
                        current->ptrace_message = pid;
                        ptrace_notify ((trace << 8) | SIGTRAP);
                }

                if (clone_flags & CLONE_VFORK) {
                        wait_for_completion(&vfork);
                        if (unlikely (current->ptrace & PT_TRACE_VFORK_DONE))
                                ptrace_notify ((PTRACE_EVENT_VFORK_DONE << 8) | SIGTRAP);
                } else
                        /*
                         * Let the child process run first, to avoid most of the
                         * COW overhead when the child exec()s afterwards.
                         */
                        set_need_resched();
        }
        return pid;
}
