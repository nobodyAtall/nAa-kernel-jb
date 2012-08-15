/*
 * Trace mm pages
 *
 * Copyright (C) 2009 Red Hat Inc, Steven Rostedt <srostedt@redhat.com>
 *
 * Code based on Matt Mackall's /proc/[kpagecount|kpageflags] code.
 */
#include <linux/module.h>
#include <linux/bootmem.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>

#include "trace_output.h"

//#define CREATE_TRACE_POINTS
#include <trace/events/mm.h>

void trace_read_page_frames(unsigned long start, unsigned long end,
			    void (*trace)(unsigned long pfn, struct page *page))
{
	unsigned long pfn = start;
	struct page *page;

	if (start > max_pfn - 1)
		return;

	if (end > max_pfn - 1)
		end = max_pfn - 1;

	while (pfn < end) {
		page = NULL;
		if (pfn_valid(pfn))
			page = pfn_to_page(pfn);
		pfn++;
		if (page)
			trace(pfn, page);
	}
}

static void trace_do_dump_pages(unsigned long pfn, struct page *page)
{
	trace_dump_pages(pfn, page);
}

static ssize_t
trace_mm_dump_range_read(struct file *filp, char __user *ubuf, size_t cnt,
		 loff_t *ppos)
{
	return simple_read_from_buffer(ubuf, cnt, ppos, "0\n", 2);
}


static ssize_t
trace_mm_dump_range_write(struct file *filp, const char __user *ubuf, size_t cnt,
		       loff_t *ppos)
{
	unsigned long val, start, end;
	char buf[64];
	int ret;

	if (cnt >= sizeof(buf))
		return -EINVAL;

	if (copy_from_user(&buf, ubuf, cnt))
		return -EFAULT;

	if (tracing_update_buffers() < 0)
		return -ENOMEM;

	if (trace_set_clr_event("mm", "dump_pages", 1))
		return -EINVAL;

	buf[cnt] = 0;

	ret = strict_strtol(buf, 10, &val);
	if (ret < 0)
		return ret;

	start = *ppos;
	if (val < 0)
		end = max_pfn - 1;
	else
		end = start + val;

	trace_read_page_frames(start, end, trace_do_dump_pages);

	*ppos += cnt;

	return cnt;
}

static const struct file_operations trace_mm_fops = {
	.open		= tracing_open_generic,
	.read		= trace_mm_dump_range_read,
	.write		= trace_mm_dump_range_write,
};

/* move this into trace_objects.c when that file is created */
static struct dentry *trace_objects_dir(void)
{
	static struct dentry *d_objects;
	struct dentry *d_tracer;

	if (d_objects)
		return d_objects;

	d_tracer = tracing_init_dentry();
	if (!d_tracer)
		return NULL;

	d_objects = debugfs_create_dir("objects", d_tracer);
	if (!d_objects)
		pr_warning("Could not create debugfs "
			   "'objects' directory\n");

	return d_objects;
}


static struct dentry *trace_objects_mm_dir(void)
{
	static struct dentry *d_mm;
	struct dentry *d_objects;

	if (d_mm)
		return d_mm;

	d_objects = trace_objects_dir();
	if (!d_objects)
		return NULL;

	d_mm = debugfs_create_dir("mm", d_objects);
	if (!d_mm)
		pr_warning("Could not create 'objects/mm' directory\n");

	return d_mm;
}

static struct dentry *trace_objects_mm_pages_dir(void)
{
	static struct dentry *d_pages;
	struct dentry *d_mm;

	if (d_pages)
		return d_pages;

	d_mm = trace_objects_mm_dir();
	if (!d_mm)
		return NULL;

	d_pages = debugfs_create_dir("pages", d_mm);
	if (!d_pages)
		pr_warning("Could not create debugfs "
			   "'objects/mm/pages' directory\n");

	return d_pages;
}

static __init int trace_objects_mm_init(void)
{
	struct dentry *d_pages;

	d_pages = trace_objects_mm_pages_dir();
	if (!d_pages)
		return 0;

	trace_create_file("dump_range", 0600, d_pages, NULL,
			  &trace_mm_fops);

	return 0;
}
fs_initcall(trace_objects_mm_init);
