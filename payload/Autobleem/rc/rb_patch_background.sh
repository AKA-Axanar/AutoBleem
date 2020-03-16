#!/bin/sh
# AutoBleem Background RB_PATCH installer

flashleds()
{
	while : ; do
		echo 1 > /sys/class/leds/green/brightness
		echo 1 > /sys/class/leds/red/brightness
		usleep 250000
		echo 1 > /sys/class/leds/green/brightness
		echo 0 > /sys/class/leds/red/brightness
		usleep 250000
	done
}

do_patch()
{
	# RetroBoot for AutoBleem mode
	export RB_MODE=1

	# Load Configuration
	source /media/retroarch/retroboot/bin/loadconfig.sh

	# Set up environment

	export XDG_CONFIG_HOME=/media
	export PATH=/media/retroarch/retroboot/bin:$PATH
	export LD_LIBRARY_PATH=/media/retroarch/retroboot/lib:$LD_LIBRARY_PATH

	# Run updates
	if [ ! $RB_PATCH -eq 1 ] || [ ! -d /media/RB_PATCH ]; then
		exit 0
	fi

	# Don't start RA while updating
	touch /tmp/.rbpatching

	export RB_PATCHLOG=/media/retroarch/logs/update.log
	rm $RB_PATCHLOG

	UPDATEERROR=0
	PATCHESFOUND=0
	PATCHESINSTALLED=0

	printf "\nLooking for patches..." >> $RB_PATCHLOG

	flashleds &
	LEDPID=$!

	for PATCHFILE in /media/RB_PATCH/PATCH_*.sh; do 

		if [ ! -f $PATCHFILE ]; then
			continue
		fi
		
		printf "\nINSTALL: Starting patch script: $PATCHFILE\n" >> $RB_PATCHLOG
		PATCHESFOUND=1

		sed -i 's/\r//g' "$PATCHFILE"
		sh "$PATCHFILE"
		PATCHERROR=$?
		
		if [ $PATCHERROR -ne 0 ]; then
			UPDATEERROR=1
			mv "$PATCHFILE" "$PATCHFILE.FAILED"
			printf "\nFAILURE: Patch failed with error $PATCHERROR.\n" >> $RB_PATCHLOG
		else
			PATCHESINSTALLED=1
			if [ $RB_PATCHCLEANUP -eq 1 ]; then
				rm "$PATCHFILE"
			fi
			printf "\nSUCCESS: Patch was installed successfully.\n" >> $RB_PATCHLOG
		fi
	done

	kill $LEDPID

	if [ $PATCHESFOUND -eq 0 ]; then
		printf "\nNo patches found." >> $RB_PATCHLOG
		export RB_PATCHED=0
		echo 0 > /sys/class/leds/red/brightness
		echo 1 > /sys/class/leds/green/brightness
	elif [ $UPDATEERROR -eq 0 -a $RB_PATCHCLEANUP -eq 1 ]; then
		printf "\nDeleting patch folder." >> $RB_PATCHLOG
		rm -rf /media/RB_PATCH
	fi

	if [ $UPDATEERROR -eq 0 ]; then
		printf "\nUpdate script completed successfully." >> $RB_PATCHLOG
		echo 0 > /sys/class/leds/red/brightness
		
		for i in 1 2 3 4 5 6 7 8 9
		do
			echo 1 > /sys/class/leds/green/brightness
			usleep 250000
			echo 0 > /sys/class/leds/green/brightness
			usleep 125000
		done
		
	else
		printf "\nUpdate script completed with errors." >> $RB_PATCHLOG

		echo 0 > /sys/class/leds/green/brightness

		for i in 1 2 3 4 5 6 7 8 9
		do
			echo 1 > /sys/class/leds/red/brightness
			usleep 250000
			echo 0 > /sys/class/leds/red/brightness
			usleep 125000
		done
	fi
			
	export RB_PATCHED=$PATCHESFOUND


	if [ $RB_PATCHED -gt 0 ]; then

		flashleds &
		LEDPID=$!
		
		printf "\nFinalizing update." >> $RB_PATCHLOG

		sync
		sleep 1
		kill $LEDPID
		
		echo 1 > /sys/class/leds/red/brightness
		echo 0 > /sys/class/leds/green/brightness
		
	fi

	echo 0 > /sys/class/leds/red/brightness
	echo 1 > /sys/class/leds/green/brightness

	rm /tmp/.rbpatching
	exit 0
}
do_patch &