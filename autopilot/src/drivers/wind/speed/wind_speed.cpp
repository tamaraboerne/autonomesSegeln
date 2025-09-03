
#include "wind_speed.h"

#define WIND_SPEED_I2C_ADDRESS 0x45
#define WIND_SPEED_PERIOD 200000
#define WIND_SPEED_TIMEOUT 3000000
#define WIND_SPEED_CMD_GET_MEASUREMENT 0x80
#define WIND_SPEED_DATA_SIZE_BYTES 2
#define WIND_SPEED_DATA_SCALE 0.1f


WindSpeed::WindSpeed(const I2CSPIDriverConfig &config) :
	I2C(config),
	ModuleParams(nullptr),
	I2CSPIDriver(config)
{
	/* empty */
}

int WindSpeed::read_data(uint8_t command, uint8_t *data_ptr, uint8_t length)
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

void WindSpeed::sensor_compouse_msg(bool send)
{
	uint8_t data[2];
	int error = read_data(WIND_SPEED_CMD_GET_MEASUREMENT, &data[0], WIND_SPEED_DATA_SIZE_BYTES);

	if (error == PX4_OK)
	{
		measurement_time = hrt_absolute_time();
		measured_speed = (float) (data[1] << 8 | data[0]) * WIND_SPEED_DATA_SCALE;

		if (send)
		{
			wind_speed_s msg{};
			msg.timestamp = hrt_absolute_time();

			msg.wind_speed = measured_speed;
			_wind_speed_pub.publish(msg);
		}
	}
}

int WindSpeed::probe()
{
	uint8_t type[WIND_SPEED_DATA_SIZE_BYTES];
	uint8_t nvalid;

	nvalid = read_data(WIND_SPEED_CMD_GET_MEASUREMENT, &type[0], WIND_SPEED_DATA_SIZE_BYTES);
	return (!(nvalid == PX4_OK));  // 0 means I can see sensor
}

int WindSpeed::init()
{
	if (I2C::init() != PX4_OK) {
		return PX4_ERROR;
	}

	_wind_speed_pub.advertise();
	ScheduleOnInterval(WIND_SPEED_PERIOD);

	return PX4_OK;
}

int WindSpeed::init_sensor()
{
	px4_usleep(2000);

	probe();

	PX4_INFO("Connected to wind speed sensor");

	return PX4_OK;
}

void WindSpeed::RunImpl()
{
	switch (_state)
	{
	case wind_speed_state::INIT:
		probe();
		init_sensor();
		_state = wind_speed_state::MEASUREMENT;
		break;

	case wind_speed_state::MEASUREMENT:
		if ((hrt_absolute_time() - measurement_time) > WIND_SPEED_PERIOD)
		{
			sensor_compouse_msg(1);
		}

		if ((hrt_absolute_time() - measurement_time) > WIND_SPEED_TIMEOUT)
		{
			_state = wind_speed_state::ERROR_READOUT;
		}

		break;

	case wind_speed_state::ERROR_GENERAL:
	case wind_speed_state::ERROR_READOUT: {
			if (_last_state != _state)
			{
				PX4_INFO("I cant get new data. The sensor may be disconnected.");
			}

			if (probe() == PX4_OK)
			{
				_state = wind_speed_state::INIT;
			}
		}
		break;
	}

	if (_last_state != _state)
	{
		_last_state = _state;
	}
}


void WindSpeed::custom_method(const BusCLIArguments &cli)
{
	switch (cli.custom1)
	{
	case 1: {
			PX4_INFO("Last measured values (%.3fs ago)", (double)(hrt_absolute_time() - measurement_time) / 1000000.0);
			PX4_INFO("Speed: %0.3fcm/s", (double)measured_speed);

		}
		break;

	case 2: {
			_state = wind_speed_state::INIT;
		}
		break;
	}
}

void WindSpeed::print_status()
{
	PX4_INFO("Wind speed sensor");
	I2CSPIDriverBase::print_status();
	PX4_INFO("Status: %s", wind_speed_state_names[_state]);
}

void WindSpeed::print_usage()
{
	PRINT_MODULE_DESCRIPTION(
		R"DESCR_STR(
### Description
Wind Speed Sensor Driver.

### Examples
CLI usage example:
$ wind_speed start -X
  Start the sensor driver on the external bus

$ wind_speed status
  Print driver status

$ wind_speed values
  Print last measured values

$ wind_speed reset
  Reinitialize sensor, reset flags

)DESCR_STR");

	PRINT_MODULE_USAGE_NAME("wind_speed", "driver");

	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(true, false);
	PRINT_MODULE_USAGE_PARAMS_I2C_ADDRESS(WIND_SPEED_I2C_ADDRESS);

	PRINT_MODULE_USAGE_PARAMS_I2C_KEEP_RUNNING_FLAG();
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();

	PRINT_MODULE_USAGE_COMMAND_DESCR("values", "Print actual data");
    	PRINT_MODULE_USAGE_COMMAND_DESCR("reset", "Reinitialize sensor");
}

int wind_speed_main(int argc, char *argv[])
{
	using ThisDriver = WindSpeed;
	BusCLIArguments cli{true, false};
	cli.default_i2c_frequency = 100000;
	cli.i2c_address = WIND_SPEED_I2C_ADDRESS;
	cli.support_keep_running = true;

	const char *verb = cli.parseDefaultArguments(argc, argv);

	if (!verb)
	{
		ThisDriver::print_usage();
		return -1;
	}

	BusInstanceIterator iterator(MODULE_NAME, cli, DRV_WIND_DEVTYPE_WIND_SPEED);

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
