/**
 * @file wind_direction.h
 *
 * Driver for the wind direction sensor connected via I2C.
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
#include <uORB/topics/wind_direction.h>
#include <math.h>

using namespace time_literals;

extern "C" __EXPORT int wind_direction_main(int argc, char *argv[]);

enum wind_direction_state
{
	ERROR_GENERAL,
	ERROR_READOUT,
	INIT,
	MEASUREMENT
};

const char *wind_direction_state_names[] =
{
	"General error",
	"Readout error",
	"Initialization",
	"Measurement"
};

class WindDirection : public device::I2C, public ModuleParams, public I2CSPIDriver<WindDirection>
{
public:
	WindDirection(const I2CSPIDriverConfig &config);
	~WindDirection() = default;

	static void print_usage();
	void RunImpl();

	int init() override;
	int probe() override;
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

	int _state = wind_direction_state::INIT;
	int _last_state = wind_direction_state::INIT;
	uORB::PublicationMulti<wind_direction_s> _wind_direction_pub{ORB_ID(wind_direction)};
};
