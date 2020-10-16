#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include "wm8960.h"

#define SDA 4
#define SCL 5

#define BCK 26
#define WS 25
#define DOUT 27
#define DIN 33

//SD Card
#define SD_CS 22
#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_SCK 18

//Voice
const int record_time = 5; // second
const char filename[] = "/sound.wav";
char i2s_buffer[64000];

void setup()
{
    uint8_t res;

    //I2C
    Wire.begin(SDA, SCL);
    Serial.begin(115200);

    res = WM8960_Init();
    if (res != 0)
        Serial.println(res);

    //SD(SPI)
    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    SPI.setFrequency(1000000);
    if (!SD.begin(SD_CS, SPI))
    {
        Serial.println("Card Mount Failed");
        while (1)
            ;
    }

    //recode
    SD.remove(filename);

    WM8960_Record(filename, i2s_buffer, record_time);

    //WM8960_Play(filename, i2s_buffer);

    //WM8960_Alarm();
}

void loop()
{
    WM8960_Play(filename, i2s_buffer);
}