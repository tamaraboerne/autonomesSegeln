#pragma once

#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#include <px4_platform_common/module.h>
#include <uORB/Subscription.hpp>
#include <uORB/Publication.hpp>

#include <uORB/topics/wind_direction.h>
#include <uORB/topics/sand_yacht_wind_dir.h>

#define SLIDING_WINDOW 3 // number of samples to average for wind direction

using namespace time_literals;

class SandYachtWindDir final :
	public ModuleBase<SandYachtWindDir>,
	public px4::ScheduledWorkItem

{
public:
	SandYachtWindDir(void);
	~SandYachtWindDir(void);
	SandYachtWindDir(const SandYachtWindDir &) = delete;
	SandYachtWindDir &operator=(const SandYachtWindDir &other) = delete;

	// ModuleBas
	static int task_spawn(int argc, char *argv[]);
	static int custom_command(int argc, char *argv[]);
	static int print_usage(const char *reason = nullptr);
	void Run() override;
	int print_status() override;

private:

	uORB::Subscription _wind_dir{ORB_ID(wind_direction)};
	uORB::Publication<sand_yacht_wind_dir_s> _wind_speed_pub{ORB_ID(sand_yacht_wind_dir)};

	wind_direction_s _wind_dir_data = {0};
	sand_yacht_wind_dir_s sand_yacht_wind_dir = {0};

	float sliding_window[SLIDING_WINDOW] = {0.0f};
	uint8_t sliding_index = 0;
	bool sliding_window_filled = false;

	int8_t impl_mode = -1; // -1 = not set, 0 = no filter impl, 1 = filter impl



	bool init();

	void add_sliding_window(float value) {
		if (!sliding_window_filled) {
			sliding_window[sliding_index] = value;
			sliding_index++;

			if (sliding_index == SLIDING_WINDOW) {
				PX4_INFO("Sliding window filled with %d samples", SLIDING_WINDOW);
				sliding_window_filled = true;
			}

			return;
		}

		for (int i = 0; i < SLIDING_WINDOW - 1; i++) {
			sliding_window[i] = sliding_window[i + 1];
		}
		sliding_window[SLIDING_WINDOW - 1] = value;
	}

	float get_sliding_median() {
		float array[SLIDING_WINDOW] = { 0.0f };
		float middle_value, temp = 0.0f;

		// fill local copy
		for (int i = 0; i < SLIDING_WINDOW; ++i) {
			array[i] = sliding_window[i];
		}

		// sort the array with bubble sort
		for (int i = 0; i < SLIDING_WINDOW - 1; ++i) {
			for (int j = i + 1; j < SLIDING_WINDOW; ++j) {
				if (array[i] > array[j]) {
					temp = array[i];
					array[i] = array[j];
					array[j] = temp;
				}
			}
		}

		if(SLIDING_WINDOW % 2 == 0) {
			middle_value = ( array[(SLIDING_WINDOW / 2) - 1] + array[SLIDING_WINDOW / 2] ) / 2.0f;
		} else {
			middle_value = array[SLIDING_WINDOW / 2];
		}

		return middle_value;
	}


};
