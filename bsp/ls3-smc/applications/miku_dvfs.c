#include "miku.h"

struct pll_level pll_levels[NUM_PLL_LEVEL] = 
{{/* Idle: Max 825MHz */
	.refc = 2,
	.loopc = 33,
	.div = 1,
	.vid = 1250, /* FIXME: Low down it later */
	.highest_scale = (4 - 1), /* 4 / 8 */
	.lowest_scale = 0, /* 1 / 8 */
	.stable_scale = (8 - 1), /* 8/8, 1650MHz */
	.node_scale = (4 - 1), /* 4 / 8 */
	.ht_scale = (3 - 1), /* 3 / 8 */
},{/* Normal: Max 1650MHz */
	.refc = 2,
	.loopc = 33,
	.div = 1,
	.vid = 1250, /* FIXME: Low down it later */
	.highest_scale = (8 - 1), /* 8 / 8 */
	.lowest_scale = (5 - 1), /* 5 / 8 */
	.stable_scale = (8 - 1), /* 8/8, 1650MHz */
	.node_scale = (8 - 1), /* 8 / 8 */
	.ht_scale = (4 - 1), /* 4 / 8 */
},{/* Boost: Max 2200MHz */
	.refc = 1,
	.loopc = 22,
	.div = 1,
	.vid = 1450, /* FIXME: Low down it later */
	.highest_scale = (8 - 1), /* 8 / 8 */
	.lowest_scale = (5 - 1), /* 5 / 8 */
	.stable_scale = (6 - 1), /* 6/8, 1650MHz */
	.node_scale = (4 - 1), /* 4 / 8 */
	.ht_scale = (3 - 1), /* 3 / 8 */
}
};

/* Shaow means what we tell kernel */
#define SHADOW_LEVEL_NUM	10
rt_uint16_t	shadow_level_freq[SHADOW_LEVEL_NUM] = {200, 410, 618, 825, 1030, 1237, 1443, BASEFREQ, 1925, 2200};

rt_uint16_t	target_shadow_freq[NUM_CORE];
rt_uint8_t	current_pll_level;
rt_uint8_t	target_pll_level;
rt_uint16_t	current_vid;
rt_uint16_t	target_vid;
rt_time_t	last_change;

static rt_time_t get_time()
{
	return rt_tick_get() / rt_tick_from_millisecond(1);
}

static rt_uint8_t core_current_scale(rt_uint8_t core_id)
{
	rt_uint32_t reg = HWREG32(LS3_CORE_SCALE_REG);
	reg = (reg >> (4 * core_id)) & 0x7;
	return reg;
}

static rt_uint8_t core_set_scale(rt_uint8_t core_id, rt_uint8_t scale)
{
	if (scale > 0x7)
		scale = 0x7;

	MIKU_DBG("Set Core: %d, scale: %d", core_id, scale);

	rt_uint32_t reg = HWREG32(LS3_CORE_SCALE_REG);

	reg &= ~(0x7 << (4 * core_id));
	reg |= scale << (4 * core_id);
	HWREG32(LS3_CORE_SCALE_REG) = reg;
}

static rt_uint16_t core_current_freq(rt_uint8_t core_id)
{
	return pll_to_freq(&pll_levels[current_pll_level]) / 8 * (core_current_scale(core_id) + 1);
}

rt_err_t miku_dvfs_fast_act(void)
{
	int i, j;
	for(i = 0; i < NUM_CORE; i++) {
		rt_uint16_t target_scale = target_shadow_freq[i] * 8 / pll_to_freq(&pll_levels[current_pll_level]);

		if(target_scale > pll_levels[current_pll_level].highest_scale)
			target_scale = pll_levels[current_pll_level].highest_scale;
		
		if(target_scale < pll_levels[current_pll_level].lowest_scale)
			target_scale = pll_levels[current_pll_level].lowest_scale;

		core_set_scale(i, target_scale);
	}

	return RT_EOK;
}

