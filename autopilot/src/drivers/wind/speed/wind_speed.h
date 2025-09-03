/**
 * @file wind_speed.h
 *
 * Driver for the wind speed sensor connected via I2C.
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
#include <uORB/topics/wind_speed.h>

using namespace time_literals;

extern "C" __EXPORT int wind_speed_main(int argc, char *argv[]);

enum wind_speed_state
{
	ERROR_GENERAL,
	ERROR_READOUT,
	INIT,
	MEASUREMENT
};

const char *wind_speed_state_names[] =
{
	"General error",
	"Readout error",
	"Initialization",
	"Measurement"
};

class WindSpeed : public device::I2C, public ModuleParams, public I2CSPIDriver<WindSpeed>
{
public:
	WindSpeed(const I2CSPIDriverConfig &config);
	~WindSpeed() = default;

	static void print_usage();
	void RunImpl();

	int init() override;
	int probe() override;
	int init_sensor();
	void print_status() override;

	void custom_method(const BusCLIArguments &cli);

	int read_data(uint8_t command, uint8_t *data_ptr, uint8_t length);

	void sensor_compouse_msg(bool send);

private:
	float measured_speed = 0;
	uint32_t measurement_time = 0;

	int _state = wind_speed_state::INIT;
	int _last_state = wind_speed_state::INIT;
	uORB::PublicationMulti<wind_speed_s> _wind_speed_pub{ORB_ID(wind_speed)};
};
