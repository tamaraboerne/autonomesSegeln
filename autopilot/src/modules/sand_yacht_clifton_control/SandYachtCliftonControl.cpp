/**
 * Controller for the sand yacht Clifton
 *
 * Implements manual control via RC and Ackermann steeering
 *
 * @author Michael BREUKER <michael.breuker@th-luebeck.de>
 */

#include "SandYachtCliftonControl.hpp"
#include <math.h>



/**
 * module start / stop handling function
 *
 * @ingroup apps
 */
extern "C" __EXPORT int sand_yacht_clifton_control_main(int argc, char *argv[]);


SandYachtCliftonControl::SandYachtCliftonControl(void) :
	ScheduledWorkItem(MODULE_NAME, px4::wq_configurations::nav_and_controllers)
{
	/* empty */
}


SandYachtCliftonControl::~SandYachtCliftonControl(void)
{
	/* empty */
}


bool SandYachtCliftonControl::init(void)
{

	// set update frequency to 50Hz (20ms interval)
	ScheduleOnInterval(20_ms);

	PX4_INFO("Sand Yacht Clifton Control started");

	return true;
}


void SandYachtCliftonControl::vehicle_status_poll(void)
{
	if (_status_sub.updated()) {
		_status_sub.copy(&_status);
	}

}


void SandYachtCliftonControl::wind_direction_poll(void)
{
	if (_wind_direction_sub.updated()) {
		_wind_direction_sub.copy(&_wind_direction);
	}
}


void SandYachtCliftonControl::manual_control_setpoint_poll(void)
{
	if (_status.nav_state == vehicle_status_s::NAVIGATION_STATE_MANUAL) {
		if (_manual_control_setpoint_sub.copy(&_manual_control_setpoint)) {

			// Set heading from the manual roll input channel
			_steering_control = _manual_control_setpoint.roll;

			// Set mainsail from the manual throttle channel
			_mainsail_control = _manual_control_setpoint.throttle;

			// Set brake from manual AUX2 channel
			_brake_control = _manual_control_setpoint.aux2;
		}
	}
}


void SandYachtCliftonControl::gps_poll(void)
{
	if (_gps_sub.updated()) {
		_gps_sub.copy(&_gps);
	}
}


void SandYachtCliftonControl::local_position_poll(void)
{
	if (_local_position_sub.updated()) {
		_local_position_sub.copy(&_local_position);
	}
}


void SandYachtCliftonControl::Run(void)
{
	if (should_exit()) {
		ScheduleClear();
		exit_and_cleanup();
		return;
	}

	PX4_DEBUG("Running SandYachtCliftonControl");

	// check status for changes
	vehicle_status_poll();

	// check manual control for changes
	manual_control_setpoint_poll();

	local_position_poll();

	wind_direction_poll();

	gps_poll();

	poll_course();
	poll_command();
	poll_sensors();
	poll_wing_direction();
	poll_land_config();
	controls = ap.calculateControls();


	if (_status.nav_state == vehicle_status_s::NAVIGATION_STATE_MANUAL) {  // manual mode
		driving.update(_mainsail_control, _steering_control, _brake_control);

	} else if (_status.nav_state == vehicle_status_s::NAVIGATION_STATE_ACRO) {  // auto mode
		driving.update(controls.mainsail, controls.steering, controls.breaking);
	}
}


int SandYachtCliftonControl::task_spawn(int argc, char *argv[])
{
	SandYachtCliftonControl *instance = new SandYachtCliftonControl();

	if (instance) {
		_object.store(instance);
		_task_id = task_id_is_work_queue;

		if (instance->init()) {
			return PX4_OK;
		}

	} else {
		PX4_ERR("alloc failed");
	}

	delete instance;
	_object.store(nullptr);
	_task_id = -1;

	return PX4_ERROR;
}


int SandYachtCliftonControl::print_status()
{
	ModuleBase<SandYachtCliftonControl>::print_status();

	if (_status.nav_state == vehicle_status_s::NAVIGATION_STATE_MANUAL) {
		PX4_INFO("Control Status: Manual Control");

	} else if (_status.nav_state == vehicle_status_s::NAVIGATION_STATE_ACRO) {
		PX4_INFO("Control Status: Autonomous Control");

	} else {
		PX4_INFO("Control Status: Unknown");
	}


	ap.print_status();


	// PX4_INFO("Left Wheel %0.2f", (double) _actuator_servos.control[LEFT]);
	// PX4_INFO("Right Wheel %0.2f", (double) _actuator_servos.control[RIGHT]);
	// PX4_INFO("Flap %0.2f", (double) _actuator_servos.control[MAINSAIL]);

	return 0;
}


int SandYachtCliftonControl::custom_command(int argc, char *argv[])
{
	return print_usage("unknown command");
}


int SandYachtCliftonControl::print_usage(const char *reason)
{
	if (reason) {
		PX4_WARN("%s\n", reason);
	}

	PRINT_MODULE_DESCRIPTION(
		R"DESCR_STR(
### Description
Controls the actuators of the sand yacht Clifton.

Publishes `actuator_servos" messages.

### Implementation
Currently, this implementation supports only two modes:

 * Manual mode: Roll and thrust controls are passed directly through to the actuators
 * Acro mode: The sand yacht Clifton runs in mission mode

### Examples
CLI usage example:
Start controller:
$ sand_yacht_clifton_control start

Print status of the controller:
$ sand_yacht_clifton_control status

Stop controller:
$ sand_yacht_clifton_control stop
)DESCR_STR");

	PRINT_MODULE_USAGE_NAME("sand_yacht_clifton_control", "controller");
	PRINT_MODULE_USAGE_COMMAND("start")
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();

	return 0;
}


int sand_yacht_clifton_control_main(int argc, char *argv[]) {
	return SandYachtCliftonControl::main(argc, argv);
}



void SandYachtCliftonControl::poll_sensors(){
	if(this->_sensors_sub.updated()){
		if(!this->_sensors_sub.copy(&sensors)){
			PX4_ERR("Failed to copy sensors data from a sub");
		}
		ap.set_sensors(sensors);
	}
}

void SandYachtCliftonControl::poll_command(){
	if(this->_command_sub.updated()){
		if(!this->_command_sub.copy(&command)){
			PX4_ERR("Failed to copy command data from a sub");
		}
		ap.set_command(command.command);
	}
}

void SandYachtCliftonControl::poll_wing_direction(){
	if(this->_wing_direction_sub.updated()){
		if(!this->_wing_direction_sub.copy(&wing_direction)){
			PX4_ERR("Failed to copy wing_direction data from a sub");
		}
		ap.set_wing_direction(wing_direction);
	}
}

void SandYachtCliftonControl::poll_land_config(){

	if(this->_land_config_sub.updated()){
		if(!this->_land_config_sub.copy(&land_config)){
			PX4_ERR("Failed to copy land_config data from a sub");
		}
		ap.set_land_config(land_config);
	}
}

void SandYachtCliftonControl::poll_course(){
	if(_course_sub.updated()){

		if(!this->_course_sub.copy(&course)){
			PX4_ERR("Failed to copy course data from a sub");
			// is ok
		}
		ap.set_course(course);
	}
}

