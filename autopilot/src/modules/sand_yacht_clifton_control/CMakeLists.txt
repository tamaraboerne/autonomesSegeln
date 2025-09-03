add_subdirectory(Autopilot)

px4_add_module(
	MODULE modules__sand_yacht_clifton_control
	MAIN sand_yacht_clifton_control
	SRCS
		SandYachtCliftonControl.cpp
		SandYachtCliftonControl.hpp
		DrivingSystem.cpp
		DrivingSystem.hpp
		Ackermann.cpp
		Ackermann.hpp
		Brake.cpp
		Brake.hpp
	DEPENDS
		Autopilot
		uORB
	)

