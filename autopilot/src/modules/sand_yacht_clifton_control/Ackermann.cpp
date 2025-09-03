/**
 * Ackermann steering system
 *
 * @author Michael BREUKER <michael.breuker@th-luebeck.de>
 */

#include "Ackermann.hpp"
#include <math.h>

float map(float x, float in_min, float in_max, float out_min, float out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float map_mid(float x, float in_min, float in_mid, float in_max, float out_min, float out_mid, float out_max) {
	if (x >= 0.0f) {
        	return map(x, in_mid, in_max, out_mid, out_max);
	} else {
        	return map(x, in_min, in_mid, out_min, out_mid);
	}
}

Ackermann::Ackermann(void) :
	_left_steering_control(0.0f),
	_right_steering_control(0.0f)
{
	/* empty */
}

void Ackermann::update(float steering_control, float epsilon) {
	float steering_angle = control_to_angle(steering_control, STEERING_ANGLE_MIN, STEERING_ANGLE_MAX);

	if (fabsf(steering_angle) < epsilon) {
		_left_steering_control = 0.0f;
		_right_steering_control = 0.0f;
		return;
	}

	float radius = WHEEL_BASE / tanf(steering_angle);

	float left_steering_angle;
	float right_steering_angle;
	if (radius >= 0.0f) {
		left_steering_angle = get_inner_steering_angle(radius);
		right_steering_angle = get_outer_steering_angle(radius);
	} else {
		left_steering_angle = -get_outer_steering_angle(-radius);
		right_steering_angle = -get_inner_steering_angle(-radius);
	}

	_left_steering_control = angle_to_control(left_steering_angle, LEFT_ANGLE_MIN, LEFT_ANGLE_MAX);
	_right_steering_control = angle_to_control(right_steering_angle, RIGHT_ANGLE_MIN, RIGHT_ANGLE_MAX);
}

float Ackermann::get_left_steering_control(void) const {
	return _left_steering_control;
}

float Ackermann::get_right_steering_control(void) const {
	return _right_steering_control;
}

float Ackermann::get_outer_steering_angle(float radius) const {
	return atanf(WHEEL_BASE / (radius + (TRACK_WIDTH / 2.0f)));
}

float Ackermann::get_inner_steering_angle(float radius) const {
	return atanf(WHEEL_BASE / (radius - (TRACK_WIDTH / 2.0f)));
}

void Ackermann::set_right_steering_control(float rad) {
	_right_steering_control = rad;
}

void Ackermann::set_left_steering_control(float rad) {
	_left_steering_control = rad;
}

float Ackermann::control_to_angle(float control, float angle_min, float angle_max) const {
	return map_mid(control, -1.0, 0.0, 1.0, angle_min, 0.0, angle_max);
}

float Ackermann::angle_to_control(float angle, float angle_min, float angle_max) const {
	return map_mid(angle, angle_min, 0.0, angle_max, -1.0, 0.0, 1.0);
}
