
#include "wing_direction.h"
#include <mathlib/mathlib.h>

#define WING_DIRECTION_I2C_ADDRESS 0x0D
#define WING_DIRECTION_PERIOD 200000
#define WING_DIRECTION_TIMEOUT 3000000
#define WING_DIRECTION_CMD_GET_MEASUREMENT 0x00
#define WING_DIRECTION_DATA_SIZE_BYTES 6
#define WING_DIRECTION_OFFSET (-37.5f + 0.0f)

// Modes
#define WING_DIRECTION_MODE_STANDBY  0b00000000
#define WING_DIRECTION_MODE_CONTINUOUS 0b00000001

// Output data rates
#define WING_DIRECTION_ODR_10HZ 0b00000000
#define WING_DIRECTION_ODR_50HZ 0b00000100
#define WING_DIRECTION_ODR_100HZ 0b00001000
#define WING_DIRECTION_ODR_200HZ 0b00001100

// Measure ranges
#define WING_DIRECTION_RANGE_2G 0b00000000
#define WING_DIRECTION_RANGE_8G 0b00010000

// Over sampling rates
#define WING_DIRECTION_OSR_512 0b00000000
#define WING_DIRECTION_OSR_256 0b01000000
#define WING_DIRECTION_OSR_128 0b10000000
#define WING_DIRECTION_OSR_64 0b11000000


WingDirection::WingDirection(const I2CSPIDriverConfig &config) :
	I2C(config),
	ModuleParams(nullptr),
	I2CSPIDriver(config)
{
	/* empty */
}

int WingDirection::read_data(uint8_t command, uint8_t *data_ptr, uint8_t length)
{
	uint8_t raw_data[length];
	int error;

	error = transfer(&command, 1, nullptr, 0);

	if (error != PX4_OK) {
		return error;
	}

	error = transfer(nullptr, 0, &raw_data[0], length);

	if (error == PX4_OK) {
		for (int i = 0; i < length; ++i) {
			data_ptr[i] = raw_data[i];
		}
	}

	return error;
}

void WingDirection::sensor_compouse_msg(bool send)
{
	uint8_t data[6] = {0};
	int error = read_data(WING_DIRECTION_CMD_GET_MEASUREMENT, &data[0], WING_DIRECTION_DATA_SIZE_BYTES);

	if (error == PX4_OK) {
		int16_t x = (data[1] << 8 | data[0]);
		int16_t z = (data[5] << 8 | data[4]);

		measurement_time = hrt_absolute_time();

		measured_direction = math::degrees(atan2f(z, x)) + WING_DIRECTION_OFFSET;

		// convert to absolute angle
		measured_direction = fmod(measured_direction + 360.0f, 360.0f);

		if (send) {
			wing_direction_s msg{};
			msg.timestamp = hrt_absolute_time();

			msg.wing_direction = measured_direction;
			_wing_direction_pub.publish(msg);
		}

	} else {
		_state = wing_direction_state::ERROR_READOUT;
	}
}

int WingDirection::probe()
{
	uint8_t type[WING_DIRECTION_DATA_SIZE_BYTES];
	uint8_t nvalid;

	nvalid = read_data(WING_DIRECTION_CMD_GET_MEASUREMENT, &type[0], WING_DIRECTION_DATA_SIZE_BYTES);
	return (!(nvalid == PX4_OK));  // 0 means I can see sensor
}

int WingDirection::soft_reset()
{
	uint8_t reset_cmd[2] = {0x0A, 0x80};
	uint8_t reset_period_cmd[2] = {0x0B, 0x01};

	int error = transfer(reset_cmd, 2, nullptr, 0);

	if (error != PX4_OK) {
		return error;
	}

	return transfer(reset_period_cmd, 2, nullptr, 0);
}

int WingDirection::init()
{
	if (I2C::init() != PX4_OK) {
		return PX4_ERROR;
	}

	_wing_direction_pub.advertise();
	ScheduleOnInterval(WING_DIRECTION_PERIOD);

	return PX4_OK;
}

int WingDirection::init_sensor()
{
	px4_usleep(2000);

	probe();

	PX4_INFO("Connected to wing direction sensor");

	int error = soft_reset();

	if (error != PX4_OK) {
		PX4_INFO("Soft-Reset failed");
		return error;
	}

	uint8_t configuration_cmd[2] = {
		0x09,
		WING_DIRECTION_OSR_128 | WING_DIRECTION_RANGE_8G |
		WING_DIRECTION_ODR_50HZ | WING_DIRECTION_MODE_CONTINUOUS
	};

	error = transfer(configuration_cmd, 2, nullptr, 0);

	if (error != PX4_OK) {
		PX4_INFO("Setting configuration failed");
		return error;
	}

	return error;
}

