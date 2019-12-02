#!/bin/bash
### Simon von Schmalensee 30/11-2019
### This script installs influx_db on a system running rasbian buster and creates a database called SOCKET_SENSE. Run as sudo



curl -sL https://repos.influxdata.com/influxdb.key | sudo apt-key add -

echo "deb https://repos.influxdata.com/debian buster stable" | sudo tee /etc/apt/sources.list.d/influxdb.list

apt update
apt install -y influxdb
systemctl start influxdb

influx -execute 'CREATE DATABASE SOCKET_SENSE'
influx -database 'SOCKET_SENSE' -execute 'CREATE USER socketsense WITH PASSWORD 'socketsense' WITH ALL PRIVILEGES'
