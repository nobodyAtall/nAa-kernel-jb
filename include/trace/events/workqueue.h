#if !defined(_TRACE_WORKQUEUE_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_WORKQUEUE_H

#include <linux/workqueue.h>
#include <linux/sched.h>
#include <linux/tracepoint.h>

#undef TRACE_SYSTEM
#define TRACE_SYSTEM workqueue

TRACE_EVENT(worklet_enqueue,

	TP_PROTO(
		struct task_struct *wq_thread,
		struct work_struct *work,
		int cpu
	),

	TP_ARGS(wq_thread, work, cpu),

	TP_STRUCT__entry(
		__array(char,			thread_comm,	TASK_COMM_LEN)
		__field(pid_t,			thread_pid)
		__field(work_func_t,		func)
		__field(int,			cpu)
	),

	TP_fast_assign(
		memcpy(__entry->thread_comm, wq_thread->comm, TASK_COMM_LEN);
		__entry->thread_pid	= wq_thread->pid;
		__entry->func		= work->func;
		__entry->cpu		= cpu;
	),

	TP_printk("thread=%s:%d func=%pF cpu=%d", __entry->thread_comm,
		__entry->thread_pid, __entry->func, __entry->cpu)
);

TRACE_EVENT(worklet_enqueue_delayed,

	TP_PROTO(
		struct task_struct *wq_thread,
		struct work_struct *work,
		int cpu,
		unsigned long delay
	),

	TP_ARGS(wq_thread, work, cpu, delay),

	TP_STRUCT__entry(
		__array(char,			thread_comm,	TASK_COMM_LEN)
		__field(pid_t,			thread_pid)
		__field(work_func_t,		func)
		__field(int,			cpu)
		__field(unsigned long,		delay)
	),

	TP_fast_assign(
		memcpy(__entry->thread_comm, wq_thread->comm, TASK_COMM_LEN);
		__entry->thread_pid	= wq_thread->pid;
		__entry->func		= work->func;
		__entry->cpu		= cpu;
		__entry->delay		= delay;
	),

	TP_printk("thread=%s:%d func=%pF cpu=%d delay=%lu",
		__entry->thread_comm, __entry->thread_pid, __entry->func,
		__entry->cpu, __entry->delay)
);

TRACE_EVENT(worklet_execute,

	TP_PROTO(struct task_struct *wq_thread, struct work_struct *work),

	TP_ARGS(wq_thread, work),

	TP_STRUCT__entry(
		__array(char,			thread_comm,	TASK_COMM_LEN)
		__field(pid_t,			thread_pid)
		__field(struct work_struct *,	work)
		__field(work_func_t,		func)
	),

	TP_fast_assign(
		memcpy(__entry->thread_comm, wq_thread->comm, TASK_COMM_LEN);
		__entry->thread_pid	= wq_thread->pid;
		__entry->work		= work;
		__entry->func		= work->func;
	),

	TP_printk("thread=%s:%d work=%p func=%pF", __entry->thread_comm,
		__entry->thread_pid, __entry->work, __entry->func)
);

/* Declare work as void *, because we can't use work->... in after f(work) */
TRACE_EVENT(worklet_complete,
	TP_PROTO(struct task_struct *wq_thread, void *work),

	TP_ARGS(wq_thread, work),

	TP_STRUCT__entry(
		__array(char,	thread_comm, TASK_COMM_LEN)
		__field(pid_t,	thread_pid)
		__field(void *,	work)
	),

	TP_fast_assign(
		memcpy(__entry->thread_comm, wq_thread->comm, TASK_COMM_LEN);
		__entry->thread_pid	= wq_thread->pid;
		__entry->work		= work;
	),

	TP_printk("thread=%s:%d work=%p", __entry->thread_comm,
		  __entry->thread_pid, __entry->work)
);

TRACE_EVENT(worklet_cancel,

	TP_PROTO(struct work_struct *work),

	TP_ARGS(work),

	TP_STRUCT__entry(
		__field(work_func_t,	func)
	),

	TP_fast_assign(
		__entry->func	= work->func;
	),

	TP_printk("func=%pF", __entry->func)
);

/* Trace the creation of one workqueue thread on a cpu */
TRACE_EVENT(workqueue_creation,

	TP_PROTO(struct task_struct *wq_thread, int cpu),

	TP_ARGS(wq_thread, cpu),

	TP_STRUCT__entry(
		__array(char,	thread_comm,	TASK_COMM_LEN)
		__field(pid_t,	thread_pid)
		__field(int,	cpu)
	),

	TP_fast_assign(
		memcpy(__entry->thread_comm, wq_thread->comm, TASK_COMM_LEN);
		__entry->thread_pid	= wq_thread->pid;
		__entry->cpu		= cpu;
	),

	TP_printk("thread=%s:%d cpu=%d", __entry->thread_comm,
		__entry->thread_pid, __entry->cpu)
);

TRACE_EVENT(workqueue_flush,

	TP_PROTO(struct task_struct *wq_thread),

	TP_ARGS(wq_thread),

	TP_STRUCT__entry(
		__array(char,	thread_comm,	TASK_COMM_LEN)
		__field(pid_t,	thread_pid)
	),

	TP_fast_assign(
		memcpy(__entry->thread_comm, wq_thread->comm, TASK_COMM_LEN);
		__entry->thread_pid	= wq_thread->pid;
	),

	TP_printk("thread=%s:%d", __entry->thread_comm, __entry->thread_pid)
);

TRACE_EVENT(workqueue_destruction,

	TP_PROTO(struct task_struct *wq_thread, int cpu),

	TP_ARGS(wq_thread, cpu),

	TP_STRUCT__entry(
		__array(char,	thread_comm,	TASK_COMM_LEN)
		__field(pid_t,	thread_pid)
		__field(int,	cpu)
	),

	TP_fast_assign(
		memcpy(__entry->thread_comm, wq_thread->comm, TASK_COMM_LEN);
		__entry->thread_pid	= wq_thread->pid;
		__entry->cpu		= cpu;
	),

	TP_printk("thread=%s:%d", __entry->thread_comm, __entry->thread_pid)
);

#endif /* _TRACE_WORKQUEUE_H */

/* This part must be outside protection */
#include <trace/define_trace.h>
