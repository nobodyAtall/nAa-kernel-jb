/*
 * Workqueue statistical tracer.
 *
 * Copyright (C) 2008 Frederic Weisbecker <fweisbec@gmail.com>
 *
 */


#include <trace/events/workqueue.h>
#include <linux/list.h>
#include <linux/percpu.h>
#include "trace_stat.h"
#include "trace.h"

/* A callback function saved in struct work_struct->func */
struct cpu_workqueue_stats;
struct workfunc_stats {
	struct list_head		list;
	struct cpu_workqueue_stats	*parent;

	/* Addr of worklet's callback function */
	work_func_t			func;

	/* Protected by cpu workqueue lock */
	unsigned int			inserted;
	unsigned int			executed;

	/*
	 * save latest work_struct's pointer to use as identifier in
	 * probe_worklet_complete, because we can't use work_struct->...
	 * after worklet got executed
	 */
	void				*work;

	/* save execution time temporarily for calculate executed time */
	u64				start_time;
	u64				max_executed_time;
	u64				total_time;
};

/* A cpu workqueue thread */
struct cpu_workqueue_stats {
	struct list_head	list;
	int			cpu;

	/* Protected by cpu workqueue lock */
	unsigned int		inserted;
	unsigned int		executed;
	/* list of struct workfunc_stats in this workqueue */
	struct list_head	workfunclist;

	/*
	 * the task maybe destroyed when we read stat file
	 * we define it to void * because we only use it as a identifier
	 */
	void			*task;
	int			pid;
	char			comm[TASK_COMM_LEN];
};

/* List of workqueue threads on one cpu */
struct workqueue_global_stats {
	struct list_head	list;
	spinlock_t		lock;
};

/* Don't need a global lock because allocated before the workqueues, and
 * never freed.
 */
static DEFINE_PER_CPU(struct workqueue_global_stats, all_workqueue_stat);
#define workqueue_cpu_stat(cpu) (&per_cpu(all_workqueue_stat, cpu))

/* To defer any workqueue freeing, we place them in this temporary list */
static LIST_HEAD(free_wq_list);
static DEFINE_SPINLOCK(free_wq_lock);

/* Number of readers in our stat file */
static int wq_file_ref;

/*
 * Update record when insert a work into workqueue
 * Caller need to hold cpu_workqueue_stats spin_lock
 */
int do_worklet_enqueue(struct cpu_workqueue_stats *cws,
			 struct work_struct *work)
{
	struct workfunc_stats *wfstat;

	cws->inserted++;

	list_for_each_entry(wfstat, &cws->workfunclist, list)
		if (wfstat->func == work->func)
			goto found;

	/* not found, first insert */

	/* work can be inserted from interrupt */
	wfstat = kzalloc(sizeof(struct workfunc_stats), GFP_ATOMIC);
	if (!wfstat)
		return -ENOMEM;

	wfstat->parent = cws;
	wfstat->func = work->func;

	list_add_tail(&wfstat->list, &cws->workfunclist);
found:
	wfstat->inserted++;

	return 0;
}

/* Enqueue of a work */
static void
probe_worklet_enqueue(struct task_struct *wq_thread, struct work_struct *work,
		      int cpu)
{
	int wqcpu = cpumask_first(&wq_thread->cpus_allowed);
	struct cpu_workqueue_stats *node;
	unsigned long flags;

	spin_lock_irqsave(&workqueue_cpu_stat(wqcpu)->lock, flags);
	list_for_each_entry(node, &workqueue_cpu_stat(wqcpu)->list, list) {
		if (node->task == wq_thread) {
			/* we ignore error of do_worklet_insertion */
			do_worklet_enqueue(node, work);
			goto found;
		}
	}
	pr_debug("trace_workqueue: entry not found\n");
found:
	spin_unlock_irqrestore(&workqueue_cpu_stat(wqcpu)->lock, flags);
}

static void
probe_worklet_enqueue_delayed(struct task_struct *wq_thread,
			      struct work_struct *work, int cpu,
			      unsigned long delay)
{
	probe_worklet_enqueue(wq_thread, work, cpu);
}

