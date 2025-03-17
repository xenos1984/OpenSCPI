#!/usr/bin/python3
# -*- coding: utf-8 -*-

# Simple DAC vs ADC test:
# Connect DAC0 output (pin 4) to ADC0 input (pin A0) and run the script.
# Expected output: measured voltage and deviation from voltage set in DAC.

import serial
import time

ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=10)

time.sleep(5)

ser.write(b"*IDN?\n")
ser.flush()
print(ser.readline())

refs = {b'VCC' : 3, b'INT1' : 1, b'INT2' : 2, b'INT4' : 4}

for ref in refs:
	ser.write(b":CONF:ANAL:VREF %s\n" % ref)
	ser.flush()
	time.sleep(1)
	for v in [0.1 * refs[ref], 0.25 * refs[ref], 0.5 * refs[ref], 0.8 * refs[ref]]:
		ser.write(b":SOUR:VOLT %f\n" % v)
		ser.flush()
		for i in range(0, 10):
			time.sleep(1)
			ser.write(b":MEAS:VOLT? 1\n")
			ser.flush()
			val = float(ser.readline())
			dev = (val - v) * 100 / v
			print("Ref = " + str(ref) + ": " + str(val) + "V; deviation " + str(dev) + "%")

#	for i in range(0, 256):
#		ser.write(b":SOUR:VOLT %d\n" % i)
#		ser.flush()
##		time.sleep(0.1)
#		ser.write(b":MEAS:VOLT? 1\n")
#		ser.flush()
#		val = float(ser.readline())
#		print(str(val) + " / " + str(i + 1) + " = " + str(val / (i + 1)))
