#!/bin/sh

export XDG_CONFIG_HOME=/media/retroarch/apps

cd /media/retroarch/apps/opentyrian
chmod +x ./opentyrian
./opentyrian > /media/retroarch/logs/opentyrian.log 2>&1