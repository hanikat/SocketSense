
#Class for fetching and providing force values from a load cell
import Settings
from hx711 import HX711
import os
import signal

class LoadCell:

	#Force values which are currently only used for testing
	#The real force values should be extracted from the load cell when implemented
	forces = [100, 200, 300, 400, 500, 400, 300, 200]
	index = 0
	hx711 = 0
	lastMeas = 0.0
	errorCount = 0
	pid = os.getpid()

	def __init__(self):
		#Setup and initialize load cell amplifier hx711
		self.hx711 = HX711(dout_pin=Settings.LC_DOUT_PIN, pd_sck_pin=Settings.LC_PD_SCK_PIN, select_channel=Settings.LC_CHANNEL, gain_channel_A=Settings.LC_GAIN)
		#Reset and zero(tare) the loadcell output
		self.hx711.reset()
		self.hx711.zero(readings=99)

	#Method to get the next force value within the list
	def getNextForce(self):
		length = len(self.forces)
		if(length == 0):
			print("ERROR(Forces.getNextForce:1): No force values have been loaded!")
			Settings.quit_prog()
		else:
                        #Check if we reached the end of the lists containing the force values, restart if that is the case
                        if(self.index < length):
                                self.index += 1
                                if(Settings.DEBUG):
                                        print("Current force value: " + str(self.forces[self.index - 1]))
                                return int(self.forces[self.index - 1])
                        else:
                                self.index = 1
                                if(Settings.DEBUG):
                                        print("Current force value: " + str(self.forces[0]))
                                return int(self.forces[0])


	def getMeasurement(self):
		#Get current meassurement from load cell
		curMeas = (self.hx711.get_data_mean(readings=Settings.LC_READINGS_PER_MEASSUREMENT)/Settings.LC_N_RATIO)
		if(Settings.DEBUG):
			print("CurMeas: " + str(curMeas))

		#Check if the values of the current meassurement are valid and within range, stop the program if errorCount is exceeded
		if(float(curMeas) == float(0) and self.errorCount < Settings.ALLOWED_ERROR_COUNT):
			#Current meassurement is zero, increment error count
			self.errorCount	+= 1
			return float(self.lastMeas)
		elif((float(curMeas - self.lastMeas) > float(Settings.ALLOWED_FORCE_DIF) or float(curMeas - self.lastMeas) > float(0) - float(Settings.ALLOWED_FORCE_DIF)) and self.errorCount > Settings.ALLOWED_ERROR_COUNT):
			#Current meassurement is out of range, increment error count
			self.errorCount += 1
			return float(self.lastMeas)
		elif(self.errorCount >= Settings.ALLOWED_ERROR_COUNT):
			#Error count threshold have been reached, terminate program
			#os.kill(self.pid, signal.SIGINT)
			return float(-1)
		else:
			#Normal meassurement was recorded, reset errorCount
			self.errorCount = 0
			self.lastMeas = curMeas
			return float(curMeas)
