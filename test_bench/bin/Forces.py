#Class for fetching and providing force values
import Settings
import os.path
import csv

class Forces:

	force_values = []
	index = 0

	#Method used to load CSV-files into the force_values variable
        def load_csv(self, path_to_file):
                if(Settings.DEBUG):
                        print("load_csv invoked, reading CSV from file")
                file = open(path_to_file,'r')
		csvrd = csv.reader(file, delimiter = ',')
		for ln in csvrd:
			for item in ln: 
                		self.force_values.append(item)

		if(len(self.force_values) <= 0):
			print("ERROR(forces.load_csv:1): Error reading values from file, or file was empty!")
			quit()

        #Method used to load data from txt file into the force_values variable
        def load_txt(self, path_to_file):
                if(Settings.DEBUG):
                        print("load_txt invoked, reading input from text file")
	#Method to get the next force value within the list
	def getNextForce(self):
		length = len(self.force_values)
		if(length == 0):
			print("ERROR(Forces.getNextForce:1): No force values have been loaded!")
			quit()
		else:
			#Check if we reached the end of the lists containing the force values, restart if that is the case
			if(self.index < length):
				self.index += 1
				if(Settings.DEBUG):
    		                	print("Fetched the next force value: " + str(self.force_values[self.index]))
				return self.force_values[self.index - 1]
			else:
				self.index = 0
				if(Settings.DEBUG):
                                	print("Fetched the next force value: " + str(self.force_values[self.index]))
				return self.force_values[self.index]

	def __init__(self):
		if(Settings.DEBUG):
			print("Forces class intialized!")
		index = 0
	#Method used to load force values from a specified file
	def load_forces(self, path_to_file):
		if(not isinstance(path_to_file, str)):
			print("ERROR(Forces.load_forces:1): Wrong type of argument supplied!")
			quit()
		else:
			if(os.path.exists(path_to_file)):
				if(Settings.DEBUG):
					print("Found file!")
				else:
					print("ERROR(Forces.load_forces:2): Could not find the file path!")
					quit()

				#Determine the correct file loader to use
				if(os.path.splitext(path_to_file)[1] == ".csv"):
					self.load_csv(path_to_file)
				elif(os.path.splitext(path_to_file)[1] == ".txt"):
					self.load_txt(path_to_file)
				else:
					if(Settings.DEBUG):
						print("ERROR(Forces.load_forces:3): Invalid file format used for force value file!")

				if(Settings.DEBUG):
					print("Lodaded " + str(len(self.force_values)) + " force values from file: " + path_to_file)
