px4_add_module(
	MODULE drivers__wind__speed
	MAIN wind_speed
	COMPILE_FLAGS
	SRCS
		wind_speed.cpp
		wind_speed.h
	DEPENDS
		px4_work_queue
	)
