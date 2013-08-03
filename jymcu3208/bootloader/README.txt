#check connection via programmer with:
sudo avrdude -c usbasp -p m8 -B1 -v -v

#backup firmware with:
sudo avrdude -c usbasp -p m8 -B1 -v -v -U flash:r:original_jy_mcu_3208_firmware.hex:r

#write fuses with:
avrdude -Chardware/tools/avrdude.conf -v -v -v -v -patmega8 -cusbasp -Pusb -e -Ulock:w:0x3F:m -Uhfuse:w:0xc8:m -Ulfuse:w:0xe4:m
avrdude -Chardware/tools/avrdude.conf -v -v -v -v -patmega8 -cusbasp -Pusb -Uflash:w:hardware/arduino/bootloaders/atmega8/ATmegaBOOT.hex:i -Ulock:w:0x0F:m 

#If you are having problems uploading sketches try the following command on the serial port to disable auto hangup
stty -F /dev/ttyUSB0 -hupcl
