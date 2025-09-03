#pragma once

#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#include <px4_platform_common/module.h>
#include <uORB/Subscription.hpp>
#include <uORB/Publication.hpp>

#include <uORB/topics/wind_speed.h>
#include <uORB/topics/sand_yacht_wind_speed.h>

using namespace time_literals;

class SandYachtWindSpeed final :
	public ModuleBase<SandYachtWindSpeed>,
	public px4::ScheduledWorkItem

{
public:
	SandYachtWindSpeed(void);
	~SandYachtWindSpeed(void);
	SandYachtWindSpeed(const SandYachtWindSpeed &) = delete;
	SandYachtWindSpeed &operator=(const SandYachtWindSpeed &other) = delete;

	// ModuleBas
	static int task_spawn(int argc, char *argv[]);
	static int custom_command(int argc, char *argv[]);
	static int print_usage(const char *reason = nullptr);
	void Run() override;
	int print_status() override;

private:

	// uorb subscriptions
	uORB::Subscription _wind_speed_sub{ORB_ID(wind_speed)};
	uORB::Publication<sand_yacht_wind_speed_s> _wind_speed_pub{ORB_ID(sand_yacht_wind_speed)};

	wind_speed_s wind_speed = {0};
	sand_yacht_wind_speed_s sand_yacht_wind_speed = {0};


	bool init();


};