void WingDirection::RunImpl()
{
	switch (_state) {
	case wing_direction_state::INIT: {
		int error = init_sensor();

		if (error != PX4_OK) {
			PX4_INFO("Problem during initialization.");
			_state = wing_direction_state::INIT;
			px4_usleep(5000);
		}
		else
		{
			_state = wing_direction_state::MEASUREMENT;
		}
		break;
	}

	case wing_direction_state::MEASUREMENT:
		if ((hrt_absolute_time() - measurement_time) > WING_DIRECTION_PERIOD) {
			sensor_compouse_msg(1);
		}

		if ((hrt_absolute_time() - measurement_time) > WING_DIRECTION_TIMEOUT) {
			_state = wing_direction_state::ERROR_READOUT;
		}

		break;

	case wing_direction_state::ERROR_GENERAL:
	case wing_direction_state::ERROR_READOUT: {
			if (_last_state != _state) {
				PX4_INFO("I cant get new data. The sensor may be disconnected.");
			}

			if (probe() == PX4_OK) {
				_state = wing_direction_state::INIT;
			}
		}
		break;
	}

	if (_last_state != _state) {
		_last_state = _state;
	}
}


void WingDirection::custom_method(const BusCLIArguments &cli)
{
	switch (cli.custom1) {
	case 1: {
			PX4_INFO("Last measured values (%.3fs ago)", (double)(hrt_absolute_time() - measurement_time) / 1000000.0);
			PX4_INFO("Dir: %0.3fdeg", (double)measured_direction);

		}
		break;

	case 2: {
			_state = wing_direction_state::INIT;
		}
		break;
	}
}

void WingDirection::print_status()
{
	PX4_INFO("Wing direction sensor");
	I2CSPIDriverBase::print_status();
	PX4_INFO("Status: %s", wing_direction_state_names[_state]);
}

void WingDirection::print_usage()
{
	PRINT_MODULE_DESCRIPTION(
		R"DESCR_STR(
### Description
Wing Direction Sensor Driver.

### Examples
CLI usage example:
$ wing_direction start -X
  Start the sensor driver on the external bus

$ wing_direction status
  Print driver status

$ wing_direction values
  Print last measured values

$ wing_direction reset
  Reinitialize sensor, reset flags

)DESCR_STR");

	PRINT_MODULE_USAGE_NAME("wing_direction", "driver");

	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(true, false);
	PRINT_MODULE_USAGE_PARAMS_I2C_ADDRESS(WING_DIRECTION_I2C_ADDRESS);

	PRINT_MODULE_USAGE_PARAMS_I2C_KEEP_RUNNING_FLAG();
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();

	PRINT_MODULE_USAGE_COMMAND_DESCR("values", "Print actual data");
    	PRINT_MODULE_USAGE_COMMAND_DESCR("reset", "Reinitialize sensor");
}

int wing_direction_main(int argc, char *argv[])
{
	//int ch;
	using ThisDriver = WingDirection;
	BusCLIArguments cli{true, false};
	cli.default_i2c_frequency = 100000;
	cli.i2c_address = WING_DIRECTION_I2C_ADDRESS;
	cli.support_keep_running = true;

	const char *verb = cli.parseDefaultArguments(argc, argv);

	if (!verb)
	{
		ThisDriver::print_usage();
		return -1;
	}

	BusInstanceIterator iterator(MODULE_NAME, cli, DRV_WING_DEVTYPE_WING_DIRECTION);

	if (!strcmp(verb, "start"))
	{
		return ThisDriver::module_start(cli, iterator);
	}

	if (!strcmp(verb, "stop"))
	{
		return ThisDriver::module_stop(iterator);
	}

	if (!strcmp(verb, "status"))
	{
		return ThisDriver::module_status(iterator);
	}

	if (!strcmp(verb, "values"))
	{
		cli.custom1 = 1;
		return ThisDriver::module_custom_method(cli, iterator, false);
	}

	if (!strcmp(verb, "reset"))
	{
		cli.custom1 = 2;
		return ThisDriver::module_custom_method(cli, iterator, false);
	}

	ThisDriver::print_usage();
	return -1;
}
