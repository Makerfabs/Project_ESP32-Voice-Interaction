# ESP32 Voice Interaction

```c++
/*
Version:		V1.1
Author:			Vincent
Create Date:	2020/10/16

Note:
		v1.1 Add Record & Play example.
		v1.2 Add Interphone example.
*/
```

![main](md_pic/main.JPG)

[toc]

# OVERVIEW

## Intruduce

[Makerfabs home page](https://www.makerfabs.com/)

[Makerfabs Wiki](https://makerfabs.com/wiki/index.php?title=Main_Page)

ESP32 Voice Interaction is an extended version with speakers and mics. The audio IC is WM8960. It has a button, SD card slot and a headphone jack.  It is convenient to produce MP3, interphone, recording pen and other items. 

![oversee](md_pic/oversee2.png)

## Equipment list

- [MakePython ESP32](https://www.makerfabs.com/wiki/index.php?title=MakePython_ESP32)
- [MakaPython Voice Interaction Hat]()
- Micro SD card (Part of the project)



# STEPS

## Prepare And Burn

**If you have any questions，such as how to install the development board, how to download the code, how to install the library. Please refer to :[Makerfabs_FAQ](https://github.com/Makerfabs/Makerfabs_FAQ)**

- Install board : Esp32 .
- Install library : Adafruit SSD1306 and dependent libraries.
- Upload test file "example/adc/adc.ino"
- After esp32 reboot, it will record 5 second. And then loop playback.



# Example

## ESP32_Record_Play

A recorder with a simple menu.

## ESP32_Interphone

UDP protocol LAN walkie-talkie.