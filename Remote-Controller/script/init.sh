#!/bin/bash

#lxterminal -e "timeout 3 python3 /home/pi/Desktop/arduino_rpi_seri_haberlesme/okuma_raspberrypi.py; bash"
#lxterminal -e "/home/pi/Desktop/button/a.out; bash"

#!/bin/bash

lxterminal -e "timeout 3 python3 /home/pi/Desktop/arduino_rpi_seri_haberlesme/okuma_raspberrypi.py; bash" &
sleep 5
lxterminal -e "/home/pi/Desktop/button/a.out; bash" &