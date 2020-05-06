#!/bin/bash

mount -o remount,rw /data

echo Installing AutoBleem payload
cp -r /etc/wpa_supplicant.conf /tmp/wpa_supplicant.conf
cp -r /etc/autobleem/ssid.cfg /tmp/ssid.cfg

rm -rf /data/autobleem
mkdir -p /data/autobleem/rootfs
mkdir -p /data/autobleem/workdir
tar -xvzf /media/Apps/abflashkit/kernel/abrootfs.tgz -C /data/autobleem/rootfs


cp -r /tmp/wpa_supplicant.conf /etc/wpa_supplicant.conf
cp -r /tmp/ssid.cfg /etc/autobleem/ssid.cfg
echo Done 