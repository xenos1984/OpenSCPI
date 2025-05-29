#!/usr/bin/python3
# -*- coding: utf-8 -*-

# Simple DAC vs ADC test:
# Connect DAC0 output (pin 4) to ADC0 input (pin A0) and run the script.
# Expected output: measured voltage and deviation from voltage set in DAC.

import serial
import time
import matplotlib.pyplot as plt

ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=10)

time.sleep(5)

ser.write(b"*IDN?\n")
ser.flush()
print(ser.readline())

refs = {b'VCC' : 3, b'INT1' : 1, b'INT2' : 2, b'INT4' : 4}

for ref in refs:
	xx = []
	yv = []
	yd = []
	ser.write(b":CONF:ANAL:VREF %s\n" % ref)
	ser.flush()
	time.sleep(1)

	for i in range(1, 11):
		v = i * 0.1 * refs[ref]
		vals = []
		devs = []
		ser.write(b":SOUR:VOLT %fV\n" % v)
		ser.flush()
		time.sleep(1)

		for i in range(0, 10):
			ser.write(b":MEAS:VOLT? 1\n")
			ser.flush()
			val = float(ser.readline())
			dev = (val - v) * 100 / v
			vals.append(val)
			devs.append(dev)
			time.sleep(0.1)

		val = sum(vals) / len(vals)
		dev = sum(devs) / len(devs)
		print("Ref = " + str(ref) + ": Uo = " + str(v)  + "V -> Ui = " + str(val) + "V; deviation = " + str(dev) + "%")

		xx.append(v)
		yv.append(val)
		yd.append(dev)

	fig, (av, ad) = plt.subplots(2, 1)
	av.plot(xx, yv)
	ad.plot(xx, yd)
	av.grid(True)
	ad.grid(True)
	plt.show()

#	for i in range(0, 256):
#		ser.write(b":SOUR:VOLT %d\n" % i)
#		ser.flush()
##		time.sleep(0.1)
#		ser.write(b":MEAS:VOLT? 1\n")
#		ser.flush()
#		val = float(ser.readline())
#		print(str(val) + " / " + str(i + 1) + " = " + str(val / (i + 1)))
