#!/bin/bash

cd /media/Apps/amiberry
cp -f conf/default/* conf/
chmod +x ./amiberry
./amiberry  -config=./conf/AB-A500.uae 

