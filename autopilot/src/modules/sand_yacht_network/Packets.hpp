#pragma once

#include <uORB/topics/sand_yacht_sensors.h>
#include <uORB/topics/sand_yacht_status.h>
#include <uORB/topics/sand_yacht_wind_speed.h>
#include <uORB/topics/sand_yacht_wind_dir.h>
#include <uORB/topics/autopilot_debug_info.h>

#include "ByteBuffer.hpp"

#define SYNC_PATTERN_LEN 5
#define PACKET_HEADER_SIZE (2 + SYNC_PATTERN_LEN)
#define PACKET_SENSORS_SIZE (PACKET_HEADER_SIZE + PACKET_SENSORS_PAYLOAD_SIZE)

#define COURSE_PACKET_POINT_SIZE 5

#define PACKET_SENSORS_PAYLOAD_SIZE 53

const uint8_t SYNC_PATTERN[SYNC_PATTERN_LEN] = {0x12, 0x24, 0x36, 0x48, 0x60};

enum PacketType {
	SENSORS = 1,
	COURSE = 2,
	COMMAND = 3,
	CONFIG = 4,
	INTERNAL_ACK = 10 // laptop -> clifton
};

// ==============================================
// Packet Helper functions
// ==============================================

ByteBuffer* packet_create_sensors(
	struct sand_yacht_sensors_s* sensors,
	struct sand_yacht_status_s* status,
	struct autopilot_debug_info_s* autopilot_status,
	uint8_t seq
);

void packet_destroy(ByteBuffer* packet);
