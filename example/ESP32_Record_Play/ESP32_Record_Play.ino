
#include <WiFi.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include "wm8960.h"
#include "makerfabs_pin.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "time.h"

#define V1  39
#define V2  36
#define mute 35

//Voice
const int record_time = 1; // second
String filename = "/textsound.wav";

char i2s_buffer[64000];

int log_index = 0;

String file_list[20];
int file_num = 0;
int file_index = 0;


//LCD

Adafruit_SSD1306 display(MP_ESP32_SSD1306_WIDTH, MP_ESP32_SSD1306_HEIGHT, &Wire, MP_ESP32_SSD1306_RST);


//WIFI
const char *ssid = "Makerfabs";
const char *password = "20160704";

//NTP time
//const char* ntpServer = "pool.ntp.org";
const char *ntpServer = "120.25.108.11";
const long gmtOffset_sec = 8 * 60 * 60; //China+8
const int daylightOffset_sec = 0;

struct tm timeinfo;

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

    pinMode ( V1 , INPUT);
    pinMode ( V2 , INPUT);
    pinMode ( mute, INPUT);
    
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
    
    //remove_soundwav_file(SD, "/");

        //connect to WiFi
    Serial.printf("Connecting to %s ", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" CONNECTED");

    //time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    Serial.println(F("Alread get npt time."));


    
    //Changefilename(); 

    //WM8960_Volume(0.8,0.8);
    WM8960_Volume(1.0,1.0);
    //WM8960_Volume(0.0,0.0);

    //recode
    //SD.remove(filename);

    //WM8960_Record(filename, i2s_buffer, record_time);

    //WM8960_Play(filename, i2s_buffer);

    //WM8960_Alarm();
}



int m0 = 1;
int t0;

