/*
 *  This sketch sends random data over UDP on a ESP32 device
 *
 */
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include "wm8960.h"
#include "makerfabs_pin.h"
//#include <esp_heap_caps.h>
//#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BUFF_LENGTH 1200

uint8_t rec_buff[BUFF_LENGTH];

// WiFi network name and password:
const char *networkName = "Makerfabs";
const char *networkPswd = "20160704";

//IP address to send UDP data to:
// either use the ip address of the server or
// a network broadcast address
const char *udpAddress = "192.168.1.255"; //255, broadccast address
const int udpPort = 195;

//Are we currently connected?
boolean connected = false;

//The udp library class
WiFiUDP udp;

//last ip
int dev_num = -1;
int last_dev = -1;
int dev_mode = 0;

#define INIT_MODE 0
#define SAY_MODE 1
#define LISTEN_MODE 2

//LCD
Adafruit_SSD1306 display(MP_ESP32_SSD1306_WIDTH, MP_ESP32_SSD1306_HEIGHT, &Wire, MP_ESP32_SSD1306_RST);

void setup()
{
    pinMode(MP_VOICE_Pin_mute, INPUT);

    Serial.begin(115200);

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

    //WM8960 init
    uint8_t res = WM8960_Init(MP_VOICE_I2S_DIN, MP_VOICE_I2S_DOUT, MP_VOICE_I2S_LRC, MP_VOICE_I2S_BCLK);
    if (res != 0)
        Serial.println(res);

    //Connect to the WiFi network
    connectToWiFi(networkName, networkPswd);
    ipshow();
}

void loop()
{
    if (digitalRead(MP_VOICE_Pin_mute) == LOW)
    {
        delay(40);
        digitalWrite(MP_VOICE_Pin_mute, HIGH);
        if (digitalRead(MP_VOICE_Pin_mute) == LOW)
        {
            if (dev_mode != SAY_MODE)
            {
                speakshow();
                dev_mode = SAY_MODE;
            }

            while (1)
            {
                digitalWrite(MP_VOICE_Pin_mute, HIGH);

                //Prepare UDP packet
                udp.beginPacket(udpAddress, udpPort);

                WM8960_Record_psram(rec_buff, BUFF_LENGTH);
                //Sign Dev num
                rec_buff[0] = (uint8_t)dev_num;

                udp.write(rec_buff, BUFF_LENGTH);
                udp.endPacket();

                if (digitalRead(MP_VOICE_Pin_mute) == HIGH)
                    break;
            }

            ipshow();
        }
    }
    else
    {
        while (1)
        {
            int packetSize = udp.parsePacket();
            if (packetSize)
            {
                //UDP packet length is less than 1472, don't need psram
                //uint8_t *rec_buff = (uint8_t *)ps_malloc(BUFF_LENGTH);
                udp.read(rec_buff, packetSize);

                //Unrecive itself
                if (rec_buff[0] == dev_num)
                {
                    continue;
                }
                else
                {
                    IPAddress rec_ip;
                    rec_ip = udp.remoteIP();
                    Serial.print("From IP: ");
                    Serial.println(rec_ip);
                    if (dev_mode != LISTEN_MODE || rec_ip[3] != last_dev)
                    {
                        last_dev = rec_ip[3];
                        ipshow();
                        dev_mode = LISTEN_MODE;
                    }
                    WM8960_Play_psram(rec_buff, BUFF_LENGTH);
                    /*
                    Serial.print("From Port: ");
                    Serial.println(udp.remotePort());
                    */
                }
            }
            else
                break;
        }
    }
}

void connectToWiFi(const char *ssid, const char *pwd)
{
    Serial.println("Connecting to WiFi network: " + String(ssid));

    // delete old config
    WiFi.disconnect(true);
    //register event handler
    WiFi.onEvent(WiFiEvent);

    //Initiate connection
    WiFi.begin(ssid, pwd);

    Serial.println("Waiting for WIFI connection...");
}

//wifi event handler
void WiFiEvent(WiFiEvent_t event)
{
    IPAddress local_ip;
    switch (event)
    {
    case SYSTEM_EVENT_STA_GOT_IP:
        //When connected set
        local_ip = WiFi.localIP();
        Serial.print("WiFi connected! IP address: ");
        Serial.println(local_ip);

        dev_num = local_ip[3];
        Serial.print("Dev Num: ");
        Serial.println(dev_num);
        //initializes the UDP state
        //This initializes the transfer buffer
        udp.begin(WiFi.localIP(), udpPort);
        connected = true;
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        Serial.println("WiFi lost connection");
        connected = false;
        break;
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
    display.println(F("Interphone"));
    display.display();
    delay(2000);
}

//Show local and recevie ip
void ipshow(void)
{
    display.clearDisplay();

    display.setTextSize(2);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);             // Start at top-left corner
    display.print("Local:");
    display.println(dev_num);
    display.setCursor(0, 20); // Start at top-left corner
    display.print("Recive:");
    display.println(last_dev);
    display.display();
}

void speakshow(void)
{
    display.clearDisplay();

    display.setTextSize(2);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);             // Start at top-left corner
    display.print("Local:");
    display.println(dev_num);
    display.setCursor(0, 20); // Start at top-left corner
    display.println("Saying...");
    display.display();
}