/* Execution of a work */
static void
probe_worklet_execute(struct task_struct *wq_thread, struct work_struct *work)
{
	int cpu = cpumask_first(&wq_thread->cpus_allowed);
	struct cpu_workqueue_stats *node;
	struct workfunc_stats *wfnode;
	unsigned long flags;

	spin_lock_irqsave(&workqueue_cpu_stat(cpu)->lock, flags);

	list_for_each_entry(node, &workqueue_cpu_stat(cpu)->list, list)
		if (node->task == wq_thread) {
			node->executed++;
			goto found_wq;
		}
	pr_debug("trace_workqueue: workqueue not found\n");
	goto end;

found_wq:
	list_for_each_entry(wfnode, &node->workfunclist, list)
		if (wfnode->func == work->func) {
			wfnode->executed++;
			wfnode->start_time = trace_clock_global();
			wfnode->work = work;
			goto found_wf;
		}
	pr_debug("trace_workqueue: worklet not found\n");
	goto end;

found_wf:
end:
	spin_unlock_irqrestore(&workqueue_cpu_stat(cpu)->lock, flags);
}

/* Complete of a work */
static void
probe_worklet_complete(struct task_struct *wq_thread, void *work)
{
	int cpu = cpumask_first(&wq_thread->cpus_allowed);
	struct cpu_workqueue_stats *node;
	struct workfunc_stats *wfnode;
	unsigned long flags;

	spin_lock_irqsave(&workqueue_cpu_stat(cpu)->lock, flags);

	list_for_each_entry(node, &workqueue_cpu_stat(cpu)->list, list)
		if (node->task == wq_thread)
			goto found_wq;
	pr_debug("trace_workqueue: workqueue not found\n");
	goto end;

found_wq:
	list_for_each_entry(wfnode, &node->workfunclist, list) {
		u64 executed_time;

		if (wfnode->work != work)
			continue;

		executed_time = trace_clock_global() - wfnode->start_time;
		wfnode->total_time += executed_time;
		if (executed_time > wfnode->max_executed_time)
			wfnode->max_executed_time = executed_time;
		goto found_wf;
	}
	pr_debug("trace_workqueue: worklet not found\n");
	goto end;

found_wf:
end:
	spin_unlock_irqrestore(&workqueue_cpu_stat(cpu)->lock, flags);
}

/* Creation of a cpu workqueue thread */
static void probe_workqueue_creation(struct task_struct *wq_thread, int cpu)
{
	struct cpu_workqueue_stats *cws;
	struct workfunc_stats *wfstat;
	unsigned long flags;

	WARN_ON(cpu < 0);

	/* Workqueues are sometimes created in atomic context */
	cws = kzalloc(sizeof(struct cpu_workqueue_stats), GFP_ATOMIC);
	if (!cws) {
		pr_warning("trace_workqueue: not enough memory\n");
		goto err_alloc_cws;
	}
	INIT_LIST_HEAD(&cws->list);
	cws->cpu = cpu;

	cws->task = wq_thread;
	cws->pid = wq_thread->pid;
	strncpy(cws->comm, wq_thread->comm, TASK_COMM_LEN);
	INIT_LIST_HEAD(&cws->workfunclist);

	/*
	 * add a dummy node to cpu_workqueue_stats->workfunclist to:
	 * 1: help print function to print header of work(ex, workqueue info)
	 * 2: used as a seq iter if no work exist in workqueue
	 */
	wfstat = kzalloc(sizeof(struct workfunc_stats), GFP_ATOMIC);
	if (!wfstat) {
		pr_warning("trace_workqueue: not enough memory\n");
		goto err_alloc_wfstat;
	}
	wfstat->parent = cws;
	list_add_tail(&wfstat->list, &cws->workfunclist);

	spin_lock_irqsave(&workqueue_cpu_stat(cpu)->lock, flags);
	list_add_tail(&cws->list, &workqueue_cpu_stat(cpu)->list);
	spin_unlock_irqrestore(&workqueue_cpu_stat(cpu)->lock, flags);

	return;

err_alloc_wfstat:
	kfree(cws);
err_alloc_cws:
	return;
}

static void free_workqueue_stats(struct cpu_workqueue_stats *stat)
{
	struct workfunc_stats *wfstat, *next;

	list_for_each_entry_safe(wfstat, next, &stat->workfunclist, list) {
			list_del(&wfstat->list);
			kfree(wfstat);
	}
	kfree(stat);
}

