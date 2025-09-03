typedef void (*rx_callback_t)(const uint8_t* data, size_t size);

ssize_t initPort(bool write);
bool closePort(int fd);

ssize_t readPort(int fd, uint8_t* buffer, size_t size);
ssize_t writePort(int fd, const uint8_t* buffer, size_t size);
