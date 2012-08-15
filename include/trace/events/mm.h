#if !defined(_TRACE_MM_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_MM_H

#include <linux/tracepoint.h>
#include <linux/mm.h>

#undef TRACE_SYSTEM
#define TRACE_SYSTEM mm

/**
 * dump_pages - called by the trace page dump trigger
 * @pfn: page frame number
 * @page: pointer to the page frame
 *
 * This is a helper trace point into the dumping of the page frames.
 * It will record various infromation about a page frame.
 */
TRACE_EVENT(dump_pages,

	TP_PROTO(unsigned long pfn, struct page *page),

	TP_ARGS(pfn, page),

	TP_STRUCT__entry(
		__field(	unsigned long,	pfn		)
		__field(	unsigned long,	flags		)
		__field(	unsigned long,	index		)
		__field(	unsigned int,	count		)
		__field(	unsigned int,	mapcount	)
	),

	TP_fast_assign(
		__entry->pfn		= pfn;
		__entry->flags		= page->flags;
		__entry->count		= atomic_read(&page->_count);
		__entry->mapcount	= page_mapcount(page);
		__entry->index		= page->index;
	),

	TP_printk("pfn=%lu flags=%lx count=%u mapcount=%u index=%lu",
		  __entry->pfn, __entry->flags, __entry->count,
		  __entry->mapcount, __entry->index)
);

#endif /*  _TRACE_MM_H */

/* This part must be outside protection */
#include <trace/define_trace.h>
