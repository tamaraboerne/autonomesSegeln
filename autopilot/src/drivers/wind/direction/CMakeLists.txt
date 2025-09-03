px4_add_module(
	MODULE drivers__wind__direction
	MAIN wind_direction
	COMPILE_FLAGS
	SRCS
		wind_direction.cpp
		wind_direction.h
	DEPENDS
		px4_work_queue
	)
