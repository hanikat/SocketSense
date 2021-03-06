import Settings
import Motor
import Forces
import LoadCell
import time
import signal
import RPi.GPIO as GPIO
from queue import Queue
from threading import Thread

#Define method used for LoadCell reading thread
def loadCell_loop(out_queue, term_sig):

	#Initalize LoadCell class
	lc = LoadCell.LoadCell()
	f = open("log.txt", "a+")
	while(True):
		if(term_sig.empty()):
			out_queue.put(lc.getMeasurement())
			f.write("Gathered data from load cell...")
		else:
			f.write("Exiting thread 2...")
			f.close
			break

#Class intilization
forces = Forces.Forces()
forces.load_forces(Settings.FORCE_VALUE_FILE)
mot = Motor.Motor()
queue = Queue()
sig = Queue()
_sentinel = object()
t1 = Thread(target = loadCell_loop, args=(queue, sig, ))
t1.start()


#Define controller for ctrl-c input
def ctrl_c_handler(sig, frame):
        print("Aborting program execution! Cleaning up GPIO pins...")
        print("Clean up done! Terminating...")
        quit_prog()

def quit_prog():
	sig.put("WHATEVER")
	Settings.quit_prog()
	time.sleep(2)
	quit()

signal.signal(signal.SIGINT, ctrl_c_handler)

mot.reset_pos()
curMovement = 0
startTime = time.time()

#Get the first target force and set the direction
force = forces.getNextForce()
direction = 0
lastForce = 0.0

print("Waiting for load cell initlization...")
while(queue.empty()):
	time.sleep(1/100)
print("Load cell initialized!")


while(True):

	if(not t1.isAlive()):
		print("ERROR(Main:4): The load cell thread has died, exiting program!")
		ctrl_c_handler(sig, 0)

	#Check if queue is empty, if that is the case use the previously fetched force value
	if(not queue.empty()):
		#Extract the current force applied from the load cell
		curForce = queue.get()
		if(Settings.DEBUG):
			print("Current force value is: " + str(curForce))
	else:
		curForce = lastForce

	#Set lastForce to be equivalent to the current force, to be used in the next loop
	lastForce = curForce

	if(isinstance(curForce, float) and isinstance(force, int)):

		#Check if the force exceeds maximum allowed value
		if(curForce < Settings.MAX_FORCE):

			#Check if the force is bigger or samller than the current value and if we are continously moving in the right direction
			#direction = 0 extends the LA, direction = 1 retracts LA
			if(force > curForce and direction == 0):
				#Targeted force is bigger and we are extending LA
				if(curMovement + Settings.STEP_SIZE >= Settings.MAX_MOV):
					print("ERROR(Main:3): The maximum movement length of the motor was reached!") 
					quit_prog()
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
				#if(time.time() - startTime >= Settings.MOTOR_DUTY_CYCLE_TIME):
				#	startTime = time.time()
				#	time.sleep(Settings.MOTOR_DUTY_CYCLE_TIME/Settings.MOTOR_DUTY_CYCLE)

				#Set new direciton of motor
				if(force > curForce):
					direction = 0
				else:
					direction = 1
				if(Settings.DEBUG):
					print("The targeted force was reached! Next force target is: " + str(force) + ", moving in direction:" + str(direction))
		else:
			print("ERROR(Main:1): The maximum force value was exceeded!")
			quit_prog()
	else:
		print("ERROR(Main:2): The force returned was not an integer value! " + str(force) + ", " + str(curForce))
		if(not isinstance(curForce, float)):
			print("Jupp")
		quit_prog()
