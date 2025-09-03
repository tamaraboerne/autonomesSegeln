px4_add_module(
	MODULE drivers__wing_direction
	MAIN wing_direction
	COMPILE_FLAGS
	SRCS
		wing_direction.cpp
		wing_direction.h
	DEPENDS
		px4_work_queue
	)
