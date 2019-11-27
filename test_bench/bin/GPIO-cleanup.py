import RPi.GPIO as GPIO

print("Cleaning up GPIO pins...")
GPIO.setmode(GPIO.BCM)
for x in range(40):
        #print("Clearing GPIO-pin nr: " + str(x))
        GPIO.setup(x, GPIO.OUT)
        GPIO.output(x, GPIO.LOW)
GPIO.cleanup()
print("Clean up done! Terminating...")
