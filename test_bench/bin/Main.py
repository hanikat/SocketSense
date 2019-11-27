import Settings
import Motor
import Forces
import LoadCell
import time
import signal
import RPi.GPIO as GPIO

#Define controller for ctrl-c input
def ctrl_c_handler(sig, frame):
        print("Aborting program execution! Cleaning up GPIO pins...")
        GPIO.setmode(GPIO.BCM)
        for x in range(40):
                #print("Clearing GPIO-pin nr: " + str(x))
                GPIO.setup(x, GPIO.OUT)
                GPIO.output(x, GPIO.LOW)
        GPIO.cleanup()
        print("Clean up done! Terminating...")
        quit()

signal.signal(signal.SIGINT, ctrl_c_handler)

#Class intilization
forces = Forces.Forces()
forces.load_forces(Settings.FORCE_VALUE_FILE)
mot = Motor.Motor()
lc = LoadCell.LoadCell()

#Get the first target force and set the direction
force = forces.getNextForce()
direction = 0

mot.reset_pos()
curMovement = 0
startTime = time.time()

while(True):

	#Extract the current force applied from the load cell
	curForce = lc.getMeasurement()
	#print("Current force applied: " + str(curForce))
	if(isinstance(curForce, float) and isinstance(force, int)):

		#Check if the force exceeds maximum allowed value
		if(curForce < Settings.MAX_FORCE):

			#Check if the force is bigger or samller than the current value and if we are continously moving in the right direction
			#direction = 0 extends the LA, direction = 1 retracts LA
			if(force > curForce and direction == 0):
				#Targeted force is bigger and we are extending LA
				if(curMovement + Settings.STEP_SIZE >= Settings.MAX_MOV):
					print("ERROR(Main:3): The maximum movement length of the motor was reached!")
					quit()
				else:
					#Maximum movement range have not yet been reached
					curMovement += Settings.STEP_SIZE
					mot.run_motor(Settings.STEP_SIZE, direction)
			elif(force <= curForce and direction == 1):
				#Targeted force is smaller and we are retracting LA
                                #Maximum movement range have not yet been reached
				curMovement -= Settings.STEP_SIZE
				mot.run_motor(Settings.STEP_SIZE, direction)
			else:
				#The LA was moved to a position where the targeted force was met and surpased, get next force value
				force = forces.getNextForce()
				curMovement = 0
				#Allow motor to stop before changing drection
				time.sleep(1/8)
				
				#Sleep motor if duty cycle threshold have been reached
				if(time.time() - startTime >= Settings.MOTORDUTY_CYCLE_TIME):
					startTime = time.time()
					time.sleep(Settings.MOTOR_DUTY_CYCLE_TIME/Settings.MOTOR_DUTY_CYCLE)
					
				#Set new direciton of motor	
				if(force > curForce):
					direction = 0
				else:
					direction = 1
				if(Settings.DEBUG):
					print("The targeted force was reached! Next force target is: " + str(force) + ", moving in direction:" + str(direction))
		else:
			print("ERROR(Main:1): The maximum force value was exceeded!")
			quit()
	else:
		print("ERROR(Main:2): The force returned was not an integer value! " + str(force) + ", " + str(curForce))
		quit()
