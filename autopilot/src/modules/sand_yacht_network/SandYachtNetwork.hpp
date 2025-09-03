#pragma once

#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#include <px4_platform_common/module.h>
#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>

#include <uORB/topics/sand_yacht_sensors.h>
#include <uORB/topics/sand_yacht_status.h>
#include <uORB/topics/sand_yacht_command.h>
#include <uORB/topics/sand_yacht_status.h>
#include <uORB/topics/sand_yacht_course.h>
#include <uORB/topics/sand_yacht_land_config.h>
#include <uORB/topics/sand_yacht_network_status.h>
#include <uORB/topics/autopilot_debug_info.h>


#include <pthread.h>

#include "ByteBuffer.hpp"
#include "Packets.hpp"


class SandYachtNetwork final :
	public ModuleBase<SandYachtNetwork>,
	public px4::ScheduledWorkItem
{
public:
	SandYachtNetwork(void);
	~SandYachtNetwork(void);
	SandYachtNetwork(const SandYachtNetwork &) = delete;
	SandYachtNetwork &operator=(const SandYachtNetwork &other) = delete;

	// ModuleBase
	static int task_spawn(int argc, char *argv[]);
	static int custom_command(int argc, char *argv[]);
	static int print_usage(const char *reason = nullptr);
	void Run() override;
	int print_status() override;

private:
	// uorb subscriptions
	uORB::Subscription _sensors_sub{ORB_ID(sand_yacht_sensors)}; // sensors info
	uORB::Subscription _status_sub{ORB_ID(sand_yacht_status)}; // autopilot status info
	uORB::Subscription _autopilot_debug_sub{ORB_ID(autopilot_debug_info)}; // autopilot debug info

	// uorb publications
	uORB::Publication<sand_yacht_network_status_s> _network_status_pub{ORB_ID(sand_yacht_network_status)};
	uORB::Publication<sand_yacht_course_s> _course_pub{ORB_ID(sand_yacht_course)};
	uORB::Publication<sand_yacht_command_s> _command_pub{ORB_ID(sand_yacht_command)};
	uORB::Publication<sand_yacht_land_config_s> _land_config_pub{ORB_ID(sand_yacht_land_config)};

	sand_yacht_sensors_s sensors = {0};
	sand_yacht_status_s status = {0};
	sand_yacht_wind_speed_s wind_speed = {0};
	sand_yacht_wind_dir_s wind_dir = {0};
	autopilot_debug_info_s autopilot_debug = {0};


	// internal
	sand_yacht_network_status_s network_status = {0};

	// packets
	sand_yacht_course_s course = {0};
	sand_yacht_command_s command = {0};
	sand_yacht_land_config_s land_config = {0};

	// state publish flags
	bool publish_course = false;
	bool publish_command = false;
	bool publish_config = false;

	// Os
	int fdWrite = -1;
	int fdRead = -1;

	pthread_t read_thread = -1;

	// internal
	bool initialized = false;

	// network
	bool paused = false; // debug
	uint8_t seq = 0;
	hrt_abstime last_cl_ack_time = 0;
	hrt_abstime last_update = hrt_absolute_time();

	// std::vector<uint8_t> seqsToConfirm;

	bool connected = false;
	size_t totalRead = 0;
	size_t totalWritten = 0;

	// helper buffer for reading packets
	uint8_t buffer[1024] = { 0 };
	ByteBuffer bb;


	bool init();
	bool post_init();
	bool free_resources();

	void update_and_publish_network_status(bool connected);
	void read_packets();
	void send_packet(ByteBuffer* data);

	bool check_connection();

	void poll_sensors();
	void poll_status();
	void poll_autopilot_debug_info();

	void send_sensors();
	void read_bytes(size_t size);

	void exit();

};
