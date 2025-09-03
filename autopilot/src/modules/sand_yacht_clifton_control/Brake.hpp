/**
 * Brake system
 *
 * @author Michael BREUKER <michael.breuker@th-luebeck.de>
 */

#pragma once


class Brake
{
public:
	Brake(float duration);

	// activate brakes
	void activate(float left_position, float right_position);

	// update brakes
	void update(void);

	// getter
	bool is_active(void) const;
	float get_left_steering_control(void) const;
	float get_right_steering_control(void) const;
private:
	bool is_positioning_finished(float &position, float position_target);
private:
	bool _active;
	float _duration;
	float _left_position;
	float _right_position;
	float _left_position_start;
	float _right_position_start;
	float _left_position_delta;
	float _right_position_delta;
	uint64_t _timestamp;

	const float LEFT_POSITION_TARGET = -1.0f;
	const float RIGHT_POSITION_TARGET = 1.0f;
};
