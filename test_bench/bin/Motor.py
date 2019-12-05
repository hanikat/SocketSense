#Class for handling all interaction with a motor
import Settings
import LinearActuator

class Motor:

	motor = 0
	position = Settings.START_POS

	#Intialize the motor using the one specified in Settings.py file
	def __init__(self):
		if(Settings.MOTOR == "LINEAR"):
			if(Settings.DEBUG):
				print("Setting up LinearActuator as motor")
			self.motor = LinearActuator.LinearActuator()
		else:
			print("ERROR(Motor.Motor:1): Could not find specified motor type!")
		if(Settings.DEBUG):
			print("Motor class intialized!")

	def cleanup():
		self.motor.cleanup()

	#Method used to move the motor a set distance (mm) and direction
	#When direction = 0 the motor will extend, when direction = 1 the motor will retract
	def run_motor(self, distance, dir):
		if(not isinstance(float(distance),float) or not isinstance(dir, int)):
			print("ERROR(Motor.run_motor:1): Wrong type of argument supplied!")
			Settings.quit_prog()
		else:
			if(Settings.MOTOR == "LINEAR"):
				self.motor.run_motor(distance, dir)
			else:
				print("ERROR(Motor.run_motor:2): Could not find specified motor type!")

	#Reset motor position to Settings.START_POS value
	def reset_pos(self):
		self.motor.reset_pos()
		self.motor.run_motor(self.position, 0)
		if(Settings.DEBUG):
			print("Motor position was reset to: " + str(self.position))
