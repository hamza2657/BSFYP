#include <LoRa.h>
#include <SPI.h>
 
#define ss 15
#define rst 16
#define dio0 2
 
int counter = 0;
 
void setup() 
{
  Serial.begin(9600); 
  LoRa.setPins(ss, rst, dio0);    //setup LoRa transceiver module
  
  while (!LoRa.begin(433E6))     //433E6 - Asia, 866E6 - Europe, 915E6 - North America
  {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xA5);
}
 
void loop() 
{ 
  LoRa.beginPacket();   //Send LoRa packet to receiver
  LoRa.print("ESP_2/");
  LoRa.print(counter);
  LoRa.print("#");
  LoRa.endPacket();
  counter++;
  delay(20000);
}
