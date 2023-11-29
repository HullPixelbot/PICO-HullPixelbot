# PICO Hull Pixelbot 
![Robot picture](images/Pixelbot-small.jpg)
This is the Raspberry Pi PICO version of the Hull Pixelbot. It can also be built for an ESP32 (although at present this is a work in progress). The control software is called HullOS. You can find the Arduino Uno version [here](https://github.com/HullPixelbot/HullOS). You can find the Hull Pixelbot on GitHub [here](http://hullpixelbot.com/). You can get the documentation for HullOS [here](https://github.com/HullPixelbot/Documentation), and 3D printable and laser cut chassis designs [here](https://github.com/HullPixelbot/Hardware). 
## Putting HullOS on a PICO
You can load HullOS onto a Raspberry Pi PICO by:

1. Download the image file from [here](HULLOSR1-3.uf2) and store it on your computer. 
1. Hold down the BOOTSEL button on your PICO. 
1. Plug the PICO into your PC.
1. Release the BOOTSEL button.
1. An external drive will open on your computer. 
1. Drag the image file onto the drive.
1. The PICO will reboot running HullOS

## Connecting to the HullOS
You can communicate with HullOS via any terminal program. The baud rate is set to 115200. HullOS will not echo commands as you type them. Press the enter key at the end of each command. The command "*IV" can be used to test a connection, it returns the version number of the software:
```
*IV
HullOS R1.3
```
## Battery power
The robot can be powered using 3 or 4 AA sized cells. If you use 4 cells you should make sure that the battery voltage does not exceed 6.5 volts (some batteries give out more than 1.5 volts when brand new) otherwise the PICO may not run. If you use rechargeable batteries you can use 4 with no problem as each battery only produces 1.2 volts. 
## Building the hardware
![Fritzing circuit](images/Hullpixelbot%20Breadboard.png)
You can build the circuit on a breadboard as shown above. The signal pins for the devices connected to the robot are as follows. If you want to change these they are set in the respective ".h" files for the different devices.

| Signal | PICO GPIO | ESP32 GPIO |
|--------|-----------|------------|
| Distance trigger | 17  | xx |
| Distance Echo | 16 | xx |
| Left motor 1| 15| 13 |
| Left motor 2| 14| 12 |
| Left motor 3| 13| 14 |
| Left motor 4| 12| 27 |
| Right motor 1| 11| 26 |
| Right motor 2| 10| 25 |
| Right motor 3| 9| 33 |
| Right motor 4| 8| 32 |
| Neopixel | 6 | 21 |
## Editing Python-ish programs
You can program the robot using Python-ish. You can find a description of the language [here].
## Building the HullOS software
The HullOS software is built using Platform.IO. You can edit the platformio.ini file in the repository to select the build version and deployment. At present the PICO code is deployed to the PICO debugger. You only need to do this if you want to change the assignment of the GPIO pins or add/change features. 

Have fun

Rob Miles