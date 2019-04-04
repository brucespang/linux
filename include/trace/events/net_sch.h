/* SPDX-License-Identifier: GPL-2.0 */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM net_sch

#if !defined(_TRACE_NET_SCH_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_NET_SCH_H

#include <linux/tracepoint.h>
#include <net/sch_generic.h>

/*
 * queue length of a sch queue
 */
TRACE_EVENT(net_sch_qlen,

  TP_PROTO(const struct Qdisc *sch, __u32 qlen),

  TP_ARGS(sch, qlen),

	TP_STRUCT__entry(
		__field(const char *, name)
		__field(__u32, qlen)
	),

	TP_fast_assign(
    __entry->name = sch->dev_queue->dev->name;
    __entry->qlen = qlen;
	),

	TP_printk("iface=%s qlen=%d\n",
		  __entry->name, __entry->qlen)
);


#endif /* _TRACE_NET_SCH_H */

/* This part must be outside protection */
#include <trace/define_trace.h>
