/**
 * Ackermann steering system
 *
 * @author Michael BREUKER <michael.breuker@th-luebeck.de>
 */

#pragma once

#include <math.h>

inline float to_radians(float degrees)
{
	return degrees * (float)(M_PI / 180.0);
}

class Ackermann
{
public:
	Ackermann(void);

	// update model
	void update(float steering_control, float epsilon = 1e-6f);

	// getter
	float get_left_steering_control(void) const;
	float get_right_steering_control(void) const;

	// setter
	void set_right_steering_control(float rad);
	void set_left_steering_control(float rad);
private:
	// calculate outer steering angle [rad]
	float get_outer_steering_angle(float radius) const;

	// calculate inner steering angle [rad]
	float get_inner_steering_angle(float radius) const;

	// convert control values (-1..+1) to angular values [rad]
	float control_to_angle(float control, float angle_min, float angle_max) const;

	// convert angular values [rad]] to control values (-1..+1)
	float angle_to_control(float angle, float angle_min, float angle_max) const;

	// current state
	float _left_steering_control;
	float _right_steering_control;

	// wheel base in [cm]. Distance between front and rear axle
	const float WHEEL_BASE = 118.5f;

	// track width in [cm]. Distance between left and right wheel
	const float TRACK_WIDTH = 93.5f;

	// minimum steering angle [rad]
	const float STEERING_ANGLE_MIN = to_radians(-30.0f);

	// maximum steering angle [rad]
	const float STEERING_ANGLE_MAX = to_radians(30.0f);

	// minimum angle of left wheel [rad]
	const float LEFT_ANGLE_MIN = to_radians(-43.0f);

	// maximum angle of left wheel [rad]
	const float LEFT_ANGLE_MAX = to_radians(38.0f);

	// minimum angle of right wheel [rad]
	const float RIGHT_ANGLE_MIN = to_radians(-42.0f);

	// maximum angle of right wheel [rad]
	const float RIGHT_ANGLE_MAX = to_radians(42.0);
};
