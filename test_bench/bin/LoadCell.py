#Class for fetching and providing force values from a load cell
import Settings
from hx711 import HX711

class LoadCell:

	#Force values which are currently only used for testing
	#The real force values should be extracted from the load cell when implemented
	forces = [100, 200, 300, 400, 500, 400, 300, 200]
	index = 0
	hx711 = 0

	def __init__(self):
		self.hx711 = HX711(dout_pin=Settings.LC_DOUT_PIN, pd_sck_pin=Settings.LC_PD_SCK_PIN, channel=Settings.LC_CHANNEL, gain=Settings.LC_GAIN)
		#self.hx711.reset()

	#Method to get the next force value within the list
	def getNextForce(self):
		length = len(self.forces)
		if(length == 0):
			print("ERROR(Forces.getNextForce:1): No force values have been loaded!")
			quit()
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
		return self.hx711.get_raw_data_mean(num_measures=1)
