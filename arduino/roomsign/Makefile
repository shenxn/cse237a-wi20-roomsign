all: compile upload

compile:
	arduino-cli compile --fqbn arduino:avr:uno .

upload:
	arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:avr:uno .

clean:
	rm -f ..arduino.avr.uno.elf ..arduino.avr.uno.hex ..arduino.avr.uno.with_bootloader.hex
