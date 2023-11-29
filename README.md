# PICO Hull Pixelbot 
![Robot picture](images/Pixelbot-small.jpg)
This is the Raspberry Pi PICO version of the Hull Pixelbot. It can also be built for an ESP32 (although at present this is a work in progress). The control software is called HullOS. You can find the Arduino Uno version [here](https://github.com/HullPixelbot/HullOS). You can find the Hull Pixelbot on GitHub [here](http://hullpixelbot.com/). You can get the documentation for HullOS [here](https://github.com/HullPixelbot/Documentation), and 3D printable and laser cut chassis designs [here](https://github.com/HullPixelbot/Hardware). 
## Building the software
The software is built using Platform.IO. You can edit the platformio.ini file to select the build version and deployment. At present the PICO code is deployed to the PICO debugger. 
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

Have fun

Rob Miles