#pragma once

#define BB_PX4_DETACH 0 // testing flag to compile without PX4
#define FALLBACK_VALUE 0

#if BB_PX4_DETACH == 0
#include <px4_log.h>
#else
#include <cstdint> // uint8_t
#include <cstdlib> // free
#include <cstring> // memcpy
#define PX4_ERR(...) printf(__VA_ARGS__)
#define PX4_WARN(...) printf(__VA_ARGS__)
#define PX4_INFO(...) printf(__VA_ARGS__)
#endif

class ByteBuffer
{
public:
	size_t capacity;
	size_t write_pos;    // write position
	size_t read_pos;     // read position
	size_t data_length;
	uint8_t *buffer;

	ByteBuffer() : capacity(0), write_pos(0), read_pos(0), data_length(0), buffer(nullptr) {}

	ByteBuffer(size_t buffer_capacity) : capacity(buffer_capacity), write_pos(0), read_pos(0), data_length(0), buffer(nullptr){
		buffer = new uint8_t[buffer_capacity];
		if (buffer == nullptr)
		{
			PX4_ERR("ByteBuffer: malloc fail, cannot allocate %zu bytes\n", buffer_capacity);
		}
	}

	~ByteBuffer(){
		delete[] buffer;
	}

	bool reallocate_from(uint8_t* buff, size_t buff_size){
		if (buff_size > capacity) {
			if (buffer != nullptr) {
				delete[] buffer;
			}
			capacity = buff_size;
			buffer = new uint8_t[capacity];
			if (buffer == nullptr) {
				PX4_ERR("ByteBuffer: malloc fail, cannot allocate %zu bytes\n", capacity);
				return false;
			}

			for (size_t i = 0; i < capacity; i++) {
				buffer[i] = 0;
			}

		}
		memcpy(buffer, buff, buff_size);
		write_pos = buff_size;
		read_pos = 0;
		data_length = buff_size;
		return true;
	}

	void *get_write_ptr(size_t bytes){
		if (!check_write_bounds(bytes, __LINE__)){
			return nullptr;
		}
		uint8_t *ptr = buffer + write_pos;
		write_pos += bytes;
		data_length = write_pos;
		return ptr;
	}

	void put(const void *src, size_t bytes){
		if (!check_write_bounds(bytes, __LINE__)){
			return;
		}
		memcpy(buffer + write_pos, src, bytes);
		write_pos += bytes;
		data_length = write_pos;
	}

	void *get_read_ptr(size_t bytes){
		if (!check_read_bounds(bytes, __LINE__)){
			return nullptr;
		}
		uint8_t *ptr = buffer + read_pos;
		read_pos += bytes;
		return ptr;
	}

	void skip(size_t bytes){
		if (!check_read_bounds(bytes, __LINE__)){
			return;
		}
		read_pos += bytes;
	}

	int8_t get_i8(){
		int8_t *ptr = (int8_t *)(get_read_ptr(sizeof(int8_t)));
		if (ptr == nullptr) {
			return ERROR_FALLBACK;
		}
		return *ptr;
	}

	void put_i8(int8_t value){
		put(&value, sizeof(int8_t));
	}

	int16_t get_i16(){
		int16_t *ptr = (int16_t *)(get_read_ptr(sizeof(int16_t)));
		if (ptr == nullptr) {
			return ERROR_FALLBACK;
		}
		return *ptr;
	}

	void put_i16(int16_t value){
		put(&value, sizeof(int16_t));
	}

	int32_t get_i32(){
		int32_t *ptr = (int32_t *)(get_read_ptr(sizeof(int32_t)));
		if (ptr == nullptr) {
			return ERROR_FALLBACK;
		}
		return *ptr;
	}

	void put_i32(int32_t value){
		put(&value, sizeof(int32_t));
	}

	int64_t get_i64(){
		int64_t *ptr = (int64_t *)(get_read_ptr(sizeof(int64_t)));
		if (ptr == nullptr) {
			return ERROR_FALLBACK;
		}
		return *ptr;
	}

	void put_i64(int64_t value){
		put(&value, sizeof(int64_t));
	}

	float get_f32(){
		float *ptr = (float *)(get_read_ptr(sizeof(float)));
		if (ptr == nullptr) {
			return ERROR_FALLBACK;
		}
		return *ptr;
	}

	void put_f32(float value){
		put(&value, sizeof(float));
	}

	double get_f64(){
		double *ptr = (double *)(get_read_ptr(sizeof(double)));
		if (ptr == nullptr) {
			return ERROR_FALLBACK;
		}
		return *ptr;
	}

	void put_f64(double value){
		put(&value, sizeof(double));
	}

	void reset(){
		write_pos = 0;
		read_pos = 0;
		data_length = 0;
	}

	void reset_read(){
		read_pos = 0;
	}

	void* getBuffer(){
		return buffer;
	}

	size_t getSize(){
		return data_length;
	}

	size_t getWritePos(){
		return write_pos;
	}

	size_t getReadPos(){
		return read_pos;
	}

private:
	static const uint8_t ERROR_FALLBACK = 0;

	bool check_write_bounds(size_t bytes, int line){
		if (write_pos + bytes > capacity){
			PX4_ERR("ByteBuffer: Write out of bounds. Write_pos: %zu, Capacity: %zu, Line: %d\n", write_pos, capacity, line);
			return false;
		}
		return true;
	}

	bool check_read_bounds(size_t bytes, int line){
		if (read_pos + bytes > data_length){
			PX4_ERR("ByteBuffer: Read out of bounds. Read_pos: %zu, Data_length: %zu, Line: %d\n", read_pos, data_length, line);
			return false;
		}
		return true;
	}
};
