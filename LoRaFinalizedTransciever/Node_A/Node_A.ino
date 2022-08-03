/*
  Lora NodeA
  Current value below 0.25 is not measurable.
*/
#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <EmonLib.h>

//pin configuration for LoRa SX-1278
#define nss 5
#define rst 14
#define dio0 2

//pins for sensors connected to Node A for currunt reading
#define H_2 34
#define H_3 27

EnergyMonitor emon1;                   
EnergyMonitor emon2;


String outgoingSendingMessage;              // outgoingSendingMessage message

//assign addresses for LoRa Transmission from master noode to others node and vice versa
byte MasterNode = 0xFF;
byte NodeA = 0xBB;



String currentReadings = "";

void setup() 
{
  LoRa.setPins(nss, rst, dio0);
  if (!LoRa.begin(433E6)) 
    while (1);

   emon1.current(H_2, 4.9);
   emon2.current(H_3, 4.9);
}

void loop() 
{
  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
}

void onReceive(int packetSize) {
  if (packetSize == 0) 
      return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingLength = LoRa.read();    // incoming msg length
  String incoming = "";

  while (LoRa.available()) 
    incoming += (char)LoRa.read();
  if (incomingLength != incoming.length())    // check length for error
     return;                             // skip rest of function
  

  // if the recipient isn't this device or broadcast,
  if (recipient != NodeA && sender != MasterNode) 
    return;                             // skip rest of function
  int Val = incoming.toInt();
  if (Val == 10)
  {
    float house2 = emon1.calcIrms(1480);  
    float house3 = emon2.calcIrms(1480);
    currentReadings = house2;
    currentReadings +=  "/";
    currentReadings += house3; 
    currentReadings += "#";
    sendMessage(currentReadings, MasterNode, NodeA);
    delay(100);
    currentReadings = "";
  }
  else
    return;

}
void sendMessage(String outgoingSendingMessage, byte MasterNode, byte NodeA) 
{
  LoRa.beginPacket();                            // start packet
  LoRa.write(MasterNode);                        // add destination address
  LoRa.write(NodeA);                             // add sender address
  LoRa.write(outgoingSendingMessage.length());   // add payload length
  LoRa.print(outgoingSendingMessage);            // add payload
  LoRa.endPacket();                              // finish packet and send it
}
