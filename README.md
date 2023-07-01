# Datalogger

This is a picture of the device supporting low frequency sensor data-logging with STM32F103RGT6.
![datalogger](/document/imgs/device4.jpg) 
![datalogger](/document/imgs/device5.jpg) 


Core functions:
* periodically collect environment data such as temperature, humidity, light intensity and sound intensity (the sampling period can be configured in software user-interface)
* log data to an external flash (16MB, more than enough memory for 1 year usage)
* provide user-friendly interface to configure the device and retrive data (simulate a USB disk to easily store and get data as files in .csv format, using python script to write configuration info into the device)

Specs:
* 2 AA batteries with 6000h life expectancy. (suppose 50uA in standby mode and 40mA in active mode, the logging time is 2s, 1h sampling period, 1000mAh battery)
* can store up to 45000 records before running out of battery charge
* support measurement of sound(35~100dB), light(0~65535lux), temperature(0~85 degree in celsius), humidity(0~100%)

How to use:
1. Open the switch on the device and it will start periodically collecting data.
2. If you need to configure the time and sampling-period of the device, plug in the micro-usb cable, run config.py and input sampling period. Unplug the cable, and the device will read the configuration info and update its time according to the local time of your computer, the LED will double blink indicating config success, and set up alarm time before going to standby mode.
3. If you need to get the data, plug in the micro-usb cable. The device should be recognized by your computer as USB Mass Storage Device. The data is stored in csv format and you can easily copy it to your computer for further data processing.
4. If the LED is constantly blinking, that is suggesting low battery.

* USB disk interface
![USB disk](/document/imgs/ui1.jpg) 
* Configuration interface
![config UI](/document/imgs/ui2.jpg) 
* Data format
![data format](/document/imgs/ui3.jpg) 


## Hardware Design
#### Schematic
![Controller](/document/imgs/sch1.png) 
![Sensors](/document/imgs/sch2.png) 

#### PCB
![PCB](/document/imgs/pcb.jpg) 

#### Mechanical Design
* Front panel using laser cutting PMMA
![panel](/document/imgs/panel.jpg) 
* 3D printed body part and bottom cover
![body](/document/imgs/shell.jpg) 
![bottom](/document/imgs/bottom.jpg) 

## Firmware
The project is developed using CubeMX and KEIL, using STM32 HAL library.

The main code is in /code/run.h

All custom library is in /code/lib/

The project file is in /firmware/

## Background
For background on / motivation for this project, please see https://opendeved.net/programmes/ilce-in-tanzania/.

Reference: (same project with different design) https://github.com/bablokb/pcb-pico-datalogger#pico-datalogger-with-integrated-power-management