#include "SandYachtWindSpeed.hpp"
#include <px4_platform_common/log.h>


#define UPDATE_RATE 100 // Hz
#define UPDATE_INTERVAL (1000 / UPDATE_RATE)

#define DATA_SCALE 10.0f

extern "C" __EXPORT int sand_yacht_windspeed_main(int argc, char *argv[]);



int sand_yacht_windspeed_main(int argc, char *argv[])
{
	// <px4_platform_common/cli.h>: port parsing? timeouts numbers?

	return SandYachtWindSpeed::main(argc, argv);
}

SandYachtWindSpeed::SandYachtWindSpeed(void) :
	ScheduledWorkItem(MODULE_NAME, px4::wq_configurations::test1) // TODO: nav_and_controllers correct category?
{
	PX4_INFO("SandYachtWindSpeed constructor called");
}

SandYachtWindSpeed::~SandYachtWindSpeed(void)
{
	PX4_INFO("SandYachtWindSpeed destructor called");
}

bool SandYachtWindSpeed::init()
{
	PX4_INFO("Initializing SandYachtWindSpeed...");
	PX4_INFO("Update interval: %d ms (%d hz)", UPDATE_INTERVAL, UPDATE_RATE);

	ScheduleOnInterval(UPDATE_INTERVAL * 1000); // ms to us

	return true;
}

int SandYachtWindSpeed::print_status()
{
	PX4_INFO("Im running.");
	PX4_INFO("Update interval: %d ms (%d hz)", UPDATE_INTERVAL, UPDATE_RATE);
	PX4_INFO("Data scale: %f", (double) DATA_SCALE);
	PX4_INFO("Wind speed (raw): %f cm/s", (double) wind_speed.wind_speed);
	PX4_INFO("Wind speed (pub): %f m/s", (double) sand_yacht_wind_speed.m_s_speed);
	return PX4_OK;
}

int SandYachtWindSpeed::custom_command(int argc, char *argv[])
{
	return print_usage("unknown command");
}

int SandYachtWindSpeed::task_spawn(int argc, char *argv[])
{
	PX4_INFO("Starting SandYachtWindSpeed task");
	SandYachtWindSpeed *instance = new SandYachtWindSpeed();

	if (!instance) {
		PX4_ERR("Alloc fail SandYachtWindSpeed");
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
void SandYachtWindSpeed::Run()
{

	if (should_exit()) {
		ScheduleClear();
		exit_and_cleanup();
		return;
	}

	if(_wind_speed_sub.updated()) {
		_wind_speed_sub.copy(&wind_speed);


		sand_yacht_wind_speed.timestamp = hrt_absolute_time();
		sand_yacht_wind_speed.m_s_speed = (float) ((float) wind_speed.wind_speed ) / ((float) 100.0f); // cm/s to m/s
		sand_yacht_wind_speed.m_s_speed *= DATA_SCALE;

		_wind_speed_pub.publish(sand_yacht_wind_speed);
	}
}

int SandYachtWindSpeed::print_usage(const char *reason)
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

	PRINT_MODULE_USAGE_NAME("module", "WingDir");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_PARAM_FLAG('f', "Optional example flag", true);
	PRINT_MODULE_USAGE_PARAM_INT('p', 0, 0, 1000, "Optional example parameter", true);
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();

	return 0;
}
