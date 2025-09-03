/**
 * @file wing_direction.h
 *
 * Driver for the wing direction sensor connected via I2C.
 *
 * @author Michael BREUKER <michael.breuker@th-luebeck.de>
 *
 */

#pragma once

#include <drivers/device/i2c.h>
#include <drivers/drv_hrt.h>
#include <mathlib/mathlib.h>
#include <px4_platform_common/module.h>
#include <px4_platform_common/module_params.h>
#include <px4_platform_common/i2c_spi_buses.h>
#include <uORB/PublicationMulti.hpp>
#include <uORB/topics/wing_direction.h>
#include <math.h>

using namespace time_literals;

extern "C" __EXPORT int wing_direction_main(int argc, char *argv[]);

enum wing_direction_state
{
	ERROR_GENERAL,
	ERROR_READOUT,
	INIT,
	MEASUREMENT
};

const char *wing_direction_state_names[] =
{
	"General error",
	"Readout error",
	"Initialization",
	"Measurement"
};

class WingDirection : public device::I2C, public ModuleParams, public I2CSPIDriver<WingDirection>
{
public:
	WingDirection(const I2CSPIDriverConfig &config);
	~WingDirection() = default;

	static void print_usage();
	void RunImpl();

	int init() override;
	int probe() override;
	int soft_reset();
	int init_sensor();
	void print_status() override;

	void custom_method(const BusCLIArguments &cli);

	int read_data(uint8_t command, uint8_t *data_ptr, uint8_t length);

	void sensor_compouse_msg(bool send);

	/*static float direction_offset;

	static void set_direction_offset(float offset_value) {
		direction_offset = offset_value;
	}*/

private:
	float measured_direction = 0;
	uint32_t measurement_time = 0;

	int _state = wing_direction_state::INIT;
	int _last_state = wing_direction_state::INIT;
	uORB::PublicationMulti<wing_direction_s> _wing_direction_pub{ORB_ID(wing_direction)};
};
