/*
 * Copyright (C) 2008 Manuel Lauss <mano@roarinelk.homelinux.net>
 *
 * Previous incarnations were:
 * Copyright (C) 2001, 2006, 2008 MontaVista Software, <source@mvista.com>
 * Copied and modified Carsten Langgaard's time.c
 *
 * Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 1999,2000 MIPS Technologies, Inc.  All rights reserved.
 *
 * ########################################################################
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * ########################################################################
 *
 * Clocksource/event using the 32.768kHz-clocked Counter1 ('RTC' in the
 * databooks).  Firmware/Board init code must enable the counters in the
 * counter control register, otherwise the CP0 counter clocksource/event
 * will be installed instead (and use of 'wait' instruction is prohibited).
 */

#include <linux/clockchips.h>
#include <linux/clocksource.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>

#include <asm/time.h>
#include <asm/mach-au1x00/au1000.h>

/* 32kHz clock enabled and detected */
#define CNTR_OK (SYS_CNTRL_E0 | SYS_CNTRL_32S)

extern int allow_au1k_wait; /* default off for CP0 Counter */

static cycle_t au1x_counter1_read(void)
{
	return au_readl(SYS_RTCREAD);
}

static struct clocksource au1x_counter1_clocksource = {
	.name		= "alchemy-counter1",
	.read		= au1x_counter1_read,
	.mask		= CLOCKSOURCE_MASK(32),
	.flags		= CLOCK_SOURCE_IS_CONTINUOUS,
	.rating		= 100,
};

static int au1x_rtcmatch2_set_next_event(unsigned long delta,
					 struct clock_event_device *cd)
{
	delta += au_readl(SYS_RTCREAD);
	/* wait for register access */
	while (au_readl(SYS_COUNTER_CNTRL) & SYS_CNTRL_M21)
		;
	au_writel(delta, SYS_RTCMATCH2);
	au_sync();

	return 0;
}

static void au1x_rtcmatch2_set_mode(enum clock_event_mode mode,
				    struct clock_event_device *cd)
{
}

static irqreturn_t au1x_rtcmatch2_irq(int irq, void *dev_id)
{
	struct clock_event_device *cd = dev_id;
	cd->event_handler(cd);
	return IRQ_HANDLED;
}

static struct clock_event_device au1x_rtcmatch2_clockdev = {
	.name		= "rtcmatch2",
	.features	= CLOCK_EVT_FEAT_ONESHOT,
	.rating		= 100,
	.irq		= AU1000_RTC_MATCH2_INT,
	.set_next_event	= au1x_rtcmatch2_set_next_event,
	.set_mode	= au1x_rtcmatch2_set_mode,
	.cpumask	= CPU_MASK_ALL_PTR,
};

static struct irqaction au1x_rtcmatch2_irqaction = {
	.handler	= au1x_rtcmatch2_irq,
	.flags		= IRQF_DISABLED | IRQF_TIMER,
	.name		= "timer",
	.dev_id		= &au1x_rtcmatch2_clockdev,
};

void __init plat_time_init(void)
{
	struct clock_event_device *cd = &au1x_rtcmatch2_clockdev;
	unsigned long t;

	/* Check if firmware (YAMON, ...) has enabled 32kHz and clock
	 * has been detected.  If so install the rtcmatch2 clocksource,
	 * otherwise don't bother.  Note that both bits being set is by
	 * no means a definite guarantee that the counters actually work
	 * (the 32S bit seems to be stuck set to 1 once a single clock-
	 * edge is detected, hence the timeouts).
	 */
	if (CNTR_OK != (au_readl(SYS_COUNTER_CNTRL) & CNTR_OK))
		goto cntr_err;

	/*
	 * setup counter 1 (RTC) to tick at full speed
	 */
	t = 0xffffff;
	while ((au_readl(SYS_COUNTER_CNTRL) & SYS_CNTRL_T1S) && --t)
		asm volatile ("nop");
	if (!t)
		goto cntr_err;

	au_writel(0, SYS_RTCTRIM);	/* 32.768 kHz */
	au_sync();

	t = 0xffffff;
	while ((au_readl(SYS_COUNTER_CNTRL) & SYS_CNTRL_C1S) && --t)
		asm volatile ("nop");
	if (!t)
		goto cntr_err;
	au_writel(0, SYS_RTCWRITE);
	au_sync();

	t = 0xffffff;
	while ((au_readl(SYS_COUNTER_CNTRL) & SYS_CNTRL_C1S) && --t)
		asm volatile ("nop");
	if (!t)
		goto cntr_err;

	/* register counter1 clocksource and event device */
	clocksource_set_clock(&au1x_counter1_clocksource, 32768);
	clocksource_register(&au1x_counter1_clocksource);

	cd->shift = 32;
	cd->mult = div_sc(32768, NSEC_PER_SEC, cd->shift);
	cd->max_delta_ns = clockevent_delta2ns(0xffffffff, cd);
	cd->min_delta_ns = clockevent_delta2ns(8, cd);	/* ~0.25ms */
	clockevents_register_device(cd);
	setup_irq(AU1000_RTC_MATCH2_INT, &au1x_rtcmatch2_irqaction);

	printk(KERN_INFO "Alchemy clocksource installed\n");

	/* can now use 'wait' */
	allow_au1k_wait = 1;
	return;

cntr_err:
	/* counters unusable, use C0 counter */
	r4k_clockevent_init();
	init_r4k_clocksource();
	allow_au1k_wait = 0;
}
