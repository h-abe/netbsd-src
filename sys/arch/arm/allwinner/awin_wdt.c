/*-
 * Copyright (c) 2013 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Matt Thomas of 3am Software Foundry.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "locators.h"
#include "opt_allwinner.h"
#include "awin_wdt.h"

#include <sys/cdefs.h>

__KERNEL_RCSID(1, "$NetBSD: awin_wdt.c,v 1.3 2014/02/20 21:48:38 matt Exp $");

#include <sys/param.h>
#include <sys/bus.h>
#include <sys/device.h>
#include <sys/intr.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/wdog.h>

#include <dev/sysmon/sysmonvar.h>

#include <arm/locore.h>

#include <arm/allwinner/awin_reg.h>
#include <arm/allwinner/awin_var.h>

#if NAWIN_WDT > 0
static int awin_wdt_match(device_t, cfdata_t, void *);
static void awin_wdt_attach(device_t, device_t, void *);

#ifndef AWIN_WDT_PERIOD_DEFAULT
#define AWIN_WDT_PERIOD_DEFAULT  10
#endif

static const uint8_t period_map[] = {
	[0] = __SHIFTIN(AWIN_WDOG_MODE_INTV_1SEC, AWIN_WDOG_MODE_INTV),
	[1] = __SHIFTIN(AWIN_WDOG_MODE_INTV_1SEC, AWIN_WDOG_MODE_INTV),
	[2] = __SHIFTIN(AWIN_WDOG_MODE_INTV_2SEC, AWIN_WDOG_MODE_INTV),
	[3] = __SHIFTIN(AWIN_WDOG_MODE_INTV_3SEC, AWIN_WDOG_MODE_INTV),
	[4] = __SHIFTIN(AWIN_WDOG_MODE_INTV_4SEC, AWIN_WDOG_MODE_INTV),
	[5] = __SHIFTIN(AWIN_WDOG_MODE_INTV_5SEC, AWIN_WDOG_MODE_INTV),
	[6] = __SHIFTIN(AWIN_WDOG_MODE_INTV_6SEC, AWIN_WDOG_MODE_INTV),
	[7] = __SHIFTIN(AWIN_WDOG_MODE_INTV_8SEC, AWIN_WDOG_MODE_INTV),
	[8] = __SHIFTIN(AWIN_WDOG_MODE_INTV_8SEC, AWIN_WDOG_MODE_INTV),
	[9] = __SHIFTIN(AWIN_WDOG_MODE_INTV_10SEC, AWIN_WDOG_MODE_INTV),
	[10] = __SHIFTIN(AWIN_WDOG_MODE_INTV_10SEC, AWIN_WDOG_MODE_INTV),
	[11] = __SHIFTIN(AWIN_WDOG_MODE_INTV_12SEC, AWIN_WDOG_MODE_INTV),
	[12] = __SHIFTIN(AWIN_WDOG_MODE_INTV_12SEC, AWIN_WDOG_MODE_INTV),
	[13] = __SHIFTIN(AWIN_WDOG_MODE_INTV_14SEC, AWIN_WDOG_MODE_INTV),
	[14] = __SHIFTIN(AWIN_WDOG_MODE_INTV_14SEC, AWIN_WDOG_MODE_INTV),
	[15] = __SHIFTIN(AWIN_WDOG_MODE_INTV_16SEC, AWIN_WDOG_MODE_INTV),
	[16] = __SHIFTIN(AWIN_WDOG_MODE_INTV_16SEC, AWIN_WDOG_MODE_INTV),
};

static struct awin_wdt_softc {
	device_t sc_dev;
	bus_space_tag_t sc_bst;
	bus_space_handle_t sc_bsh;
	struct sysmon_wdog sc_smw;
        u_int sc_wdog_period;
        bool sc_wdog_armed;
	uint32_t sc_wdog_mode;
} awin_wdt_sc = {
	.sc_bst = &awin_bs_tag,
	.sc_wdog_period = AWIN_WDT_PERIOD_DEFAULT,
};

static int
awin_wdt_tickle(struct sysmon_wdog *smw)
{
	struct awin_wdt_softc * const sc = smw->smw_cookie;
	bus_space_write_4(sc->sc_bst, sc->sc_bsh, AWIN_WDOG_CTRL_REG,
	    AWIN_WDOG_CTRL_RSTART);
	return 0;
}

static int
awin_wdt_setmode(struct sysmon_wdog *smw)
{
	struct awin_wdt_softc * const sc = smw->smw_cookie;

	if ((smw->smw_mode & WDOG_MODE_MASK) == WDOG_MODE_DISARMED) {
		/*
		 * We can't disarm the watchdog.
		 */
		return sc->sc_wdog_armed ? EBUSY : 0;
	}

	if (sc->sc_wdog_armed && smw->smw_period == sc->sc_wdog_period) {
		bus_space_write_4(sc->sc_bst, sc->sc_bsh, AWIN_WDOG_MODE_REG,
		    sc->sc_wdog_mode);
		bus_space_write_4(sc->sc_bst, sc->sc_bsh, AWIN_WDOG_CTRL_REG,
		    AWIN_WDOG_CTRL_RSTART);
		return 0;
	}
	if (smw->smw_period > __arraycount(period_map)) {
		return EINVAL;
	}
	if (smw->smw_period == WDOG_PERIOD_DEFAULT) {
		smw->smw_period = AWIN_WDT_PERIOD_DEFAULT;
		sc->sc_wdog_period = AWIN_WDT_PERIOD_DEFAULT;
	}
	sc->sc_wdog_mode = AWIN_WDOG_MODE_EN | AWIN_WDOG_MODE_RST_EN
	    | period_map[sc->sc_wdog_period];

	bus_space_write_4(sc->sc_bst, sc->sc_bsh, AWIN_WDOG_MODE_REG,
	    sc->sc_wdog_mode);
	bus_space_write_4(sc->sc_bst, sc->sc_bsh, AWIN_WDOG_CTRL_REG,
	    AWIN_WDOG_CTRL_RSTART);
	return 0;
}

