/**
 * Controller for the sand yacht Clifton
 *
 * Implements manual control via RC and Ackermann steeering
 *
 * @author Michael BREUKER <michael.breuker@th-luebeck.de>
 */

#include <float.h>

#include <drivers/drv_hrt.h>
#include <lib/mathlib/mathlib.h>
#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/defines.h>
#include <px4_platform_common/posix.h>
#include <px4_platform_common/tasks.h>
#include <px4_platform_common/module.h>
#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/SubscriptionCallback.hpp>
#include <uORB/topics/manual_control_setpoint.h>
#include <uORB/topics/vehicle_status.h>
#include <uORB/topics/vehicle_local_position.h>
#include <uORB/topics/wind_direction.h>
#include <uORB/topics/sensor_gps.h>
#include <uORB/topics/sand_yacht_course.h>
#include "DrivingSystem.hpp"
#include "Autopilot.hpp"
#include <uORB/topics/sand_yacht_status.h>
#include <uORB/topics/sand_yacht_sensors.h>
#include <uORB/topics/sand_yacht_command.h>
#include <uORB/topics/dummy.h>
#include <uORB/topics/wing_direction.h>
#include <uORB/topics/sand_yacht_land_config.h>



using namespace time_literals;

inline float toDegrees(float degrees)
{
	return degrees * (float)(180.0 / M_PI);
}

class SandYachtCliftonControl final :
	public ModuleBase<SandYachtCliftonControl>,
	public px4::ScheduledWorkItem
{
public:
	SandYachtCliftonControl(void);
	~SandYachtCliftonControl(void);
	SandYachtCliftonControl(const SandYachtCliftonControl &) = delete;
	SandYachtCliftonControl operator=(const SandYachtCliftonControl &other) = delete;

	/** @see ModuleBase */
	static int task_spawn(int argc, char *argv[]);

	int print_status() override;

	/** @see ModuleBase */
	static int custom_command(int argc, char *argv[]);

	/** @see ModuleBase */
	static int print_usage(const char *reason = nullptr);

	bool init(void);

private:


	uORB::Subscription _sensors_sub{ORB_ID(sand_yacht_sensors)};
	uORB::Subscription _command_sub{ORB_ID(sand_yacht_command)};

	uORB::Subscription _wing_direction_sub{ORB_ID(wing_direction)};
	uORB::Subscription _land_config_sub{ORB_ID(sand_yacht_land_config)};
	uORB::Subscription _course_sub{ORB_ID(sand_yacht_course)};

	sand_yacht_course_s course = {0};
	sand_yacht_sensors_s sensors = {0};
	sand_yacht_command_s command = {0};
	wing_direction_s wing_direction = {0};

	sand_yacht_land_config_s land_config = {
		.timestamp = 0,
		.sail_speed_tolerance = 0.0f,
		.sail_speed_constant = 1.0f,
		.steering_constant_p = 0.5f,
		.flap_step = 0.05f
	};

	Controls controls;


	// loop
	void Run(void) override;

	// check status
	void vehicle_status_poll(void);

	// check manual control
	void manual_control_setpoint_poll(void);

	// check local position
	void local_position_poll(void);

	// check wind direction
	void wind_direction_poll(void);

	void gps_poll(void);

	void poll_sensors();

	void poll_command();

	void poll_course();

	void poll_wing_direction();

	void poll_land_config();


	// status subscription
	uORB::Subscription _status_sub{ORB_ID(vehicle_status)};

	// manual control subscription
	uORB::Subscription _manual_control_setpoint_sub{ORB_ID(manual_control_setpoint)};

	// vehicle position subscription
	uORB::Subscription _local_position_sub{ORB_ID(vehicle_local_position)};

	// wind direction subscription
	uORB::Subscription _wind_direction_sub{ORB_ID(wind_direction)};

	// gps subscription
	uORB::Subscription _gps_sub{ORB_ID(sensor_gps)};

	// RC data
	manual_control_setpoint_s _manual_control_setpoint{};

	// status data
	vehicle_status_s _status{};

	// stores data from local position
	struct vehicle_local_position_s _local_position {};

	// stores data from wind direction
	struct wind_direction_s _wind_direction {};

	// stores data from gps
	struct sensor_gps_s _gps {};

	DrivingSystem driving;

	float _mainsail_control{0.0f};
	float _steering_control{0.0f};
	float _brake_control{0.0f};

	Autopilot ap;
};
