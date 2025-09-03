/**
 * Brake system
 *
 * @author Michael BREUKER <michael.breuker@th-luebeck.de>
 */

#include "Brake.hpp"
#include <drivers/drv_hrt.h>


Brake::Brake(float duration) :
	_active(false),
	_duration(duration),
	_left_position(0.0f),
	_right_position(0.0f),
	_left_position_start(0.0f),
	_right_position_start(0.0f),
	_left_position_delta(0.0f),
	_right_position_delta(0.0f),
	_timestamp(0)
{
	/* empty */
}


void Brake::activate(float left_position, float right_position)
{
	_active = true;

	_left_position_start = left_position;
	_left_position_delta = LEFT_POSITION_TARGET - _left_position_start;

	_right_position_start = right_position;
	_right_position_delta = RIGHT_POSITION_TARGET  - _right_position_start;

	_timestamp = hrt_absolute_time();
}


void Brake::update(void)
{
	if (!_active) {
		return;
	}

	uint64_t now = hrt_absolute_time();
	float elapsed = (float(now) - float(_timestamp)) / 1e6f;  // convert to [sec]

	if (elapsed >= _duration) {
		_left_position = LEFT_POSITION_TARGET;
		_right_position = RIGHT_POSITION_TARGET;
		_active = false;

	} else {
		float progress = elapsed / _duration;

		_left_position = _left_position_start + _left_position_delta * progress;
		bool left_finished = is_positioning_finished(_left_position, LEFT_POSITION_TARGET);

		_right_position = _right_position_start + _right_position_delta * progress;
		bool right_finished = is_positioning_finished(_right_position, RIGHT_POSITION_TARGET);

		_active = !(left_finished && right_finished);
	}
}


bool Brake::is_positioning_finished(float &position, float position_target)
{
	if (abs(position) >= abs(position_target)) {
		position = position_target;
		return true;
	}

	return false;
}


bool Brake::is_active(void) const
{
	return _active;
}


float Brake::get_left_steering_control(void) const
{
	return _left_position;
}


float Brake::get_right_steering_control(void) const
{
	return _right_position;
}
