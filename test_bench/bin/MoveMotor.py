#Used to move the motor a set distance and in a given direction without exceeding the maximum force
import argparse
import Motor
import Settings
import LoadCell
import signal

parser = argparse.ArgumentParser(description='Move motor a set distance and direction.')
parser.add_argument('--dist', action="store", dest="distance", type=float, help='Distance to move the motor')
parser.add_argument('--dir', action="store", dest="direction", type=int, help='Direction to move the motor, 1 is retract, 0 is extend')
args = parser.parse_args()
print("Distance: " + str(args.distance) + ", Direction: " + str(args.direction))



#Define controller for ctrl-c input
def ctrl_c_handler(sig, frame):
        print("Aborting program execution! Cleaning up GPIO pins...")
        print("Clean up done! Terminating...")
        Settings.quit_prog()

signal.signal(signal.SIGINT, ctrl_c_handler)


#Argument sanity check
if(isinstance(args.distance, float) and isinstance(args.direction, int)):
	if(args.distance > 0 and args.distance <= Settings.MAX_POS and args.direction <=1 and args.direction >= 0):
		#Initialize motor and load cell
		mot = Motor.Motor()
		lc = LoadCell.LoadCell()
		while(args.distance > 0):
			#Extract the current force applied from the load cell
			curForce = lc.getMeasurement()
			if(isinstance(curForce, float)):
		                #Check if the force exceeds maximum allowed value
				if(curForce < Settings.MAX_FORCE):
					mot.run_motor(float(1), args.direction)
					args.distance -= 1
				else:
					mot.run_motor(10, 1)
					print("ERROR: Maximum force value was exceeded!")
					mot.cleanup()
					Settings.quit_prog()
	else:
		print("ERROR: Arguments not within valid range!")
else:
	print("ERROR: Invalid argument type(s)!")
	Settings.quit_prog()
