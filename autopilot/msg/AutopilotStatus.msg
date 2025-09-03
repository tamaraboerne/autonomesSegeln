uint64 timestamp

uint8 waypoint 	# 0b0 bis 0b___11111
		# Index des wegpunktes der Gerade angefahren wird

uint8 state 	# 0b_00_____ = Idle
		# 0b_01_____ = Ready
		# 0b_10_____ = Driving



uint8 error	# 0b0_______ = Kein Fehler
		# 0b1_______ = Fehler


