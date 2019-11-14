# ----- COMMON -----
#Root directory
ROOT_DIR = "/home/pi/Scripts/test_bench"

#Defines if debug is enabled
DEBUG = True


# ----- FORCES -----

#Force value file
FORCE_VALUE_FILE = ROOT_DIR + "/data/force_values.csv"

#Define maximum allowed force
MAX_FORCE = 1000


# ----- MOTOR -----

#Definition of what motor is used, possible values:
#LINEAR
MOTOR = 'LINEAR'

#Define start position of motor (mm)
START_POS = 100
#Define maximum position of motor (mm)
MAX_POS = 315
#Define maximum movement range in one force value (mm)
MAX_MOV = 20
#Define the step size in mm
STEP_SIZE = 0.5


# ----- PIN PLACEMENT -----
#Using GPIO.BCM outlay
#LOAD CELL
LC_DOUT_PIN = 5
LC_PD_SCK_PIN = 6

#MOTOR
MOTOR_DIR_PIN = 23
MOTOR_PWM_PIN = 18


# ----- LOAD CELL -----
LC_CHANNEL = 'A'
LC_GAIN = 64

