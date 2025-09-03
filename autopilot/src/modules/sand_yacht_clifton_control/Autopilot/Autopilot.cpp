#include "Autopilot.hpp"
#include <math.h>

uint8_t STATE_IDLE = 0b0;
uint8_t STATE_READY = 0b00100000;
uint8_t STATE_DRIVING = 0b01000000;

uint8_t AUTOPILOT_ERROR = 0b10000000;
uint8_t NO_AUTOPILOT_ERROR = 0b0;

uint8_t COMMAND_STOP = 0;
uint8_t COMMAND_START = 1;
uint8_t COMMAND_CONTINUE = 2;

// flap_control
double current_speed = 0;

// steering_control
float error = 0.0f;
float preferred_angle = 0.0f;

// forward declaration
float addAngles(float a, float b);
float angleDifference(float a, float b);
void changeState();
double desired_angle_of_attack();


// === Utility Functions ===
/**
 * @brief Clamps a value between a minimum and maximum range.
 *
 * @param value The input value to be trimmed.
 * @param min The minimum allowable value.
 * @param max The maximum allowable value.
 * @return The trimmed value within the [min, max] range.
 */
float trim(float value, float min, float max) {
	return value < min
	? min : value > max
	? max : value;
}

/**
 * @brief Linearly maps a value from one range to another.
 *
 * @param x The input value.
 * @param in_min Lower bound of the input range.
 * @param in_max Upper bound of the input range.
 * @param out_min Lower bound of the output range.
 * @param out_max Upper bound of the output range.
 * @return The scaled value in the output range.
 */
