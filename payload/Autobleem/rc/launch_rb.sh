#!/bin/bash

# Retroboot launcher for AutoBleem

if [ -f /tmp/.rbpatching ]; then
	exit 0
fi



#kill sony stuffs and set powermanagement parameters
killall -s KILL showLogo sonyapp ui_menu auto_dimmer pcsx dimmer 
echo 2 > /data/power/disable

echo Image "$1"
echo Core "$2"

sh /media/retroarch/retroboot/bin/launch_rfa_rom.sh "$1" "$2"
rm /tmp/.abload
usleep 250000 
