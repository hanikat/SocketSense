# ----- COMMON -----
#Root directory
ROOT_DIR = "/home/pi/Desktop/SocketSense/test_bench"

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
START_POS = 50
#Define maximum position of motor (mm)
MAX_POS = 150
#Define maximum movement range in one force value (mm)
MAX_MOV = 20
#Define the step size in mm
STEP_SIZE = 1
#Define for how long the motor should run in one go (seconds)
MOTOR_DUTY_CYCLE_TIME = 20
#Define the duty cycle of the motor (percentage (float), 0-1)
MOTOR_DUTY_CYCLE = 0.25

# ----- PIN PLACEMENT -----
#Using GPIO.BCM outlay
#LOAD CELL
LC_DOUT_PIN = 5
LC_PD_SCK_PIN = 6

#MOTOR
MOTOR_IN_1 = 23
MOTOR_IN_2 = 18
MOTOR_EN_PIN = 2


# ----- LOAD CELL -----
LC_CHANNEL = 'A'
LC_GAIN = 64
LC_N_RATIO = -735.23
LC_READINGS_PER_MEASSUREMENT = int(1)