float scale(float x, float in_min, float in_max, float out_min, float out_max){
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/**
 * @brief Adds two angles and normalizes the result to [0, 360).
 *
 * @param a First angle (in degrees)
 * @param b Second angle (in degrees)
 * @return Normalized sum of the two angles in the range [0, 360)
 */
float addAngles(float a, float b) {
	float result = fmod(a + b, 360.0f);
	if (result < 0.0f) result += 360.0f;
	return result;
}

/**
 * @brief Computes the smallest signed difference between two angles.
 *
 * @param a First angle
 * @param b Second angle
 * @return difference between the two angles. -180 to +180
 */
float angleDifference(float a, float b) {
	float diff = fmod(a - b, 360.0f);
	if (diff < -180.0f) diff += 360.0f;
	if (diff > 180.0f) diff -= 360.0f;
	return diff;
}

float Autopilot::calculate_angle(Position current, Position target) {

	double dx = static_cast<double>(target.x - current.x);
	double dy = static_cast<double>(target.y - current.y);

	//calculate angle
	static double angle_rad;
	angle_rad = std::atan2(dx, dy);

	double angle_deg = angle_rad * 180.0 / M_PI; // convert from radiant to degree

	return  static_cast<float>(angle_deg);
}

// === Core Autopilot Functions ===
Autopilot::Autopilot(){

}

Controls Autopilot::calculateControls(){
	testWaypointReached();
	update_status();
	publish_debug_info();

	return Controls {
		.steering = steering_control(),
		.mainsail = flap_controller(),
		.breaking = this->status.state == STATE_DRIVING ? 0.0f: -1.0f
	};
}

// === Control Algorithms ===
float Autopilot::flap_controller() {
	current_speed = angleDifference(old_wing_direction, wing_direction.wing_direction);
	int16_t angle_error = angleDifference( addAngles(sensors.wind_direction, desired_angle_of_attack()), wing_direction.wing_direction);
	double desired_speed = land_config.sail_speed_constant * angle_error;

	current_flap = trim(current_flap, -1.0, 1.0);

	current_flap = current_speed < desired_speed - land_config.sail_speed_tolerance
	? current_flap + land_config.flap_step
	: current_speed > desired_speed + land_config.sail_speed_tolerance
	? current_flap - land_config.flap_step
	: current_flap;

	return current_flap;
}

float Autopilot::steering_control() {
	error = angleDifference(
		calculate_angle(
			Position {
				static_cast<int16_t>(sensors.x),
				static_cast<int16_t>(sensors.y)
			},

			Position {
				waypoints[status.current_waypoint].pos.x,
				waypoints[status.current_waypoint].pos.y
			}
		),
		sensors.heading);

	preferred_angle = (double) error * land_config.steering_constant_p;

	return scale(trim(preferred_angle, -44.0, 44.0 ), -44.0f, 44.0f, -1.0, 1.0f);
}

double Autopilot::desired_angle_of_attack(){
	if (status.state != STATE_DRIVING) return 0;
	// Grenzwerte aus Polardiagram in Herr Fischers BA abgelesen
	if (sensors.wind_direction < 162 && sensors.wind_direction > 18) return 8.25;
	if (sensors.wind_direction > 196 && sensors.wind_direction < 342) return -8.25;

	return 0;
}

// === Navigation & Status Handling ===
void Autopilot::testWaypointReached() {

	if (this->status.state != STATE_DRIVING) return;
	if(
		//TODO punkt typen unsterscheiden in tolleranzbereich
		abs(sensors.x - waypoints[status.current_waypoint].pos.x)
		+ abs(sensors.y - waypoints[status.current_waypoint].pos.y)
		< (waypoints[status.current_waypoint].curve
			? land_config.curve_waypoint_tolerance
			: land_config.default_waypoint_tolerance
		)
	) {
		PX4_INFO("Reached waypoint %d (x=%d, y=%d), increasing waypoint...",
			status.current_waypoint,
			waypoints[status.current_waypoint].pos.x,
			waypoints[status.current_waypoint].pos.y
		);

		status.current_waypoint++;
		if (status.current_waypoint >= courseLength) {
			this->status.current_waypoint = 0;
			this->status.state = STATE_IDLE;
			PX4_INFO("Autopilot reached final waypoint");
		}
	}
}

void Autopilot::update_status()
{
	updated_status.waypoint = this->status.current_waypoint & 0b11111;
	updated_status.state = this->status.state;
	updated_status.error = (this->status.error == true) ? AUTOPILOT_ERROR: NO_AUTOPILOT_ERROR;
	updated_status.timestamp = hrt_absolute_time();

	if(
		updated_status.waypoint != last_status.waypoint ||
		updated_status.error != last_status.error ||
		updated_status.state != last_status.state

	){
		_autopilot_status.publish(updated_status);
	}


	last_status = updated_status;
}

void Autopilot::publish_debug_info() {

	static autopilot_debug_info_s updated_debug_info = {0};
	static autopilot_debug_info_s last_debug_info = {0};

	updated_debug_info.timestamp = hrt_absolute_time();
	updated_debug_info.target_x = waypoints[status.current_waypoint].pos.x;
	updated_debug_info.target_y = waypoints[status.current_waypoint].pos.y;

	updated_debug_info.target_angle = calculate_angle(
			Position {
				static_cast<int16_t>(sensors.x),
				static_cast<int16_t>(sensors.y)
			},
			Position {
				waypoints[status.current_waypoint].pos.x,
				waypoints[status.current_waypoint].pos.y
			}
		);

	updated_debug_info.target_relative_angle = angleDifference(
		updated_debug_info.target_angle,
		sensors.heading
	);

	updated_debug_info.target_distance =
		abs(sensors.x - waypoints[status.current_waypoint].pos.x)
		+ abs(sensors.y - waypoints[status.current_waypoint].pos.y);

	updated_debug_info.main_sail_target_angle = sensors.wind_direction + desired_angle_of_attack();

	if (
		updated_debug_info.target_x != last_debug_info.target_x ||
		updated_debug_info.target_y != last_debug_info.target_y ||
		abs(updated_debug_info.target_angle - last_debug_info.target_angle) > 0.001 ||
		abs(updated_debug_info.target_relative_angle - last_debug_info.target_relative_angle) > 0.001 ||
		updated_debug_info.target_distance != last_debug_info.target_distance ||
		abs(updated_debug_info.main_sail_target_angle - last_debug_info.main_sail_target_angle) > 0.001
	){
		_autopilot_debug_info.publish(updated_debug_info);
	}


	last_debug_info = updated_debug_info;
}

// === Setters ===
void Autopilot::set_sensors(sand_yacht_sensors_s sensors_new){
	this->sensors = sensors_new;
}

void Autopilot::set_command(uint8_t command){
	if (command == COMMAND_START && this->status.state == STATE_READY) {
		this->status.state = STATE_DRIVING;
		this->status.current_waypoint = 0;
	} else if (command == COMMAND_STOP) {
		this->status.state = STATE_IDLE;
		this->status.error = NO_AUTOPILOT_ERROR;
	} else if (command == COMMAND_CONTINUE) this->status.error = NO_AUTOPILOT_ERROR;
	else PX4_INFO("Cannot handle Command %d. currently in state: %d", command, this->status.state);
}

void Autopilot::set_wing_direction(wing_direction_s wing_direction_new){
	old_wing_direction = this->wing_direction.wing_direction;
	this->wing_direction = wing_direction_new;
}

void Autopilot::set_land_config(sand_yacht_land_config_s land_config_new){
	this->land_config = land_config_new;
}

void Autopilot::set_course(sand_yacht_course_s course_new){
	this->courseLength = course_new.length;
	PX4_INFO("[set_course] Setting course with %d waypoints", this->courseLength);

	for (int i = 0; i < courseLength; i++) {
		waypoints[i].pos.x = course_new.x[i];
		waypoints[i].pos.y = course_new.y[i];
		waypoints[i].curve = (course_new.type[i] == 0);
	}

	if (this->status.state == STATE_IDLE && this->status.error == NO_AUTOPILOT_ERROR){
		this->status.state = STATE_READY;
	}
}

// === Debug Output ===
void Autopilot::print_status() {
	PX4_INFO("Autopilot Status:");
	PX4_INFO("Current Waypoint: %d", status.current_waypoint);
	PX4_INFO("State: %s", (status.state == STATE_IDLE) ? "Idle" :
		(status.state == STATE_READY) ? "Ready" : "Driving");
		PX4_INFO("Error: %s", (status.error == AUTOPILOT_ERROR) ? "Yes" : "No");
	PX4_INFO("Distance to waypoint(dx, dy): (%lld, %lld)",
		sensors.x - waypoints[status.current_waypoint].pos.x,
		sensors.y - waypoints[status.current_waypoint].pos.y);

	PX4_INFO("Distance to waypoint: %f",
		sqrt(pow(sensors.x - waypoints[status.current_waypoint].pos.x, 2) +
			 pow(sensors.y - waypoints[status.current_waypoint].pos.y, 2)));

	PX4_INFO("Error to waypoint: %f", (double) error);

}
