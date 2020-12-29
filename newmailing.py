import smtplib
import netifaces as ni
import time
def is_interface_up(interface):
    addr = ni.ifaddresses(interface)
    return ni.AF_INET in addr


time.sleep(60)
server=smtplib.SMTP('smtp.gmail.com',587)
server.starttls()
server.login("", "")
ni.ifaddresses("wlan0")
msg="Subject:Raspberry Online!\n\nHello from Raspberry Pi! You find me at: wlan0: " + ni.ifaddresses("wlan0")[ni.AF_INET][0]['addr']
if(is_interface_up("eth0")):
	msg = msg + ", eth0: " + ni.ifaddresses("eth0")[ni.AF_INET][0]['addr']
server.sendmail("","",msg)
server.quit()
