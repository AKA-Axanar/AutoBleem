#! /bin/sh
# Retroboot App Launcher

if [ -z "${RB_LIBRARY_PATH}" ]; then
	source /media/retroarch/retroboot/bin/loadconfig.sh
fi
if [ ! -d "${RB_LIBRARY_PATH}" ]; then
	sh /media/retroarch/retroboot/bin/init_libs.sh
fi
sh "/media/retroarch/apps/eduke32/launcheduke32.sh"
