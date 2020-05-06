================================================================================
          ABFlashKit - Safe kernel flasher for Playstation Classic
================================================================================
                           by Screemer and madmonkey

The tool provides automatic kernel and payload install for AutoBleem Tool

    !!! THIS TOOL WILL MODIFY YOUR INTERNAL STORAGE OF THE CONSOLE !!! 

The tool flashes the kernel in multiple steps. All steps are automatic

1. The backup of console firmware is created in USB/LBOOT.EPB - do not delete 
   this file ever. This step is pretty slow, and may take couple minutes to 
   complete
2. Signs the backup with correct signature needed for internal PSC recovery 
3. Switches console into possible recovery mode
4. Flashes the kernel image with a new AutoBleem kernel
5. Updates the payload needed for extended functionality
6. Switches back the console to safe normal mode
7. Triggers the reboot

In case one of flashing step fails console will boot into Recovery mode. In 
recovery mode console will not boot but will blink power led green and then red.
If this happens put back USB with the backed up LBOOT.EPB and repower the console
by replacing power cable.
After successfull restore the system will boot as normal.

In the case the backup is corrupted the restore may fail and after couple seconds
The console wil blink with RED even if the USB stick with backup is placed in the 
USB port. If this happens:

1. Copy backup from any other console - they are universal or
2. (advanced users) Use Fastboot tool to flash old kernel and MISC partition
   This step is not part of this read me