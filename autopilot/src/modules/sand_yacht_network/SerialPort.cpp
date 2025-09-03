#include "SerialPort.hpp"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <px4_platform_common/log.h>

#define SERIAL_BAUD_RATE B38400
#define SERIAL_PORT "/dev/ttyS1"

/**
 * Opens the serial port for reading or writing and configures it with the required settings.
*/
ssize_t initPort(bool write)
{
	PX4_INFO("Initializing serial port %s with baud rate 38400 with mode %s", SERIAL_PORT, write ? "write" : "read");

	int mode = write ? O_WRONLY : O_RDONLY;
	int fd = open(SERIAL_PORT, mode | O_NOCTTY);


	if (fd < 0) {
		PX4_ERR("Failed to open serial port %s: %s", SERIAL_PORT, strerror(errno));
		return PX4_ERROR;
	}

	struct termios tty;
	// get current terminal attributes
	if (tcgetattr(fd, &tty) != 0) {
		PX4_ERR("Failed to get terminal attributes for %s: %s", SERIAL_PORT, strerror(errno));
		close(fd);
		return  PX4_ERROR;
	}

	cfsetospeed(&tty, SERIAL_BAUD_RATE);
	cfsetispeed(&tty, SERIAL_BAUD_RATE);

	// 8 data bits
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;

	// 1 stop bit
	tty.c_cflag &= ~CSTOPB;

	// No parity
	tty.c_cflag &= ~PARENB;

	// Raw mode
	tty.c_lflag = 0;
	tty.c_oflag = 0;
	tty.c_iflag = 0;

	// Enable receiver, set local mode
	tty.c_cflag |= (CLOCAL | CREAD);

	// Disable hardware flow control
	tty.c_cflag &= ~CRTSCTS;

	tty.c_cc[VMIN] = 1;   // Wait for at least 1 character
	tty.c_cc[VTIME] = 0;  // No timeout

	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		PX4_ERR("Failed to apply attributes for %s: %s(%d)", SERIAL_PORT, strerror(errno), errno);
		close(fd);
		return PX4_ERROR;
	}

	return fd;
}


bool closePort(int fd)
{
	if (fd < 0) { // debug workaround
		return true;
	}

	if (close(fd) < 0) {
		PX4_ERR("Fail to close serial port %d: %s???????", fd, strerror(errno));
		return false;
	}

	return true;
}

ssize_t readPort(int fd, uint8_t *buffer, size_t size){

	size_t total_read = 0;
	ssize_t read_now = 0;
	while (total_read < size) {
		read_now = read(fd, buffer + total_read, size - total_read);
		if (read_now < 0){
			PX4_ERR("readPort() error(%d) %s", errno, strerror(errno));
		}
		total_read += read_now;
	}
	return total_read;

	// ssize_t bytesRead = read(fd, buffer, size);

	// if (bytesRead < 0) {
	// 	PX4_ERR("readPort() error(%d) %s", errno, strerror(errno));
	// 	return 0;
	// }

	// if(bytesRead == 0) {
	// 	PX4_ERR("readPort() returned 0 bytes, no data available");
	// 	return 0;
	// }

	// return (size_t)bytesRead;
}



ssize_t writePort(int fd, const uint8_t *buffer, size_t size)
{
	ssize_t bytesWritten = write(fd, buffer, size);

	if (bytesWritten < 0) {
		PX4_ERR("writePort() error(%d) %s", errno, strerror(errno));
		return 0;
	}

	if (tcdrain(fd) < 0) {
		PX4_ERR("Failed to drain serial port %d: %s", fd, strerror(errno));
		return 0;
	}

	usleep(1000 + (size * 100)); // wait for the data to be sent, todo could be reduced even more? test it
	return bytesWritten;
}
