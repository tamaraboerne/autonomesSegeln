/**
 * Ackermann steering model
 *
 * @author Michael BREUKER <michael.breuker@th-luebeck.de>
 */

#pragma once

#include "Ackermann.hpp"
#include "Brake.hpp"
#include <uORB/Publication.hpp>
#include <uORB/topics/actuator_servos.h>


class DrivingSystem
{
	enum SERVO_CHANNELS {
		LEFT = 0,
		RIGHT = 1,
		MAINSAIL = 2
	};

	typedef enum {
		STEERING = 0,
		BRAKING = 1
	} DrivingMode;
public:
	DrivingSystem(void);

	void update(float mainsail_control, float steering_control, float braking_control);
private:
	// actuator servo publication
	uORB::Publication<actuator_servos_s> _actuator_servos_pub{ORB_ID(actuator_servos)};

	// servo data
	actuator_servos_s _actuator_servos{};

private:
	DrivingMode _mode;
	DrivingMode _mode_last;

	Brake _braking;
	Ackermann _steering;
};
