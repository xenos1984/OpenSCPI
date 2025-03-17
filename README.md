# OpenSCPI project

The aim of this project is to create an open firmware for various microcontroller boards which implements the [SCPI protocol](http://en.wikipedia.org/wiki/Standard_Commands_for_Programmable_Instruments).

## Command categories

The following types of commands are supposed to be supported:

- analog output via DAC
- analog input via ADC
- digital output via GPIO
- digital input via GPIO
- serial communication via UART
- I²C communication via I²C
- SPI communication via SPI
- USB communication via USB
- Ethernet communication via TCP/IP
- WiFi communication via TCP/IP
- Bluetooth communication via UART
- GPIO control via PWM

## Supported Boards

Currently, the project is in an early phase and targetting only a test platform:

- Wavgat UNO [LGT8F328P](https://github.com/dbuezas/lgt8fx).