/* Destruction of a cpu workqueue thread */
static void probe_workqueue_destruction(struct task_struct *wq_thread, int cpu)
{
	struct cpu_workqueue_stats *node;
	unsigned long flags;

	spin_lock_irqsave(&workqueue_cpu_stat(cpu)->lock, flags);
	list_for_each_entry(node, &workqueue_cpu_stat(cpu)->list, list) {

		if (node->task != wq_thread)
			continue;

		list_del(&node->list);

		/*
		 * We actually defer this workqueue freeing and
		 * its worklets until no more readers are present on our
		 * stat file. We are in atomic context here and can't wait
		 * for the file and the previous copied entries that point
		 * to this workqueue to be released.
		 */
		spin_lock(&free_wq_lock);
		if (!wq_file_ref)
			free_workqueue_stats(node);
		else
			list_add_tail(&node->list, &free_wq_list);
		spin_unlock(&free_wq_lock);

		goto found;
	}

	pr_debug("trace_workqueue: don't find workqueue to destroy\n");
found:
	spin_unlock_irqrestore(&workqueue_cpu_stat(cpu)->lock, flags);

}

static struct workfunc_stats *workqueue_stat_start_cpu(int cpu)
{
	unsigned long flags;
	struct workfunc_stats *ret = NULL;

	spin_lock_irqsave(&workqueue_cpu_stat(cpu)->lock, flags);

	if (!list_empty(&workqueue_cpu_stat(cpu)->list)) {
		struct cpu_workqueue_stats *cws;
		cws = list_entry(workqueue_cpu_stat(cpu)->list.next,
			struct cpu_workqueue_stats, list);
		/*
		 * cpu_workqueue_stats->workfunclist at least have a dummy node
		 */
		ret = list_entry(cws->workfunclist.next, struct workfunc_stats,
			list);
	}

	spin_unlock_irqrestore(&workqueue_cpu_stat(cpu)->lock, flags);

	return ret;
}

static void *workqueue_stat_start(struct tracer_stat *trace)
{
	int cpu;
	void *ret = NULL;

	for_each_possible_cpu(cpu) {
		ret = workqueue_stat_start_cpu(cpu);
		if (ret)
			return ret;
	}
	return NULL;
}

static void *workqueue_stat_next(void *prev, int idx)
{
	struct workfunc_stats *prev_wfstat = prev;
	int cpu = prev_wfstat->parent->cpu;
	unsigned long flags;
	void *ret = NULL;

	spin_lock_irqsave(&workqueue_cpu_stat(cpu)->lock, flags);

	if (!list_is_last(&prev_wfstat->list,
	    &prev_wfstat->parent->workfunclist)) {
		ret = list_entry(prev_wfstat->list.next, struct workfunc_stats,
			list);
		spin_unlock_irqrestore(&workqueue_cpu_stat(cpu)->lock, flags);
		return ret;
	}

	if (!list_is_last(&prev_wfstat->parent->list,
	    &workqueue_cpu_stat(cpu)->list)) {
		struct cpu_workqueue_stats *cws = list_entry(
			prev_wfstat->parent->list.next,
			struct cpu_workqueue_stats, list);
		ret = list_entry(cws->workfunclist.next, struct workfunc_stats,
			list);
		spin_unlock_irqrestore(&workqueue_cpu_stat(cpu)->lock, flags);
		return ret;
	}

	spin_unlock_irqrestore(&workqueue_cpu_stat(cpu)->lock, flags);

	do {
		cpu = cpumask_next(cpu, cpu_possible_mask);
		if (cpu >= nr_cpu_ids)
			return NULL;
	} while (!(ret = workqueue_stat_start_cpu(cpu)));

	return ret;
}

static int workqueue_stat_show(struct seq_file *s, void *p)
{
	struct workfunc_stats *wfstat = p;
	struct cpu_workqueue_stats *cws = wfstat->parent;

	if (!wfstat->func) {
		/* It is first dummy node, need to print workqueue info */
		seq_printf(s, "  %3d   %6d   %6u                     %s:%d\n",
			   cws->cpu,
			   cws->inserted,
			   cws->executed,
			   cws->comm,
			   cws->pid);
	} else {
		/* It is effect node, need to print workfunc info */
		int lastwf = list_is_last(&wfstat->list, &cws->workfunclist);
		unsigned long long avg_time;

		if (wfstat->executed) {
			avg_time = wfstat->total_time;
			do_div(avg_time, wfstat->executed);
			avg_time = ns2usecs(avg_time);
		} else {
			avg_time = 0;
		}

		seq_printf(s, "  %3d   %6d   %6u   %6llu   %6llu   %c-%pF\n",
			cws->cpu,
			wfstat->inserted,
			wfstat->executed,
			ns2usecs(wfstat->max_executed_time),
			avg_time,
			lastwf ? '`' : '|',
			wfstat->func);
	}

	return 0;
}

