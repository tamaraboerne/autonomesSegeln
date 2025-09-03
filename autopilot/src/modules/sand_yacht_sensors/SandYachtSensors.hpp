#pragma once

#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#include <px4_platform_common/module.h>
#include <uORB/Subscription.hpp>
#include <uORB/Publication.hpp>

#include <uORB/topics/sensor_gps.h>
#include <uORB/topics/sand_yacht_sensors.h>
#include <uORB/topics/sand_yacht_network_status.h>
#include <uORB/topics/sand_yacht_wind_dir.h>
#include <uORB/topics/sand_yacht_wing_dir.h>
#include <uORB/topics/sand_yacht_wind_speed.h>


// #define LOG_WIND_SPEED 0



using namespace time_literals;

struct XY_Coordinate {
	double x_m; // Meter Ost
	double y_m; // Meter Nord
};

struct GPS_Coordinate {
	double lat_deg;
	double lon_deg;
};

class SandYachtSensors final :
	public ModuleBase<SandYachtSensors>,
	public px4::ScheduledWorkItem

{
public:
	SandYachtSensors(void);
	~SandYachtSensors(void);
	SandYachtSensors(const SandYachtSensors &) = delete;
	SandYachtSensors &operator=(const SandYachtSensors &other) = delete;

	// ModuleBas
	static int task_spawn(int argc, char *argv[]);
	static int custom_command(int argc, char *argv[]);
	static int print_usage(const char *reason = nullptr);
	void Run() override;
	int print_status() override;

private:
	// GPS_Coordinate ref_point = {48.0000000, 11.0000000};

	GPS_Coordinate ref_point = {53.8345, 10.69953};

	// double EARTH_RADIUS_METERS = 6378137.0; // mittlerer Radius
	double EARTH_RADIUS_METERS = 6371000.0; // took from CoordinateSystem.java

	uORB::Subscription _gps_sub{ORB_ID(sensor_gps)};
	uORB::Subscription _sensors_wind_dir{ORB_ID(sand_yacht_wind_dir)}; // wind direction
	uORB::Subscription _sensors_wind_speed{ORB_ID(sand_yacht_wind_speed)}; // wind speed
	uORB::Subscription _sensors_wing_dir{ORB_ID(sand_yacht_wing_dir)}; // wing direction

    	uORB::Publication<sand_yacht_sensors_s> _sand_sensors_pub{ORB_ID(sand_yacht_sensors)};

	sensor_gps_s gps_data = {0};
	sand_yacht_sensors_s msg = {0};
	sand_yacht_wind_dir_s wind_dir = {0};
	sand_yacht_wind_speed_s wind_speed = {0};
	sand_yacht_wing_dir_s wing_dir = {0};

	// gps things
	double lat, lon = 0.0;
	GPS_Coordinate current = {0.0, 0.0};
	XY_Coordinate local = {0.0, 0.0};





	bool init();

	// Umrechnung GPS → lokal kartesisch (flache Projektion)
	void gps_to_local_xy(const GPS_Coordinate &ref, const GPS_Coordinate point, XY_Coordinate *out);

};
