/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-10     zhuangwei    first version
 */

#include <rtthread.h>
#include "miku.h"

int main(int argc, char** argv)
{
	MIKU_DBG("BRIDGE ID: %x\n", HWREG32(LS7A_MISC_BASE + 0x3ff8));


#if 0
	up9512s_pmic_init();

//	MIKU_DBG("PMIC IOUT: %x\n", pmic_get_iout());
//	MIKU_DBG("PMIC VOUT: %x\n", pmic_get_vout());
//	MIKU_DBG("PMIC TEMP: %x\n", pmic_get_temp());

	MIKU_DBG("TRY SET VID");
	pmic_set_vid(0x9e);

	miku_init();
	MIKU_DBG("Miku init\n");
#endif
	/* 
	 * The main thread will be used to process CMD,
	 * We shouldn't kill it, otherwise childreen will die too. 
	 */
	miku_init();
	while(1) {
		miku_proc_cmd();
		/* Give up ticks to other tasks */
		rt_thread_yield();
	}
	return 0;
}
