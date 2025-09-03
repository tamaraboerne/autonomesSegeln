/**
 * Autopilot
 *
 * @author SS2025 Clifton Group
 */

#pragma once

// #include "Ackermann.hpp"
// #include "Brake.hpp"
 #include <math.h>
 #include <uORB/Publication.hpp>
 #include <uORB/Subscription.hpp>
 #include <uORB/topics/sand_yacht_status.h>
 #include <uORB/topics/sand_yacht_sensors.h>
 #include <uORB/topics/sand_yacht_command.h>
 #include <uORB/topics/sand_yacht_course.h>
 #include <uORB/topics/dummy.h>
 #include <uORB/topics/wing_direction.h>
 #include <uORB/topics/sand_yacht_land_config.h>
 #include <uORB/topics/autopilot_debug_info.h>




typedef struct {
	float steering;
	float mainsail;
	float breaking;
} Controls;

typedef struct {
	int16_t x;
	int16_t y;
} Position;

typedef struct {
	Position pos;
	bool curve;
} Waypoint;

typedef struct {
	u_int8_t current_waypoint;
	u_int8_t state;
	bool error;
} Status;


class Autopilot
{
public:
	Autopilot();

	/**
	 * @brief Main method to calculate steering, sail and brake control signals.
	 *
	 * @return Controls struct containing control values for steering, mainsail, and brake.
	 */
	Controls calculateControls(void);

	/**
	 * @brief Returns the closest waypoint index for a given (x, y) position.
	 *
	 * @param x X-coordinate
	 * @param y Y-coordinate
	 * @return Index of the closest waypoint
	 */
	uint8_t pointToCourse(int32_t x, int32_t y);

	/**
	 * @brief Method to inject the sensordata into the Object and
	 * sets the instance variable sensors to the provided sensor data
	 *
	 * @param sensors new sensordata
	 *
	 */
	void set_sensors(sand_yacht_sensors_s sensors);

	/**
	 * @brief Sets the current command for the Autopilot (e.g., STOP, START).
	 *
	 * @param command Command code
	 */
	void set_command(uint8_t command);

	/**
	 * @brief Sets the course (list of waypoints) for the Autopilot to follow.
	 *
	 * @param course Course containing waypoints and length
	 */
	void set_course(sand_yacht_course_s course);

	/**
	 * @brief Updates the current wind direction measurement.
	 *
	 * @param wing_direction Wing direction sensor reading
	 */
	void set_wing_direction(wing_direction_s wing_direction);

	/**
	 * @brief Sets runtime configuration parameters from land configuration message.
	 *
	 * @param land_config Configuration struct for autopilot behavior
	 */
	void set_land_config(sand_yacht_land_config_s land_config);

	/**
	 * @brief Manually sets the current state of the Autopilot.
	 *
	 * @param _state New internal state
	 */
	void update_state(uint8_t _state);

	/**
	 * @brief Prints internal status for debugging purposes.
	 */
	void print_status();


private:

	/**
	 * @brief Regulates the flap position based on sail movement speed and wind alignment.
	 *
	 * @return Flap control value (normalized between -1 and 1)
	 */
	float flap_controller();

	/**
	 * @brief Proportional steering control using heading error.
	 *
	 * @return Normalized steering value (-1 to 1)
	 */
	float steering_control();

	/**
	 * @brief Updates and publishes the current autopilot status.
	 *
	 * This method updates the `sand_yacht_status_s` message, which reflects the internal
	 * status of the autopilot, including:
	 * - The currently active waypoint index (limited to 5 bits).
	 * - The internal autopilot state (e.g., navigating, waiting, finished).
	 * - An error flag indicating whether the system is in a valid state or not.
	 *
	 * The timestamp is updated to the current system time.
	 *
	 * The status message is only published if one or more of the following have changed
	 * since the last update:
	 * - The waypoint index.
	 * - The autopilot state.
	 * - The error condition.
	 *
	 * This conditional publishing minimizes communication overhead by avoiding redundant messages.
	 */
	void update_status();

	/**
	 * @brief Checks if the current waypoint has been reached and updates the autopilot state.
	 *
	 * Compares the current position to the target waypoint using a distance threshold.
	 * If the waypoint is reached and the autopilot is in driving mode, the next waypoint is activated.
	 * If the final waypoint is reached, the autopilot switches to idle mode.
	 */
	void testWaypointReached();

	/**
	 * @brief Calculates the desired angle between wind and wing (angle of attack).
	 *
	 * @return Target angle of attack
	 */
	double desired_angle_of_attack();

	/**
	 * @brief Calculates the angle between two positions (used for navigation).
	 *
	 * @param current Current position
	 * @param target Target waypoint
	 * @return Angle to target in degrees
	 */
	float calculate_angle(Position current, Position target);

	/**
	 * @brief Publishes debug information for external UI tools or logging.
	 *
	 * This method prepares and publishes a `autopilot_debug_info_s` message containing
	 * key navigation and control information useful for debugging or visualization.
	 * It includes the target waypoint position, distance to target, desired sail angle,
	 * and heading angles relative to the boat's current position.
	 *
	 * The message is only published if relevant fields have changed significantly
	 * compared to the last published state, to reduce unnecessary data transmission.
	 *
	 * Published fields:
	 * - target_x / target_y: Coordinates of the current target waypoint.
	 * - target_angle: Absolute angle from current position to the waypoint.
	 * - target_relative_angle: Difference between boat heading and target angle.
	 * - target_distance: Manhattan distance to the target waypoint.
	 * - main_sail_target_angle: Desired main sail angle based on wind and angle of attack.
	 */
	void publish_debug_info();


	// uORB Publications
	uORB::Publication<sand_yacht_status_s> _autopilot_status{ORB_ID(sand_yacht_status)};
	uORB::Publication<autopilot_debug_info_s> _autopilot_debug_info{ORB_ID(autopilot_debug_info)};

	// Sensor and config data
	wing_direction_s wing_direction = {0};
	sand_yacht_sensors_s sensors = {0};
	// Autopilot status
	sand_yacht_status_s updated_status = {0};
	sand_yacht_status_s last_status = {0};
	// Configuration parameters
	sand_yacht_land_config_s land_config = {
		.timestamp = 0,
		.sail_speed_tolerance = 0.0f,
		.sail_speed_constant = 1.0f,
		.steering_constant_p = -1.0f,
		.flap_step = 0.05f,
		.default_waypoint_tolerance = 50,
		.curve_waypoint_tolerance = 75,
	};


	Status status = {0};
	uint8_t courseLength = 0;
	Waypoint waypoints[32] = {0};

	// Internal steering limits
	float max_steering_step = 0.5;

	// Previous wing direction
	float old_wing_direction = 0;

	// Actuator states
	float current_flap;
	float current_steering;

};
