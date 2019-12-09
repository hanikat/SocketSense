#Class for handling and running a linear actuator
import Settings
import RPi.GPIO as GPIO
import time

class LinearActuator:

	#Variable used for PWM control
	pwm = 0

	#Linear actuator specific variables, maybe move these to Settings.py?
	direction = 0
	frequency = 20000
	duty_cycle = 100
	#Number of seconds to run per mm of movement
	s_mm = 1/5
	retract_correction = 0.9271

	def __init__(self):
		if(Settings.DEBUG):
			print("LinearActuator class intialized!")

		#Intialize GPIO for LinearActuator control
		GPIO.setmode(GPIO.BCM)
		GPIO.setwarnings(False)
		GPIO.setup(Settings.MOTOR_IN_1, GPIO.OUT)
		GPIO.setup(Settings.MOTOR_IN_2, GPIO.OUT)
		GPIO.setup(Settings.MOTOR_EN_PIN, GPIO.OUT)
		#Disable motor until we need to use it
		GPIO.output(Settings.MOTOR_EN_PIN, False)
		if(Settings.PWM_MOTOR):
			self.pwm = GPIO.PWM(Settings.MOTOR_IN_1, self.frequency)

	def cleanup():
		Settings.quit_prog()

	#Method used to move motor "distance" number of mm
	#dir is used to controll the direction of the linear actuator, 0 = extend, 1 = retract
	def run_motor(self, distance, dir):
		#Set the direction to run the linear actuator in
		if(dir > 1 or dir < 0):
			print("ERROR(LinearActuator.run_motor:1): Invalid direction supplied as argument: " + str(dir))
			Settings.quit_prog()

		#Run motor for set amount of time equal to "distance" mm
		if(distance > Settings.MAX_POS or distance <= 0):
			print("ERROR(LinearActuator.run_motor:2): Invalid movement distance supplied as argument: " + str(distance))
		if(dir == 1):
			distance = (distance / self.retract_correction);

		if(Settings.PWM_MOTOR):
			GPIO.output(Settings.MOTOR_EN_PIN, dir)
			self.pwm.start(self.duty_cycle)
			time.sleep(distance * self.s_mm)
			self.pwm.stop()
		else:
			#Enable motor and start moving it
			GPIO.output(Settings.MOTOR_EN_PIN, True)
			GPIO.output(Settings.MOTOR_IN_1, dir)
			GPIO.output(Settings.MOTOR_IN_2, not dir)

			#Wait until motor has moved the given distance
			time.sleep(distance * self.s_mm)

			#Disable motor and stop moving it
			GPIO.output(Settings.MOTOR_IN_1, 0)
			GPIO.output(Settings.MOTOR_IN_2, 0)
			GPIO.output(Settings.MOTOR_EN_PIN, False)

		#if(Settings.DEBUG):
		#	print("Motor was moved " + str(distance) + " mm")

	#Reset motor position to 0
	def reset_pos(self):
		self.run_motor(Settings.MAX_POS, 1)