CFATTACH_DECL_NEW(awin_wdt, 0,
	awin_wdt_match, awin_wdt_attach, NULL, NULL);

static int
awin_wdt_match(device_t parent, cfdata_t cf, void *aux)
{
	struct awinio_attach_args * const aio = aux;
	const struct awin_locators * const loc = &aio->aio_loc;
	const int port = cf->cf_loc[AWINIOCF_PORT];

	if (awin_wdt_sc.sc_dev != NULL)
		return 0;

	if (strcmp(cf->cf_name, loc->loc_name)
	    || (port != AWINIOCF_PORT_DEFAULT && port != loc->loc_port))
		return 0;

	KASSERT(loc->loc_port == AWINIOCF_PORT_DEFAULT);

	return 1;
}

static void
awin_wdt_attach(device_t parent, device_t self, void *aux)
{
	struct awin_wdt_softc * const sc = &awin_wdt_sc;
	struct awinio_attach_args * const aio = aux;
	const struct awin_locators * const loc = &aio->aio_loc;

	sc->sc_dev = self;
	sc->sc_wdog_armed = (device_cfdata(self)->cf_flags & 1) != 0;

	sc->sc_bst = aio->aio_core_bst;
	bus_space_subregion(sc->sc_bst, aio->aio_core_bsh,
	    loc->loc_offset, loc->loc_size, &sc->sc_bsh);

	aprint_naive("\n");
	aprint_normal(": default period is %u seconds%s\n",
	    sc->sc_wdog_period,
	    sc->sc_wdog_armed ? " (wdog is active)" : "");

	sc->sc_smw.smw_name = device_xname(sc->sc_dev);
	sc->sc_smw.smw_cookie = sc;
	sc->sc_smw.smw_setmode = awin_wdt_setmode;
	sc->sc_smw.smw_tickle = awin_wdt_tickle;
	sc->sc_smw.smw_period = sc->sc_wdog_period;

	if (sysmon_wdog_register(&sc->sc_smw) != 0)
		aprint_error_dev(self, "unable to register with sysmon\n");

	if (sc->sc_wdog_armed) {
		int error = sysmon_wdog_setmode(&sc->sc_smw, WDOG_MODE_KTICKLE,
		    sc->sc_wdog_period);
		if (error)
			aprint_error_dev(self,
			    "failed to start kernel tickler: %d\n", error);
	}
}
#endif

void
awin_wdog_reset(void)
{
	cpsid(I32_bit|F32_bit);
	bus_space_write_4(&awin_bs_tag, awin_core_bsh,
	    AWIN_TMR_OFFSET + AWIN_WDOG_MODE_REG,
	    AWIN_WDOG_MODE_EN | AWIN_WDOG_MODE_RST_EN);
	for (;;) {
		__asm("wfi");
	}
}
