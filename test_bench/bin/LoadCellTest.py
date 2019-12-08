import Settings
import LoadCell
import time

lc = LoadCell.LoadCell()

start_time = time.time()
meassurements = 0
sum = 0
n = 1000

input("Load cell has been zeroed, place weight on load cell and press enter to coninue... ")
meas = lc.getMeasurement()
low = meas
high = meas
while(meassurements < n):
	meas = lc.getMeasurement()
	if(meas < low):
		low = meas
	elif(meas > high):
		high = meas
	sum += meas

	print("Current Load: " + str(meas))
	meassurements += 1

print("Elapsed time: " + str(time.time() - start_time))
print("Highest recorded load: " + str(high))
print("Lowest recorded load: " + str(low))
print("Average load: " + str(sum/n))
