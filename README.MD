# SocketSense Project
This repository is used to store all code and scripts used within the SocketSense project.

The repository should be setup in a folder at the location "/home/pi/Scripts/" on the raspberry pi.


# Code Introduction
Following scripts can be found in the "test_bench/bin" sub-folder. These scripts are used to control and run the test benchs mechanical parts.

## Main.py
This is the main script used to run the test bench. The script takes no input but the settings and parameters used when running the test bench can instead be configured in the Settings.py file. The Main.py file intializes the controlling classes for the mechanical test becnh. This script then tries to reach the targeted force values stored in a separate file within the test_bench/data folder. 

## Settings.py
This file contains settings and parameters used to run the test bench and its mechanical components.

## Motor.py
This class acts as a wrapper for controlling the motor(s) of the test bench. Currently only linear actuator is supported to be used within the test bench.

## LinearActuator.py
This class is used to control a linear actuator used within the test bench. The linear actuator can be extended and retracted using the methods found within this class.

## LoadCell.py
This class is used to request and read data from load cells used in the test bench. 
 
## hx711.py
This class was taken from the GitHub repository of gandalf15: https://github.com/gandalf15/HX711
The code supplied in this repository have been slighlty modified to clear out some bugs and to better fit our needs.

## Forces.py
This class is used to load values from a file with a given format. Currently only CSV files are supported. The file path can be configured in the Settings.py file. The next target force can be loaded from this class once the file have been succesfully loaded.

## LoadCellTest.py
A script used to create an instance of the load cell and record meassurements from it. The statistics of the meassurements will be printed at the end of the script.

## MoveMotor.py
A script which takes two arguments: --dir (int) and --dist (float). The direction can be either 1 (retract) or 0 (extend) and the distance is a float value greater than 0. The script will move the motor a set number of mm in the given direction.

## GPIO-cleanup.py
This script clears all the GPIO pins and makes sure that they are on the low state.

# Script for sending mail with IPs
To set up the script which sends a mail at each boot of the raspbery Pi containing the IP-addresses of the unit the following line needs to be added into the file opened when running the command "sudo crontab -e":

@reboot /usr/bin/python /home/pi/Scripts/newmailing.py >/home/pi/logs/cronlog 2>&1

Also, ssmtp and python have to be installed on the unit. The configuration of SMTP have to be set with the parameters found in ssmtp.conf. Just copy the ssmtp.conf to the "/etc/ssmtp/" directory. 
