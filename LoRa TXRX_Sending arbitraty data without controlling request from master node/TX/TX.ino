#include <LoRa.h>
#include <SPI.h>
#include "EmonLib.h"
 
#define ss 5
#define rst 14
#define dio0 2

EnergyMonitor elemon_2;
EnergyMonitor elemon_1;

float Irms;
int counter = 1;
float value = 0;




float readsensor()
{
  //if( sensornumber == 1)
  //{
   Irms = elemon_1.calcIrms(1480); 
  //}
//  if( sensornumber == 2)
//  {
//   Irms = elemon_2.calcIrms(1480); 
//  
//  }
  return Irms;
}

#if 0
void transmitter ()
{
  if(counter == 1)
  {
    LoRa.beginPacket();   //Send LoRa packet to receiver
    LoRa.print("Sensor 1/");
    LoRa.print(value);
    LoRa.print("#");
    LoRa.endPacket();
    Serial.println(value);
    
  }
  if(counter == 2)
  {
    LoRa.beginPacket();   //Send LoRa packet to receiver
    LoRa.print("Sensor 2/");
    LoRa.print(value);
    LoRa.print("#");
    LoRa.endPacket();
    Serial.println(value);
  }
}
#endif

void setup() 
{
//*****************LoRa*******************
  Serial.begin(9600); 
  LoRa.setPins(ss, rst, dio0);    //setup LoRa transceiver module
  while (!LoRa.begin(433E6))     //433E6 - Asia, 866E6 - Europe, 915E6 - North America
  {
    Serial.print(".");
     delay(500);
  }
  LoRa.setSyncWord(0xA5);

//*****************Sensor*****************

  elemon_1.current(36, 4.8);
  elemon_2.current(39, 4.8); 
}
 
void loop() 
{
  value = readsensor(counter);
  //Serial.println(value);
  //delay(100);
  //transmitter ();
  delay(2000);
  

  //counter++;
  //if (counter == 3)
  //counter == 1; 
}
