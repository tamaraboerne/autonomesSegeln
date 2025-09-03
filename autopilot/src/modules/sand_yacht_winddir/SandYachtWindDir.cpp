#include "SandYachtWindDir.hpp"
#include <px4_platform_common/log.h>
#include <cmath>


#define UPDATE_RATE 5 // Hz
#define UPDATE_INTERVAL (1000 / UPDATE_RATE)

#define DIRECTION_OFFSET 0

/*
Defines min and max values for wind speed (avoid spike or invalid values)
*/

extern "C" __EXPORT int sand_yacht_winddir_main(int argc, char *argv[]);



int sand_yacht_winddir_main(int argc, char *argv[])
{
	// <px4_platform_common/cli.h>: port parsing? timeouts numbers?

	return SandYachtWindDir::main(argc, argv);
}


SandYachtWindDir::SandYachtWindDir(void) :
	ScheduledWorkItem(MODULE_NAME, px4::wq_configurations::test1) // TODO: nav_and_controllers correct category?
{
	PX4_INFO("SandYachtWindDir constructor called");
}

SandYachtWindDir::~SandYachtWindDir(void)
{
	PX4_INFO("SandYachtWindDir destructor called");
}

bool SandYachtWindDir::init()
{
	PX4_INFO("Initializing SandYachtWindDir...");
	PX4_INFO("Update interval: %d ms (%d hz)", UPDATE_INTERVAL, UPDATE_RATE);
	// int32_t _impl_mode = 1;
	// param_t impl_mode_param = param_find("IMPL");
	// if (impl_mode_param != PARAM_INVALID) {
	// 	PX4_INFO("Found parameter IMPL, reading value");
	// 	param_get(impl_mode_param, &impl_mode);
	// } else {
	// 	PX4_ERR("Parameter IMPL not found, using default value %d", impl_mode);
	// }

	this->impl_mode = 1;
	ScheduleOnInterval(UPDATE_INTERVAL * 1000); // ms to us

	return true;
}

int SandYachtWindDir::print_status()
{
	PX4_INFO("Im running.");
	PX4_INFO("Update interval: %d ms (%d hz)", UPDATE_INTERVAL, UPDATE_RATE);
	PX4_INFO("Sliding window size: %d", SLIDING_WINDOW);
	PX4_INFO("Sliding window filled: %s", sliding_window_filled ? "true" : "false");
	PX4_INFO("Current impl_mode: %d", impl_mode);
	PX4_INFO("Wind direction: %f", (double) _wind_dir_data.wind_direction);
	PX4_INFO("Wind direction (median): %f", (double) get_sliding_median());

	return PX4_OK;
}

int SandYachtWindDir::custom_command(int argc, char *argv[])
{
	return print_usage("unknown command");
}

int SandYachtWindDir::task_spawn(int argc, char *argv[])
{
	PX4_INFO("Starting SandYachtWindDir task");
	SandYachtWindDir *instance = new SandYachtWindDir();

	if (!instance) {
		PX4_ERR("Alloc fail SandYachtWindDir");
		return PX4_ERROR;
	}

	if (!instance->init()) {
		PX4_ERR("Initialization failed");
		delete instance;
		return PX4_ERROR;
	}

	_object.store(instance);
	_task_id = task_id_is_work_queue;

	// delete instance;
	// _object.store(nullptr);
	// _task_id = -1;

	return PX4_OK;
}

// PX4 logs from this context are printed in dmesg
void SandYachtWindDir::Run()
{
	if (should_exit()) {
		ScheduleClear();
		exit_and_cleanup();
		return;
	}

	if(_wind_dir.updated()) {
		_wind_dir.copy(&_wind_dir_data);
		_wind_dir_data.wind_direction += DIRECTION_OFFSET;

		add_sliding_window(_wind_dir_data.wind_direction);
		if(!sliding_window_filled) {
			return;
		}
		sand_yacht_wind_dir.wind_dir = get_sliding_median();

		sand_yacht_wind_dir.timestamp = hrt_absolute_time();
		_wind_speed_pub.publish(sand_yacht_wind_dir);
	}
}

int SandYachtWindDir::print_usage(const char *reason)
{
	if (reason) {
		PX4_WARN("%s\n", reason);
	}

	PRINT_MODULE_DESCRIPTION(
		R"DESCR_STR(
### Description
Section that describes the provided module functionality.

This is a template for a module running as a task in the background with start/stop/status functionality.

### Implementation
Section describing the high-level implementation of this module.

### Examples
CLI usage example:
$ module start -f -p 42

)DESCR_STR");

	PRINT_MODULE_USAGE_NAME("module", "WindDir");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_PARAM_FLAG('f', "Optional example flag", true);
	PRINT_MODULE_USAGE_PARAM_INT('p', 0, 0, 1000, "Optional example parameter", true);
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();

	return 0;
}
