# SocketSense Project
This repository is used to store all code and scripts used within the SocketSense project.

The repository should be setup in a folder at the location "/home/pi/Scripts/" on the raspberry pi.

# Script for sending mail with IPs
To set up the script which sends a mail at each boot of the raspbery Pi containing the IP-addresses of the unit the following line needs to be added into the file opened when runni$

@reboot /usr/bin/python /home/pi/Scripts/newmailing.py >/home/pi/logs/cronlog 2>&1


Also, ssmtp and python have to be installed on the unit. The configuration of SMTP have to be set with the parameters found in ssmtp.conf. Just copy the ssmtp.conf to the "/etc/ssmtp/" directory. 
