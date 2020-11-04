# ESP32 Voice Interaction

```c++
/*
Version:		V1.0
Author:			Vincent
Create Date:	2020/10/16

Note:

*/
```

![main](md_pic/main.JPG)

[toc]

# OVERVIEW

## Intruduce

[Makerfabs home page](https://www.makerfabs.com/)

[Makerfabs Wiki](https://makerfabs.com/wiki/index.php?title=Main_Page)

MakePython **************************

![oversee](md_pic/oversee2.png)

## Equipment list

- [MakePython ESP32](https://www.makerfabs.com/wiki/index.php?title=MakePython_ESP32)
- [MakaPython Voice Interaction Hat]()
- Micro SD card



# STEPS

## Prepare And Burn

**If you have any questions，such as how to install the development board, how to download the code, how to install the library. Please refer to :[Makerfabs_FAQ](https://github.com/Makerfabs/Makerfabs_FAQ)**

- Voice Interaction Hat for MakePython can’t work alone, it need work with Makepython ESP32.
- Install board : esp32 .

- Install library : Adafruit SSD1306 and dependent libraries.

- Upload test file "example/adc/adc.ino"

- After esp32 reboot, it will record 5 second. And then loop playback.

## Record

* Upload file "example/record/record.ino".
* Press the button to start recording, the recording time is 5S, and the recording will be played one minute after the end of the recording.

## ESP32 Recording equipment

* Upload file "example/ESP32_Record_Play/ESP32_Record_Play.ino"
* Turn up and down to select the option, press the button to confirm the selection.
* After entering the recording function, you need to keep pressing the button until the recording end.
* The playback function can display two recording files, one is the current recording completed, and the other is the last recorded file.