/*
 * Here we are sure that we have no more readers on our stat file
 * and that further readers will block until we return from this function.
 * We can then safely free these pending entries
 */
static void workqueue_stat_file_release(void)
{
	unsigned long flags;

	spin_lock_irqsave(&free_wq_lock, flags);

	if (!--wq_file_ref) {
		struct cpu_workqueue_stats *node, *next;

		list_for_each_entry_safe(node, next, &free_wq_list, list) {
			list_del(&node->list);
			free_workqueue_stats(node);
		}
	}

	spin_unlock_irqrestore(&free_wq_lock, flags);
};

static void workqueue_stat_file_open(void)
{
	unsigned long flags;

	spin_lock_irqsave(&free_wq_lock, flags);
	wq_file_ref++;
	spin_unlock_irqrestore(&free_wq_lock, flags);
}

		/**/
static int workqueue_stat_headers(struct seq_file *s)
{
	seq_printf(s, "# CPU INSERTED EXECUTED    MAX us   AVG us"
			"   TASKNAME:PID\n");
	seq_printf(s, "#  |     |        |           |      ` |"
			"     -WORKFUNC\n");
	seq_printf(s, "#  |     |        |           |        |          |\n");
	return 0;
}

struct tracer_stat workqueue_stats __read_mostly = {
	.name		 = "workqueues",
	.stat_start	 = workqueue_stat_start,
	.stat_next	 = workqueue_stat_next,
	.stat_show	 = workqueue_stat_show,
	.stat_headers	 = workqueue_stat_headers,
	.file_open	 = workqueue_stat_file_open,
	.file_open	 = workqueue_stat_file_release,
};


int __init stat_workqueue_init(void)
{
	if (register_stat_tracer(&workqueue_stats)) {
		pr_warning("Unable to register workqueue stat tracer\n");
		return 1;
	}

	return 0;
}
fs_initcall(stat_workqueue_init);

/*
 * Workqueues are created very early, just after pre-smp initcalls.
 * So we must register our tracepoints at this stage.
 */
int __init trace_workqueue_early_init(void)
{
	int ret, cpu;

	ret = register_trace_worklet_enqueue(probe_worklet_enqueue);
	if (ret)
		goto out;

	ret = register_trace_worklet_enqueue_delayed(
		probe_worklet_enqueue_delayed);
	if (ret)
		goto out_worklet_enqueue;

	ret = register_trace_worklet_execute(probe_worklet_execute);
	if (ret)
		goto out_worklet_enqueue_delayed;

	ret = register_trace_worklet_complete(probe_worklet_complete);
	if (ret)
		goto out_worklet_execute;

	ret = register_trace_workqueue_creation(probe_workqueue_creation);
	if (ret)
		goto out_worklet_complete;

	ret = register_trace_workqueue_destruction(probe_workqueue_destruction);
	if (ret)
		goto out_workqueue_creation;

	for_each_possible_cpu(cpu) {
		spin_lock_init(&workqueue_cpu_stat(cpu)->lock);
		INIT_LIST_HEAD(&workqueue_cpu_stat(cpu)->list);
	}

	return 0;

out_workqueue_creation:
	unregister_trace_workqueue_creation(probe_workqueue_creation);
out_worklet_complete:
	unregister_trace_worklet_complete(probe_worklet_complete);
out_worklet_execute:
	unregister_trace_worklet_execute(probe_worklet_execute);
out_worklet_enqueue_delayed:
	unregister_trace_worklet_enqueue_delayed(probe_worklet_enqueue_delayed);
out_worklet_enqueue:
	unregister_trace_worklet_enqueue(probe_worklet_enqueue);
out:
	pr_warning("trace_workqueue: unable to trace workqueues\n");

	return 1;
}
early_initcall(trace_workqueue_early_init);