void loop()
{
    if (!getLocalTime(&timeinfo))
        Serial.println("Failed to obtain time");


  
    int enter = digitalRead(mute) ;
    int play1 = digitalRead (V1);
    int play2 = digitalRead (V2);
    switch (m0)
    {
      case 1: displaymenu(1); break;
      case 2: displaymenu(2); break;
    }
    if ( play1 == 0 && m0 != 0)
    {
      m0--;
      if (m0>2) m0=1;
      delay(300);
    }

    if ( play2 == 0 && m0 !=0 )
    {
      m0++;
      if (m0<1) m0=2;
      delay(300);
    }
    
    if( enter ==0 )
    {
      t0 = m0;
/**************** play ***********************/      
      if( t0 == 2)
      { 
        m0 = 0;
        playmenu();       
         delay(500);
         m0 = 1;

       }
/************ record **********************/
       if( t0 == 1)
       {

            Changefilename();
            displayrecording(1);
            WM8960_Record(filename, i2s_buffer, record_time);
            //delay(500);
          //Changefilename(); 
         displayrecording(2);
       }
    }
    
    
   
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

void displayrecording(int t)
{
    display.clearDisplay();            
    display.setTextColor(SSD1306_WHITE); // Draw white text    
    display.setCursor(0, 40); // Start at top-left corner
    display.setTextSize(1);
    if ( t==1)
    {
      //display.println(F("Recording..."));
    }
    else
      display.println(F("Recording completed"));
    display.display();
    delay(100);
}

void displaying(int t)
{
    
    display.clearDisplay();         
    display.setTextColor(SSD1306_WHITE); // Draw white text    
    display.setCursor(0, 40); // Start at top-left corner
    display.setTextSize(1);
    if ( t==1)
    {
      display.setCursor(0, 0); // Start at top-left corner
      display.setTextSize(1);
      display.setTextColor(SSD1306_BLACK,SSD1306_WHITE);
      display.println(file_list[file_num-1]);
      display.setCursor(0, 20);
      display.setTextColor(SSD1306_WHITE); 
      display.println(file_list[file_num-2]);
      display.setCursor(0, 40);
      display.println("EXIT");
    }
    else if ( t==2 )
    {
      display.setCursor(0, 0); // Start at top-left corner
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.println(file_list[file_num-1]);
      display.setCursor(0, 20);
      display.setTextColor(SSD1306_BLACK,SSD1306_WHITE); 
      display.println(file_list[file_num-2]);
      display.setCursor(0, 40);
      display.setTextColor(SSD1306_WHITE); 
      display.println("EXIT");

    }
    else 
    {
      display.setCursor(0, 0); // Start at top-left corner
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.println(file_list[file_num-1]);
      display.setCursor(0, 20);
      display.setTextColor(SSD1306_WHITE); 
      display.println(file_list[file_num-2]);
      display.setCursor(0, 40);
      display.setTextColor(SSD1306_BLACK,SSD1306_WHITE); 
      display.println("EXIT");
    }
    display.display();  
}

void displaymenu(int t)
{
    display.clearDisplay();
    int hour = timeinfo.tm_hour;
    int min = timeinfo.tm_min;
    
    String displaytime = String(hour) + ":" + String(min);
    display.setTextSize(2);              // Normal 1:1 pixel scale
    if ( t==1)
    {
      display.setTextColor(SSD1306_BLACK,SSD1306_WHITE); // Draw white text    
      display.setCursor(0, 0); // Start at top-left corner
      display.println(F("RECORD"));
      display.setTextColor(SSD1306_WHITE); // Draw white text    
      display.setCursor(0, 20);
      display.println(F("PLAY"));
      display.setCursor(64, 44);
      display.println(displaytime);
      
      
    }
    else
    {
      display.setTextColor(SSD1306_WHITE); // Draw white text    
      display.setCursor(0, 0); // Start at top-left corner
      display.println(F("RECORD"));
      display.setTextColor(SSD1306_BLACK,SSD1306_WHITE); // Draw white text    
      display.setCursor(0, 20);
      display.println(F("PLAY"));
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(64, 44);
      display.println(displaytime);
    }
    display.display();
    //delay(2000);
}

void Changefilename()
{
    int hour = timeinfo.tm_hour;
    int min = timeinfo.tm_min;
    int day = timeinfo.tm_mday;
    int mon = timeinfo.tm_mon + 1;
    filename = "/" + String(mon) + String(day) + String(hour) + String(min) + "v.wav";

    
}

void WM8960_Record(String filename, char *buff, int record_time)
{
    int headerSize = 44;
    byte header[headerSize];
    int waveDataSize = record_time * 16000 * 16 * 2 / 8;
    int waveDataSize1  = waveDataSize;
    int recode_time = millis();
    int part_time = recode_time;

    File file = SD.open(filename, FILE_WRITE);
    if (!file)
        return;

    Serial.println("Begin to record:");
    //CreateWavHeader(header, waveDataSize);
    //file.write(header, headerSize);
    
    int enter1 = digitalRead(mute) ;
    int t = 0;
    while (enter1 == 0){
      display.clearDisplay(); 
      display.setTextSize(2); 
      display.setCursor(40, 20);
      display.setTextColor(SSD1306_WHITE);
      display.print(t+1);
      display.println(F("s"));
      display.setCursor(0, 40);
      display.setTextSize(1);
      display.println(F("Recording..."));
      display.display(); 
    for (int j = 0; j < waveDataSize / sizeof(buff); ++j)
    {
        I2S_Read(buff, sizeof(buff));
        file.write((const byte *)buff, sizeof(buff));
        if ((millis() - part_time) > 1000)
        {
            Serial.print(".");
            part_time = millis();
        }
    }
      t++;
      enter1 = digitalRead(mute) ;
    }
    waveDataSize1 = waveDataSize1 * t;
    file.seek(0);
    CreateWavHeader(header, waveDataSize1);
    file.write(header, headerSize);
    
    Serial.println("");
    Serial.println("Finish");
    Serial.println(millis() - recode_time);
    file.close();
}

void playmenu()
{ 
    file_num = get_music_list(SD, "/", 0, file_list);
    int m2 = 1;
    while(m2 != 0)
    {

        switch(m2)
          {
              case 1: displaying(1); break;
              case 2: displaying(2); break;
              case 3: displaying(3) ; break;
             }

        int enter = digitalRead(mute) ;
        int play1 = digitalRead (V1);
        int play2 = digitalRead (V2);
        if ( play1 == 0 && t0 == 2)
        {
            m2--;
            if (m2>3) m2=1;
            delay(300);
        }

        if ( play2 == 0 && t0 ==2 )
        {
            m2++;
            if (m2<1) m2=3;
            delay(300);
        }
    
        if( enter ==0 && t0 == 2)
        {   
          switch(m2)
          {
              case 1: {playsounding(); WM8960_Play(file_list[file_num-1], i2s_buffer); } break;
              case 2: {playsounding(); WM8960_Play(file_list[file_num-2], i2s_buffer); } break;
              case 3: m2=0 ; break;
             }
          delay(300);
         }     
    }
   Serial.printf("menu m2 exit"); 
}

int get_music_list(fs::FS &fs, const char *dirname, uint8_t levels, String wavlist[30])
{
    Serial.printf("Listing directory: %s\n", dirname);
    int i = 0;

    File root = fs.open(dirname);
    if (!root)
    {
        Serial.println("Failed to open directory");
        return i;
    }
    if (!root.isDirectory())
    {
        Serial.println("Not a directory");
        return i;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
        }
        else
        {
            String temp = file.name();
            if (temp.endsWith(".wav"))
            {
                wavlist[i] = temp;
                i++;
            }
            
        }
        file = root.openNextFile();
    }
    file.close();
    return i;
}
/*
void remove_soundwav_file(fs::FS &fs, const char *dirname)
{
    Serial.printf("remove:");


    File root = fs.open(dirname);
    if (!root)
    {
        Serial.println("Failed to open directory");
        
    }
    if (!root.isDirectory())
    {
        Serial.println("Not a directory");
        
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
        }
        else
        {
            String temp = file.name();
            if (temp.endsWith("sound.wav"))
            {
                fs.remove(temp);
               
            }
            
        }
        file = root.openNextFile();
    }
    file.close();
 
}
*/
void playsounding()
{
   
    display.clearDisplay();
    display.setTextSize(2);              // Normal 1:1 pixel scale

      display.setTextColor(SSD1306_BLACK,SSD1306_WHITE); // Draw white text    
      display.setCursor(0, 20); // Start at top-left corner
      display.println(F("Playing..."));
    display.display();

}
