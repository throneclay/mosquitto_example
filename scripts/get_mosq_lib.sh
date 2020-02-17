#!/bin/bash
wget https://mosquitto.org/files/source/mosquitto-1.6.8.tar.gz
tar -zxvf mosquitto-1.6.8.tar.gz
cd mosquitto-1.6.8
mkdir build
cd build
cmake ..
make DESTDIR=../../mosquitto install
