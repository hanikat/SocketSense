#Class for fetching and providing force values from a load cell
import Settings

class LoadCell:

	#Force values which are currently only used for testing
	#The real force values should be extracted from the load cell when implemented
	forces = [100, 200, 300, 400, 500, 450, 400, 300, 200]
	index = 0

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
                                self.index = 0
                                if(Settings.DEBUG):
                                        print("Current force value: " + str(self.forces[self.index]))
                                return int(self.forces[self.index])
