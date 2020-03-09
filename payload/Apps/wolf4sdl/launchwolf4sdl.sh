#!/bin/sh
cd /media/retroarch/apps/wolf4sdl
chmod +x "wolf3d"
export HOME=/media/retroarch/apps/wolf4sdl
./wolf3d --resf 1280 720 --joystick 0 > /media/retroarch/logs/wolf3d.log 2>&1
