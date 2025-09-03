#include "SandYachtNetwork.hpp"
#include "SerialPort.hpp"
#include <px4_platform_common/log.h>
// #include <px4_platform_common/getopt.h>
// #include <px4_platform_common/posix.h>


#define UPDATE_RATE 5 // Hz
#define UPDATE_INTERVAL (1000 / UPDATE_RATE) // ms

// time in seconds to wait for a connection
#define MAX_DELAY 3


#define ENABLE_SEND
#define MISSMATCH_DEBUG
#define FIND_SYNC_PATTERN // DEBUG: defined = find, not defined = skip finding sync pattern.

extern "C" __EXPORT int sand_yacht_network_main(int argc, char *argv[]);

/**
 * Reads stream of bytes from the serial port and trying to find magic header.
 * Blocking function, will wait until the header is found or other error occurs.
 */
bool find_sync_pattern(int fd) {
    ssize_t r;
    int i = 0;
    bool matched = false;
    uint8_t buffer[SYNC_PATTERN_LEN];


    // initial fill of the buffer, so we can check and do sliding later.
    for (i = 0; i < SYNC_PATTERN_LEN; ++i) {
        r = readPort(fd, &buffer[i], 1);
        if (r != 1) return false;
    }

    while (true) {
        matched = true;
        for (i = 0; i < SYNC_PATTERN_LEN; ++i) {
            if (buffer[i] != SYNC_PATTERN[i]) {
                matched = false;
                break;
            }
        }
        if (matched){
		return true;
	}

        for (i = 0; i < SYNC_PATTERN_LEN - 1; ++i) {
            buffer[i] = buffer[i + 1];
        }
        r = readPort(fd, &buffer[SYNC_PATTERN_LEN - 1], 1);
	if (r != 1) {
		PX4_ERR("Failed to read from serial port: %s", strerror(errno));
		return false; // read error
	}
    }

    return false;
}



int sand_yacht_network_main(int argc, char *argv[])
{
	return SandYachtNetwork::main(argc, argv);
}

SandYachtNetwork::SandYachtNetwork(void) :
	ScheduledWorkItem(MODULE_NAME, px4::wq_configurations::nav_and_controllers) // TODO: nav_and_controllers correct category?
{
	PX4_INFO("SandYachtNetwork constructor called");
}

SandYachtNetwork::~SandYachtNetwork(void)
{
	PX4_INFO("SandYachtNetwork destructor called");
}

bool SandYachtNetwork::init(){
	PX4_INFO("Initializing SandYachtNetwork...");
	PX4_INFO("Addr (this): %p", this);
	PX4_INFO("Update interval: %d ms (%d hz)", UPDATE_INTERVAL, UPDATE_RATE);

	ScheduleOnInterval(UPDATE_INTERVAL * 1000); // ms to us

	return true;
}

/**
 * init() function is called in other context, so we cannot initialize all OS things there.
 */
bool SandYachtNetwork::post_init(){
	if(fdRead == -1){
		fdRead = initPort(false);
		PX4_INFO("Serial Port FD Read: %d", fdRead);
	}

	if(fdWrite == -1){
		fdWrite = initPort(true);
		PX4_INFO("Serial Port FD Write: %d", fdWrite);
	}

	if(read_thread == -1){
		// anonymous function as a thread entry point which calls read_packet method
		// using anonymous function to pass 'this' context to the thread and call read_packets correct.
		int ret = pthread_create(&read_thread, nullptr, [](void* self) -> void* {
			(static_cast<SandYachtNetwork*>(self))->read_packets();
			return nullptr;
		}, this);

		if (ret != 0) {
			PX4_ERR("Failed to create read thread: %s", strerror(ret));
			return false;
		}
		PX4_INFO("Read thread: %d", read_thread);
	}

	return true;
}

bool SandYachtNetwork::free_resources(){
	PX4_INFO("Stopping SandYachtNetwork...");
	if (read_thread != -1) {
		PX4_INFO("Requesting read thread to stop");

		if (pthread_cancel(read_thread) != 0) {
			PX4_ERR("Failed to cancel read thread: %s", strerror(errno));
			return false;
		}

		PX4_INFO("Waiting for read thread to finish");
		if (pthread_join(read_thread, nullptr) != 0) {
			PX4_ERR("Failed to join read thread: %s", strerror(errno));
			return false;
		}
	}

	if (fdRead != -1 && !closePort(fdRead)) {
		PX4_ERR("Failed to close serial port???");
		return false;
	}

	if (fdWrite != -1 && !closePort(fdWrite)) {
		PX4_ERR("Failed to close serial port???");
		return false;
	}



	fdRead = -1;
	fdWrite = -1;
	read_thread = -1;

	return true;
}


int SandYachtNetwork::print_status()
{
	int secAgo  = ( hrt_absolute_time() - last_cl_ack_time ) / 1000000;

	PX4_INFO("Seq: %d", seq);
	PX4_INFO("Connected: %s", connected ? "true" : "false");
	PX4_INFO("Paused: %s", paused ? "true" : "false");
	PX4_INFO("Serial Port ReadFD: %d", fdRead);
	PX4_INFO("Serial Port WriteFD: %d", fdWrite);
	PX4_INFO("Total bytes read: %zu", totalRead);
	PX4_INFO("Total bytes written: %zu", totalWritten);
	PX4_INFO("Connection: %s (%d sec ago)", check_connection() ? "OK" : "FAILED", secAgo);

	return PX4_OK;
}

