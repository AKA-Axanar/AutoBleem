#!/bin/sh
cd /media/Apps/wolf4sdl
chmod +x "wolf3d"
export HOME=/media/Apps/wolf4sdl
./wolf3d --resf 1280 720 --joystick 0 > /media/retroarch/logs/wolf3d.log 2>&1
