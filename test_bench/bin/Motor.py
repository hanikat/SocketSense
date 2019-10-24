#Class for handling all interaction with the motor
import Settings

class Motor:
	def __init__(self):
		if(Settings.DEBUG):
			print("Motor class intialized!")
	
	def step_motor(self, steps):
		if(not isinstance(steps,int)):
			print("ERROR(Motor.step_motor:1): Wrong type of argument supplied!")
			quit()
		else:
			if(Settings.DEBUG):
				print("Stepped motor " + str(steps) + " steps")