int SandYachtNetwork::custom_command(int argc, char *argv[])
{
	SandYachtNetwork *instance = _object.load(); // custom_command is static
	if (!instance) {
		PX4_ERR("Module not running???");
		return print_usage("Module not running");
	}

	if (argc < 1) {
		return print_usage("missing command");
	}

	if (strcmp(argv[0], "pause") == 0) {
		instance->paused = !instance->paused;
		PX4_INFO("Paused: %s", instance->paused ? "true" : "false");
		return PX4_OK;
	}

	if (strcmp(argv[0], "reset") == 0) {
		PX4_INFO("Resetting numbers...");
		instance->seq = 0;
		instance->totalRead = 0;
		instance->totalWritten = 0;
		return PX4_OK;
	}


	return print_usage("unknown command");
}

int SandYachtNetwork::task_spawn(int argc, char *argv[])
{
	PX4_INFO("Starting SandYachtNetwork task");
	SandYachtNetwork *instance = new SandYachtNetwork();

	if (!instance) {
		PX4_ERR("Alloc fail SandYachtNetwork");
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

// Helper functin to stop the module and cleanup resources.
void SandYachtNetwork::exit(){
	update_and_publish_network_status(false);
	if(!free_resources()){
		PX4_ERR("Failed to free resources, what happened?");
	}

	ScheduleClear();
	exit_and_cleanup();
}

/**
 * Looks like this function is called in a main process context, we need to initialize all OS resources here.
 * Other functions are called in other contexts, so we get errors 9 or errors 13 from the OS if we try to use them being in the wrong context.
 * PX4_INFO logs from this context are printed in dmesg
 */
void SandYachtNetwork::Run(){
	bool connection;

	if (should_exit()) {
		exit();
		return;
	}

	if(!initialized){
		if(!post_init()){
			exit();
			return;
		}
		initialized = true;
		update_and_publish_network_status(false);
	}


	poll_sensors();
	poll_status();
	poll_autopilot_debug_info();

	if(publish_config) {
		PX4_INFO("Got Config: Land config: sail_speed_tolerance=%.2f, sail_speed_constant=%.2f, flap_step=%.2f, default_waypoint_tolerance=%u, curve_waypoint_tolerance=%u",
					land_config.sail_speed_tolerance,
					land_config.sail_speed_constant,
					(double) land_config.flap_step,
					(unsigned int) land_config.default_waypoint_tolerance,
					(unsigned int) land_config.curve_waypoint_tolerance);

		_land_config_pub.publish(land_config);
		publish_config = false;
	}

	if(publish_course){
		PX4_INFO("Got course: %d points", course.length);
		for(int i = 0; i < course.length; i++){
			PX4_INFO("Course point %d: x=%d, y=%d, type=%d", i, course.x[i], course.y[i], course.type[i]);
		}

		_course_pub.publish(course);
		publish_course = false;
	}

	if(publish_command){
		PX4_INFO("Got command: %d", command.command);
		_command_pub.publish(command);
		publish_command = false;
	}


	connection = check_connection();
	update_and_publish_network_status(connection);

	if(connection && !paused){
		send_sensors();
	}

	last_update = hrt_absolute_time();
}

int SandYachtNetwork::print_usage(const char *reason)
{
	if (reason) {
		PX4_WARN("%s\n", reason);
	}

	PRINT_MODULE_DESCRIPTION(
		R"DESCR_STR(
### Description
Module for managing network communication with the Sand Yacht system via XBee 3 radio module.


### Implementation
Section describing the high-level implementation of this module.

### Examples
CLI usage example:
$ sand_yacht_network start
$ sand_yacht_network pause
$ sand_yacht_network stop

)DESCR_STR");

	PRINT_MODULE_USAGE_NAME("module", "network");
	PRINT_MODULE_USAGE_COMMAND("start");
	// PRINT_MODULE_USAGE_PARAM_FLAG('f', "Optional example flag", true);
	// PRINT_MODULE_USAGE_PARAM_INT('p', 0, 0, 1000, "Optional example parameter", true);
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
	PRINT_MODULE_USAGE_COMMAND("pause");


	return PX4_OK;
}

// =============================================================================
// Network implementation
// =============================================================================

// Helper function to read a specific number of bytes from the serial port and serialize them into the ByteBuffer.
void SandYachtNetwork::read_bytes(size_t size) {
	ssize_t readen = readPort(fdRead, buffer, size);
	if (readen == -1) {
		PX4_ERR("Failed to read from serial port: %d (%s)", readen, strerror(errno));
		return;
	}

	if (readen == 0) {
		PX4_ERR("No data read from serial port???");
		return;
	}


	if (!bb.reallocate_from(buffer, size)) {
		PX4_ERR("Failed to reallocate ByteBuffer from read buffer");
	}

	totalRead += readen;
}

// Function that creates a sensors packet and sends it.
void SandYachtNetwork::send_sensors(){
	ByteBuffer* packet = packet_create_sensors(&sensors, &status, &autopilot_debug, ++seq);

	if(packet == NULL){
		PX4_ERR("Packet creation failed??? %d (errno %d?)", seq, errno);
		return;
	}

	if(seq >= 255){
  		PX4_WARN("Sequence number exceeded 255, resetting to 0");
  		seq = 0;
	}

	send_packet(packet);
	packet_destroy(packet);

}

// Function that writes data to the serial port to the XBee.
void SandYachtNetwork::send_packet(ByteBuffer* data){
	if (data == NULL || data->getSize() == 0) {
		PX4_ERR("Invalid packet data");
		return;
	}

	#ifdef ENABLE_SEND
	ssize_t bytes_written = writePort(fdWrite, data->buffer, data->getSize());
	if (bytes_written < 0) {
		PX4_ERR("Failed to write to serial port: %s", strerror(errno));
		return;
	}
	totalWritten += bytes_written;
	#endif

}


/**
 * Function runs in a separate thread and reads packets from the serial port.
 * Will be cancelled when the module is stopped.
 */

void SandYachtNetwork::read_packets(){
	PX4_INFO("Starting read_packet thread...");
	uint8_t _type;// , _seq;

	// course packet info
	uint8_t course_length = 0;

	// SPEED HERE IS CRITICAL. No lot IO here. Otherwise we lose data.
	while (true){
		if (paused) {
			usleep(20000);
			continue;
		}

		#ifdef FIND_SYNC_PATTERN
		if (!find_sync_pattern(fdRead)) {
            		PX4_ERR("Failed to find magic header, exiting read loop.");
            		break;
        	}
		#endif

		read_bytes(2); // read 2 bytes: type and seq
		_type = bb.get_i8();
		// _seq = bb.get_i8();

		switch (_type){
			case PacketType::INTERNAL_ACK:
				last_cl_ack_time = hrt_absolute_time();
				break;

			case PacketType::COMMAND:
				read_bytes(1);

				command.command = bb.get_i8();

				command.timestamp = hrt_absolute_time();
				publish_command = true;

				break;

			case PacketType::COURSE:
				read_bytes(1);

				course_length = bb.get_i8();

				read_bytes(course_length * COURSE_PACKET_POINT_SIZE);
				for(int i = 0; i < course_length; i++){
					course.x[i] = bb.get_i16();
					course.y[i] = bb.get_i16();
					course.type[i] = bb.get_i8();
				}
				course.length = course_length;

				course.timestamp = hrt_absolute_time();
				publish_course = true;

				break;
			case PacketType::CONFIG:
				read_bytes(8 + 8 + 4 + 8 + 1 + 1);

				land_config.sail_speed_tolerance = bb.get_f64();
				land_config.sail_speed_constant = bb.get_f64();
				land_config.flap_step = bb.get_f32();
				land_config.steering_constant_p = bb.get_f64();
				land_config.curve_waypoint_tolerance = (uint8_t) bb.get_i8();
				land_config.default_waypoint_tolerance = (uint8_t) bb.get_i8();

				land_config.timestamp = hrt_absolute_time();
				publish_config = true;

				break;

			default:
				#ifdef	 MISSMATCH_DEBUG
					// PX4_INFO("Unknown type: %u %u", _type, _seq);
				#endif
				break;
		}


	}
}

void SandYachtNetwork::update_and_publish_network_status(bool _connected){
	if(connected == _connected){
		return;
	}

	connected = _connected;
	network_status.connected = _connected;
 	_network_status_pub.publish(network_status);
 	PX4_INFO("Network status updated: connected=%s", _connected ? "true" : "false");
}

/**
 * Function to check if the connection is still alive.
 * It checks if the last client ACK was received within the MAX_DELAY time.
 */
bool SandYachtNetwork::check_connection(){
	if (last_cl_ack_time == 0){ // 0 - initial value, no ACK received yet
		return false;
	}

	return (hrt_absolute_time() - last_cl_ack_time) < (MAX_DELAY * 1000000); // convert to us
}

/**
 * Checks for UORB messages and updates the internal state.
 */
void SandYachtNetwork::poll_sensors(){
	if (!_sensors_sub.updated()) {
		return;
	}

	if (!_sensors_sub.copy(&sensors)) {
		PX4_ERR("Failed to copy sensors data from a sub");
	}
}
/**
 * Polls the autopilot status and updates the internal state.
 */
void SandYachtNetwork::poll_status(){
	if (!_status_sub.updated()) {
		return;
	}

	if (!_status_sub.copy(&status)) {
		PX4_ERR("Failed to copy status data from a sub");
	}
}

/**
 * Polls the autopilot debug info and updates the internal state.
 */
void SandYachtNetwork::poll_autopilot_debug_info(){
	if (!_autopilot_debug_sub.updated()) {
		return;
	}

	if (!_autopilot_debug_sub.copy(&autopilot_debug)) {
		PX4_ERR("Failed to copy autopilot debug info data from a sub");
		return;
	}

}
