#include "miku.h"

rt_err_t miku_init(void)
{
	miku_cmd_init();
	miku_features_init();
	miku_sensors_init();
	miku_fan_init();
	miku_threads_init();
}