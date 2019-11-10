#Root directory
ROOT_DIR = "/home/pi/Scripts/test_bench"

#Defines if debug is enabled
DEBUG = True

#Force value file
FORCE_VALUE_FILE = ROOT_DIR + "/data/force_values.csv"

#Definition of what motor is used, possible values:
#LINEAR
MOTOR = "LINEAR"

#Define maximum allowed force
MAX_FORCE = 1000

#Define start position of motor (mm)
START_POS = 100
#Define maximum position of motor (mm)
MAX_POS = 315
#Define maximum movement range in one force value (mm)
MAX_MOV = 20
#Define the step size in mm
STEP_SIZE = 0.5
