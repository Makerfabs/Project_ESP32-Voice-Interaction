/*
Auther  : Vincent
Date    : 2020/10/16
Function    : Record 5s, and play back.
*/

#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include "wm8960.h"
#include "makerfabs_pin.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Voice
const int record_time = 5; // second
const char filename[] = "/sound.wav";
char i2s_buffer[64000];

//LCD

Adafruit_SSD1306 display(MP_ESP32_SSD1306_WIDTH, MP_ESP32_SSD1306_HEIGHT, &Wire, MP_ESP32_SSD1306_RST);

void setup()
{
    uint8_t res;

    //LCD
    Wire.begin(MP_ESP32_I2C_SDA, MP_ESP32_I2C_SCL);
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    { // Address 0x3C for 128x32
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }

    display.clearDisplay();
    logoshow();
    Serial.begin(115200);

    res = WM8960_Init(MP_VOICE_I2S_DIN, MP_VOICE_I2S_DOUT, MP_VOICE_I2S_LRC, MP_VOICE_I2S_BCLK);
    if (res != 0)
        Serial.println(res);

    //SD(SPI)
    pinMode(MP_VOICE_SD_CS, OUTPUT);
    digitalWrite(MP_VOICE_SD_CS, HIGH);
    SPI.begin(MP_VOICE_SPI_SCK, MP_VOICE_SPI_MISO, MP_VOICE_SPI_MOSI);
    SPI.setFrequency(1000000);
    if (!SD.begin(MP_VOICE_SD_CS, SPI))
    {
        Serial.println("Card Mount Failed");
        while (1)
            ;
    }

    //WM8960_Volume(0.8,0.8);
    //WM8960_Volume(1.0,1.0);
    WM8960_Volume(0.9,0.9);

    //recode
    //SD.remove(filename);

    //WM8960_Record(filename, i2s_buffer, record_time);

    //WM8960_Play(filename, i2s_buffer);

    //WM8960_Alarm();
}

void loop()
{
    WM8960_Play("/speak.wav", i2s_buffer);
}

void logoshow(void)
{
    display.clearDisplay();

    display.setTextSize(2);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);             // Start at top-left corner
    display.println(F("MakePython"));
    display.setCursor(0, 20); // Start at top-left corner
    display.println(F("Voice"));
    display.display();
    delay(2000);
}