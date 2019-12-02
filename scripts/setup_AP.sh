#!/bin/bash
#Simon von Schmalensee 30/11-2019
#Script for setting up a raspberry pi4 running buster as an accespoint with DHCP-server
#run as sudo 
apt install dnsmasq hostapd
systemctl stop dnsmasq
systemctl stop hostapd

DHCPCD_CONFIG="interface wlan0\n static ip_address=192.168.4.1/24\n nohook wpa_supplicant"

echo -e $DHCPCD_CONFIG >> /etc/dhcpcd.conf

service dhcpcd restart

mv /etc/dnsmasq.conf /etc/dnsmasq.conf.orig

DNSMASQ_CONFIG="interface=wlan0\n dhcp-range=192.168.4.2,192.168.4.20,255.255.255.0,24h"

echo -e $DNSMASQ_CONFIG >> /etc/dnsmasq.conf

systemctl reload dnsmasq

HOSTAPD_CONFIG="interface=wlan0\n driver=nl80211\n ssid=SOCKET_SENSE\n hw_mode=g\n channel=7\n wmm_enabled=0\n macaddr_acl=0\n auth_algs=1\n ignore_broadcast_ssid=0\n wpa=2\n wpa_passphrase=socketsense\n wpa_key_mgmt=WPA-PSK\n wpa_pairwise=TKIP\n rsn_pairwise=CCMP\n"

echo -e $HOSTAPD_CONFIG >> /etc/hostapd/hostapd.conf






