#include "SandYachtSensors.hpp"
#include "px4_platform_common/defines.h"

#include <px4_platform_common/log.h>
#include <cmath>


#define UPDATE_RATE 2 // Hz
#define UPDATE_INTERVAL (1000 / UPDATE_RATE)

#define OFFSET_POS_X 49.0 // Offset in cm
#define OFFSET_POS_Y 0.0 // Offset in cm

#define HEADING_OFFSET 0.0 // Offset in degrees

/*
Defines min and max values for wind speed (avoid spike or invalid values)
*/

extern "C" __EXPORT int sand_yacht_sensors_main(int argc, char *argv[]);



int sand_yacht_sensors_main(int argc, char *argv[])
{
	// <px4_platform_common/cli.h>: port parsing? timeouts numbers?

	return SandYachtSensors::main(argc, argv);
}

SandYachtSensors::SandYachtSensors(void) :
	ScheduledWorkItem(MODULE_NAME, px4::wq_configurations::test1) // TODO: nav_and_controllers correct category?
{
	PX4_INFO("SandYachtSensors constructor called");
}

SandYachtSensors::~SandYachtSensors(void)
{
	PX4_INFO("SandYachtSensors destructor called");
}

bool SandYachtSensors::init()
{
	PX4_INFO("Initializing SandYachtSensors...");
	PX4_INFO("Update interval: %d ms (%d hz)", UPDATE_INTERVAL, UPDATE_RATE);

	ScheduleOnInterval(UPDATE_INTERVAL * 1000); // ms to us

	return true;
}

int SandYachtSensors::print_status()
{
	PX4_INFO("Im running.");
	PX4_INFO("Update interval: %d ms (%d hz)", UPDATE_INTERVAL, UPDATE_RATE);
	return PX4_OK;
}

int SandYachtSensors::custom_command(int argc, char *argv[])
{
	return print_usage("unknown command");
}

int SandYachtSensors::task_spawn(int argc, char *argv[])
{
	PX4_INFO("Starting SandYachtSensors task");
	SandYachtSensors *instance = new SandYachtSensors();

	if (!instance) {
		PX4_ERR("Alloc fail SandYachtSensors");
		return PX4_ERROR;
	}

	if (!instance->init()) {
		PX4_ERR("Initialization failed");
		delete instance;
		return PX4_ERROR;
	}

	_object.store(instance);
	_task_id = task_id_is_work_queue;

	// delete instance;
	// _object.store(nullptr);
	// _task_id = -1;

	return PX4_OK;
}

// PX4 logs from this context are printed in dmesg
void SandYachtSensors::Run()
{
	if (should_exit()) {
		ScheduleClear();
		exit_and_cleanup();
		return;
	}


	if (_gps_sub.updated()) {
		if (_gps_sub.copy(&gps_data)) {
			lat = gps_data.latitude_deg; // Aus SensorGps.msg
			lon = gps_data.longitude_deg;
			current = {lat, lon};

			this->gps_to_local_xy(ref_point, current, &local);

			// PX4_INFO("Local XY: %.2f m, %.2f m", local.x_m, local.y_m);
			// PX4_INFO("GPS: lat: %.7f, lon: %.7f", lat, lon);

			// publish XY_Coordinate to topic SandYachtSensors.msg
			msg.x = (int64_t) local.x_m + (int64_t) OFFSET_POS_X;
			msg.y = (int64_t) local.y_m + (int64_t) OFFSET_POS_Y;
			msg.lat = lat;
			msg.lon = lon;
			msg.heading = ((double) gps_data.heading /  M_PI * 180.0) + HEADING_OFFSET;
			msg.vel_m_s = gps_data.vel_m_s;
		} else {
			PX4_ERR("Failed to copy GPS data????");
		}
	}

	if (_sensors_wind_dir.updated()) {
		if (_sensors_wind_dir.copy(&wind_dir)) {
			msg.wind_direction = wind_dir.wind_dir;
		} else {
			PX4_ERR("Failed to copy wind direction data????");
		}
	}

	if (_sensors_wind_speed.updated()) {
		if (_sensors_wind_speed.copy(&wind_speed)) {
			msg.wind_speed = wind_speed.m_s_speed;
		} else {
			PX4_ERR("Failed to copy wind speed data????");
		}
	}

	if (_sensors_wing_dir.updated()) {
		if (_sensors_wing_dir.copy(&wing_dir)) {
			msg.wing_surface = wing_dir.wing_dir;
		} else {
			PX4_ERR("Failed to copy wing direction data????");
		}
	}




	msg.timestamp = hrt_absolute_time();
	_sand_sensors_pub.publish(msg);
}

int SandYachtSensors::print_usage(const char *reason)
{
	if (reason) {
		PX4_WARN("%s\n", reason);
	}

	PRINT_MODULE_DESCRIPTION(
		R"DESCR_STR(
### Description
Section that describes the provided module functionality.

This is a template for a module running as a task in the background with start/stop/status functionality.

### Implementation
Section describing the high-level implementation of this module.

### Examples
CLI usage example:
$ module start -f -p 42

)DESCR_STR");

	PRINT_MODULE_USAGE_NAME("module", "Sensors");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_PARAM_FLAG('f', "Optional example flag", true);
	PRINT_MODULE_USAGE_PARAM_INT('p', 0, 0, 1000, "Optional example parameter", true);
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();

	return 0;
}

// Umrechnung GPS → lokal kartesisch (flache Projektion).
// Return the cm coordinates relative to the reference point.
void SandYachtSensors::gps_to_local_xy(const GPS_Coordinate &ref, const GPS_Coordinate point, XY_Coordinate *out) {
	//Wandelt den Breitengrad des Referenzpunkts (ref.lat_deg) von Grad in Bogenmaß (Radiant) um.
	const double lat0_rad = ref.lat_deg * M_PI / 180.0;

	//Berechnet wie weit  aktueller GPS-Punkt von ref entfernt ist?
	const double d_lat = (point.lat_deg - ref.lat_deg) * M_PI / 180.0;
	const double d_lon = (point.lon_deg - ref.lon_deg) * M_PI / 180.0;

	out->x_m = ( EARTH_RADIUS_METERS * d_lon * std::cos(lat0_rad) ) * 100; //cm
	out->y_m = ( EARTH_RADIUS_METERS * d_lat ) * 100; // cm

}
