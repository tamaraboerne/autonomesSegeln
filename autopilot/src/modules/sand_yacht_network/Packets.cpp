#include "Packets.hpp"

#include <px4_platform_common/log.h>

#define WAYPOINT_BITS 0b00011111
#define STATE_BITS 0b01100000
#define ERROR_BITS 0b10000000
/**
 * Fills packet header with magic bytes and type.
 */
void fill_packet_header(ByteBuffer* bb, uint8_t type, uint8_t seq){
	for(int i = 0; i < SYNC_PATTERN_LEN; i++){
		bb->put_i8(SYNC_PATTERN[i]);
	}

	bb->put_i8(type);
	bb->put_i8(seq);
}

/**
 * Creates a sensor packet. includes Clifton sensors, autopilot status and autopilot debug info.
 */
ByteBuffer* packet_create_sensors(
	struct sand_yacht_sensors_s* sensors,
	struct sand_yacht_status_s* status,
	struct autopilot_debug_info_s* autopilot_debug,
	uint8_t seq
){
	ByteBuffer* bb = new ByteBuffer(PACKET_SENSORS_SIZE);
	if(bb == NULL){
		PX4_ERR("ByteBuffer creation failed");
		return NULL;
	}

	fill_packet_header(bb, SENSORS, seq);

	uint8_t statusFields = (status->error & ERROR_BITS) | (status->state & STATE_BITS) | (status->waypoint & WAYPOINT_BITS);

	// autopilot status
	bb->put_i8(statusFields);

	// sensors
	bb->put_i16(sensors->wind_direction);
	bb->put_f32(sensors->wind_speed);
	bb->put_i16(sensors->wing_surface);
	bb->put_f32(sensors->vel_m_s); // velocity
	bb->put_i16(sensors->x); // x
	bb->put_i16(sensors->y); // y
	bb->put_i16(sensors->heading); // heading

	// autopilot debug info
	bb->put_f64(sensors->lat); // latitude
	bb->put_f64(sensors->lon); // longitude

	bb->put_i16(autopilot_debug->target_x);
	bb->put_i16(autopilot_debug->target_y);
	bb->put_f32(autopilot_debug->target_angle);
	bb->put_f32(autopilot_debug->target_relative_angle);
	bb->put_i16(autopilot_debug->target_distance);
	bb->put_f32(autopilot_debug->main_sail_target_angle);

	return bb;
}

/**
 * Destroys a packet, frees memory.
 */
void packet_destroy(ByteBuffer* packet){
	if(packet == NULL){
		PX4_ERR("Packet is NULL");
		return;
	}

	delete packet;
}
