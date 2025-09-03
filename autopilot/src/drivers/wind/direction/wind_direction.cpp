
#include "wind_direction.h"

#define WIND_DIRECTION_I2C_ADDRESS 0x44
#define WIND_DIRECTION_PERIOD 200000
#define WIND_DIRECTION_TIMEOUT 3000000
#define WIND_DIRECTION_CMD_GET_MEASUREMENT 0x80
#define WIND_DIRECTION_DATA_SIZE_BYTES 2
#define WIND_DIRECTION_DATA_SCALE 0.1f
#define WIND_DIRECTION_OFFSET (79.8f + 5.0f)

WindDirection::WindDirection(const I2CSPIDriverConfig &config) :
	I2C(config),
	ModuleParams(nullptr),
	I2CSPIDriver(config)
{
	/* empty */
}

int WindDirection::read_data(uint8_t command, uint8_t *data_ptr, uint8_t length)
{
	uint8_t raw_data[length];
	int error;

	error = transfer(&command, 1, nullptr, 0);
	if (error != PX4_OK)
	{
		return error;
	}

	error = transfer(nullptr, 0, &raw_data[0], length);
	if (error == PX4_OK)
	{
		for (int i = 0; i < length; ++i)
		{
			data_ptr[i] = raw_data[i];
		}
	}


	return error;
}

void WindDirection::sensor_compouse_msg(bool send)
{
	uint8_t data[2] = {0};
	int error = read_data(WIND_DIRECTION_CMD_GET_MEASUREMENT, &data[0], WIND_DIRECTION_DATA_SIZE_BYTES);

	if (error == PX4_OK)
	{
		measurement_time = hrt_absolute_time();
		measured_direction = (float)(data[1] << 8 | data[0]) * WIND_DIRECTION_DATA_SCALE + WIND_DIRECTION_OFFSET; //+ direction_offset;
		measured_direction = fmod(measured_direction,360.0f);

		if (send) {
			wind_direction_s msg{};
			msg.timestamp = hrt_absolute_time();

			msg.wind_direction = measured_direction;
			_wind_direction_pub.publish(msg);
		}
	} else {
		_state = wind_direction_state::ERROR_READOUT;
	}
}

int WindDirection::probe()
{
	uint8_t type[WIND_DIRECTION_DATA_SIZE_BYTES];
	uint8_t nvalid;

	nvalid = read_data(WIND_DIRECTION_CMD_GET_MEASUREMENT, &type[0], WIND_DIRECTION_DATA_SIZE_BYTES);
	return (!(nvalid == PX4_OK));  // 0 means I can see sensor
}

int WindDirection::init()
{
	if (I2C::init() != PX4_OK) {
		return PX4_ERROR;
	}

	_wind_direction_pub.advertise();
	ScheduleOnInterval(WIND_DIRECTION_PERIOD);

	return PX4_OK;
}

int WindDirection::init_sensor()
{
	px4_usleep(2000);

	probe();

	PX4_INFO("Connected to wind direction sensor");

	return PX4_OK;
}

void WindDirection::RunImpl()
{
	switch (_state)
	{
	case wind_direction_state::INIT:
		probe();
		init_sensor();
		_state = wind_direction_state::MEASUREMENT;
		break;

	case wind_direction_state::MEASUREMENT:
		if ((hrt_absolute_time() - measurement_time) > WIND_DIRECTION_PERIOD)
		{
			sensor_compouse_msg(1);
		}

		if ((hrt_absolute_time() - measurement_time) > WIND_DIRECTION_TIMEOUT)
		{
			_state = wind_direction_state::ERROR_READOUT;
		}

		break;

	case wind_direction_state::ERROR_GENERAL:
	case wind_direction_state::ERROR_READOUT: {
			if (_last_state != _state)
			{
				PX4_INFO("I cant get new data. The sensor may be disconnected.");
			}

			if (probe() == PX4_OK)
			{
				_state = wind_direction_state::INIT;
			}
		}
		break;
	}

	if (_last_state != _state)
	{
		_last_state = _state;
	}
}


void WindDirection::custom_method(const BusCLIArguments &cli)
{
	switch (cli.custom1)
	{
	case 1: {
			PX4_INFO("Last measured values (%.3fs ago)", (double)(hrt_absolute_time() - measurement_time) / 1000000.0);
			PX4_INFO("Dir: %0.3fdeg", (double)measured_direction);

		}
		break;

	case 2: {
			_state = wind_direction_state::INIT;
		}
		break;
	}
}

void WindDirection::print_status()
{
	PX4_INFO("Wind direction sensor");
	I2CSPIDriverBase::print_status();
	PX4_INFO("Status: %s", wind_direction_state_names[_state]);
}

void WindDirection::print_usage()
{
	PRINT_MODULE_DESCRIPTION(
		R"DESCR_STR(
### Description
Wind Direction Sensor Driver.

### Examples
CLI usage example:
$ wind_direction start -X
  Start the sensor driver on the external bus

$ wind_direction status
  Print driver status

$ wind_direction values
  Print last measured values

$ wind_direction reset
  Reinitialize sensor, reset flags

)DESCR_STR");

	PRINT_MODULE_USAGE_NAME("wind_direction", "driver");

	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(true, false);
	PRINT_MODULE_USAGE_PARAMS_I2C_ADDRESS(WIND_DIRECTION_I2C_ADDRESS);

	PRINT_MODULE_USAGE_PARAMS_I2C_KEEP_RUNNING_FLAG();
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();

	PRINT_MODULE_USAGE_COMMAND_DESCR("values", "Print actual data");
    	PRINT_MODULE_USAGE_COMMAND_DESCR("reset", "Reinitialize sensor");
}

int wind_direction_main(int argc, char *argv[])
{
	//int ch;
	using ThisDriver = WindDirection;
	BusCLIArguments cli{true, false};
	cli.default_i2c_frequency = 100000;
	cli.i2c_address = WIND_DIRECTION_I2C_ADDRESS;
	cli.support_keep_running = true;

	/*while ((ch = cli.getOpt(argc, argv, "o:")) != EOF) {
		switch (ch) {
		case 'o':
			float offset = (float) atof(cli.optArg());
			PX4_INFO("Offset: %f", (double) offset);
			WindDirection::set_direction_offset(offset);
			//cli.custom2 = (int) offset * 1000;
			break;
		}
	}*/

	const char *verb = cli.parseDefaultArguments(argc, argv);

	if (!verb)
	{
		ThisDriver::print_usage();
		return -1;
	}

	BusInstanceIterator iterator(MODULE_NAME, cli, DRV_WIND_DEVTYPE_WIND_DIRECTION);

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
