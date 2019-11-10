#Class for handling and running a linear actuator
import Settings
import RPi.GPIO as GPIO
import time

class LinearActuator:

	#Pin mapping
	pin_dir = 23
	pin_pwm = 18
	#Variable used for PWM control
	pwm = 0

	#Linear actuator specific variables, maybe move these to Settings.py?
	direction = 0
	frequency = 20000
	duty_cycle = 100
	#Number of seconds to run per mm of movement
	s_mm = 1/34.46
	retract_correction = 0.9261

	def __init__(self):
		if(Settings.DEBUG):
			print("LinearActuator class intialized!")

		#Intialize GPIO for LinearActuator control
		GPIO.setmode(GPIO.BCM)
		GPIO.setwarnings(False)
		GPIO.setup(self.pin_dir, GPIO.OUT)
		GPIO.setup(self.pin_pwm, GPIO.OUT)
		self.pwm = GPIO.PWM(self.pin_pwm, self.frequency)

	#Method used to move motor "distance" number of mm
	#dir is used to controll the direction of the linear actuator, 0 = extend, 1 = retract
	def run_motor(self, distance, dir):
		#Set the direction to run the linear actuator in
		if(dir > 1 or dir < 0):
			print("ERROR(LinearActuator.run_motor:1): Invalid direction supplied as argument: " + str(dir))
			quit()
		GPIO.output(self.pin_dir, dir)

		#Run motor for set amount of time equal to "distance" mm
		if(distance > Settings.MAX_POS or distance <= 0):
			print("ERROR(LinearActuator.run_motor:2): Invalid movement distance supplied as argument: " + str(distance))
		if(dir == 1):
			distance = (distance / self.retract_correction);
		self.pwm.start(self.duty_cycle)
		time.sleep(distance * self.s_mm)
		self.pwm.stop()

		if(Settings.DEBUG):
			print("Motor was moved " + str(distance) + " mm")

	#Reset motor position to 0
	def reset_pos(self):
		self.run_motor(Settings.MAX_POS, 1)