rt_err_t miku_judge_dvfs(void)
{
	rt_uint16_t highest_freq = 0;
	rt_uint8_t wanted_level = 0;
	int i;

	for(i = 0; i < NUM_CORE; i++) {
		if (target_shadow_freq[i] > highest_freq)
			highest_freq = target_shadow_freq[i];
	}

	wanted_level = 0;
	/* Now see what's the best level fit that freq */
	if (highest_freq > pll_to_freq(&pll_levels[NUM_PLL_LEVEL - 1]))
		wanted_level = NUM_PLL_LEVEL - 1;
	else {
		for(i = 0; i < NUM_PLL_LEVEL; i++) {
			if (pll_to_highest(&pll_levels[i]) >= highest_freq) {
				wanted_level = i;
				break;
			}
		}
	}

	/* FIXME: Add time checks So, do we need real action? */
	if (wanted_level != current_pll_level) {
	}

	target_pll_level = wanted_level;
	target_vid = pll_levels[wanted_level].vid;
}

rt_err_t miku_dvfs_action(void)
{
	struct pll_level *action_level = &pll_levels[target_pll_level];
	rt_uint32_t level;

	level = rt_hw_interrupt_disable();

	/* Bump UP: VID First */
	if (current_vid < target_vid) {
		pmic_vctrl(target_vid);
		current_vid = target_vid;
	}

	if (current_pll_level != target_pll_level) {
		MIKU_DBG("DVFS ACT: REFC: %d, LOOPC: %d, DIV: %d", action_level->refc, action_level->loopc, action_level->div);
		ht_scale_sel(action_level->ht_scale);
		node_scale_sel(action_level->node_scale);
		main_pll_sel(action_level->refc, action_level->loopc, action_level->div);
		current_pll_level = target_pll_level;
		stable_scale_sel(action_level->stable_scale);
		/* Resel Scale */
		miku_dvfs_fast_act();
	}

	/* Bump Down: PLL First */
	if (current_vid > target_vid) {
		pmic_vctrl(target_vid);
		current_vid = target_vid;
	}

	rt_hw_interrupt_enable(level);

}

rt_uint8_t miku_get_freq_levels(struct smc_message *msg)
{
	rt_uint32_t argp = msg->arg;
	struct freq_level_args *arg = (struct freq_level_args*)&argp;

	arg->min_level = 0;
	arg->max_normal_level = 7;
	arg->max_boost_level = SHADOW_LEVEL_NUM - 1;

	msg->value = argp;

	return MIKU_EOK;
}

rt_uint8_t miku_get_freq_info(struct smc_message *msg)
{
	rt_uint32_t argp = msg->arg;
	struct freq_info_args *arg = (struct freq_info_args*)&argp;

	if (arg->index == FREQ_INFO_INDEX_FREQ) {
		rt_uint8_t lvl = arg->info;
		rt_uint16_t freq;

		if (arg->info >= SHADOW_LEVEL_NUM)
			return MIKU_ECMDFAIL;

		freq = shadow_level_freq[lvl];
		MIKU_DBG("DVFS: Get Level %u, FREQ: %u\n", lvl, freq);
		arg->info = freq;
	} else
		return MIKU_ECMDFAIL;

	msg->value = argp;
	return MIKU_EOK;
	
}

rt_uint8_t miku_set_cpu_level(struct smc_message *msg)
{
	rt_uint32_t argp = msg->arg;
	struct freq_level_setting_args *arg = (struct freq_level_setting_args*)&argp;
	int i;
	
	if (!arg->cpumask)
		return MIKU_ECMDFAIL;

	if (arg->level >= SHADOW_LEVEL_NUM)
		return MIKU_ECMDFAIL;

	for (i = 0; i < NUM_CORE; i++)
	{
		if ((1 << i) & arg->cpumask) {
			uint16_t freq = shadow_level_freq[arg->level];
			target_shadow_freq[i] = freq;
			MIKU_DBG("Setting Core %d, Shadow Freq: %d\n", i, freq);
		}
	}

	miku_dvfs_fast_act();
	msg->value = argp;
	return MIKU_EOK;
}

rt_err_t miku_enable_dvfs(void)
{
	miku_cmd_install(CMD_GET_FREQ_LEVELS, miku_get_freq_levels);
	miku_cmd_install(CMD_GET_FREQ_INFO, miku_get_freq_info);
	miku_cmd_install(CMD_SET_CPU_LEVEL, miku_set_cpu_level);
	MIKU_DBG("DVFS Enabled");
	return RT_EOK;
}

rt_err_t miku_dvfs_init(void)
{
	int i;
	for (i = 0; i < NUM_CORE; i++)
		target_shadow_freq[i] = BASEFREQ;

	current_pll_level = 1;
}
