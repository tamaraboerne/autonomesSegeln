/**
 * Ackermann steering model
 *
 * @author Michael BREUKER <michael.breuker@th-luebeck.de>
 */

#include "DrivingSystem.hpp"
#include <math.h>


float clip(float v, float v_min, float v_max)
{
	return fmax(fmin(v, v_max), v_min);
}


DrivingSystem::DrivingSystem(void) :
	_mode(STEERING),
	_mode_last(STEERING),
	_braking(2.0f)
{
	/* empty */
}


void DrivingSystem::update(float mainsail_control, float steering_control, float braking_control)
{
	_mode_last = _mode;

	if (braking_control < 0.0f) {
		_mode = BRAKING;

	} else {
		_mode = STEERING;
	}

	_steering.update(steering_control);

	if (_mode != _mode_last) {  // on edge
		if (_mode == BRAKING) {
			_braking.activate(
				_steering.get_left_steering_control(),
				_steering.get_right_steering_control()
			);
		}

		return;
	}

	float left_mix;
	float right_mix;

	if (_mode == BRAKING) {
		_braking.update();

		left_mix = _braking.get_left_steering_control();
		right_mix = _braking.get_right_steering_control();

	} else {
		left_mix = _steering.get_left_steering_control();
		right_mix = _steering.get_right_steering_control();
	}

	_actuator_servos.timestamp = hrt_absolute_time();
	_actuator_servos.timestamp_sample = (unsigned) time(NULL);
	_actuator_servos.control[LEFT] = clip(left_mix, -1.0f, 1.0);
	_actuator_servos.control[RIGHT] = clip(right_mix, -1.0, 1.0);
	_actuator_servos.control[MAINSAIL] = clip(mainsail_control, -1.0, 1.0);

	_actuator_servos_pub.publish(_actuator_servos);
}
