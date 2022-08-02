/**********Required libraries **********/
#include<WiFi.h>
#include <LoRa.h>
#include <SPI.h>
#include <HTTPClient.h>


/***********Wifi and Excel Sheet Setup for data uploading **********/
String UniqueID = "";
String power = "";
const char* ssid = "QAU";
const char* PASSWORD = "Abcd1dcbA2";

const char* APP_SERVER = "script.google.com";
const char* KEY_H1 = "AKfycbyDMTqACYsa1UJdU4XWCLOdlSFIw6FKB0wT-aj_higTwWDpgx2jziM3tLYkjUAaxZ-KzA";
const char* KEY_H2 = "AKfycbzEFMbt9Wf5EJX0MBSekG4kcho2Lgpat_WXBiOvqBPPdOAchLvRUrZC8KGkg3QTkbnP5g";
const char* key;

uint64_t DEEP_SLEEP_TIME_SEC = 8;  // sleep time in sec.

void accessToGoogleSheets() 
{
  HTTPClient http;
  String URL = "https://script.google.com/macros/s/";
  URL += KEY_H1;
  URL += "/exec?";
  URL += "UniqueID=";
  URL += UniqueID;
  URL += "&power=";
  URL += power;
 Serial.println(URL);
  http.begin(URL);
  http.GET();
}

//void espDeepSleep() 
//{
//    esp_sleep_enable_timer_wakeup(DEEP_SLEEP_TIME_SEC * 1000 * 1000);  // set deep sleep time
//    esp_deep_sleep_start();   // enter deep sleep
//}


/******setting up LoRa*************/
#define ss 5
#define rst 14
#define dio0 2

void setup()
{
  Serial.begin(9600);
  WiFi.begin(ssid, PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  LoRa.setPins(ss, rst, dio0);    //setup LoRa transceiver module
  while (!LoRa.begin(433E6))     //433E6 - Asia, 866E6 - Europe, 915E6 - North America
  {
        delay(500);
  }
  LoRa.setSyncWord(0xA5);
}

void loop()
{
  int packetSize = LoRa.parsePacket();    // try to parse packet
  if (packetSize)
  {
    while (LoRa.available())              // read packet
    {
      String LoRaData = LoRa.readString();
      int pos1 = LoRaData.indexOf('/');
      int pos2 = LoRaData.indexOf('#');
      UniqueID = LoRaData.substring(0, pos1);
      power = LoRaData.substring(pos1 + 1, pos2);
      Serial.print(UniqueID);
      Serial.print(" = ");
      Serial.println(power);
     }
//    if (UniqueID == "Sensor 1")
//      key = KEY_H1;
//    if (UniqueID == "ESP_2")
//      key = KEY_H2;
    accessToGoogleSheets();
  }


}
