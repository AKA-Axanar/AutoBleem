#!/bin/sh

echo "Starting OpenBOR" >> /media/retroarch/logs/OpenBOR.log

rm /media/retroarch/logs/OpenBOR.log

echo "Unzipping Libraries" >> /media/retroarch/logs/OpenBOR.log

mkdir -p /tmp/lib
cp /media/retroarch/apps/openbor/libs.tar.gz /tmp/lib
cd /tmp/lib
tar xvzf libs.tar.gz
rm /tmp/lib/libs.tar.gz

export LD_LIBRARY_PATH=/tmp/lib

echo "Starting OpenBOR executable" >> /media/retroarch/logs/OpenBOR.log

cd "/media/Apps/openbor/OpenBOR"
chmod +x "./OpenBOR-psc"

./OpenBOR-psc > /media/retroarch/logs/OpenBOR.log 2>&1

echo "Cleaning up libs" >> /media/retroarch/logs/OpenBOR.log

rm -rf /tmp/lib
