/*
  Lora NodeB
  Current value below 0.25 is not measurable.
*/
#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <EmonLib.h>

#define nss 5
#define rst 14
#define dio0 2

//pins for sensors connected to Node A for currunt reading
#define H_1 34
#define main 27

EnergyMonitor emon1;                   
EnergyMonitor emon2;

int msgcount = 0;

String outgoingMessage;              // outgoingMessage message
byte MasterNode = 0xFF;
byte NodeB = 0xCC;



String currentReadings = "";

void setup() 
{
  LoRa.setPins(nss, rst, dio0);
  if (!LoRa.begin(433E6)) 
    while (true);

  emon1.current(main, 4.9);
  emon2.current(H_1, 4.9);
  
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
  if (incomingLength != incoming.length())   // check length for error
    return;                                  // skip rest of function
  

  // if the recipient isn't this device or broadcast,
  if (recipient != NodeB && sender != MasterNode) 
    return;                             // skip rest of function
  
  
  int Val = incoming.toInt();
  if (Val == 20)
  {
    float Main   = emon1.calcIrms(1480);  
    float house1 = emon2.calcIrms(1480);
    currentReadings = Main;
    currentReadings +=  "/";
    currentReadings += house1; 
    currentReadings += "#";
    sendMessage(currentReadings, MasterNode, NodeB);
    delay(100);
    currentReadings = "";
   }
   else
    return;
}
void sendMessage(String outgoingMessage, byte MasterNode, byte NodeB) 
{
  LoRa.beginPacket();                         // start packet
  LoRa.write(MasterNode);                     // add destination address
  LoRa.write(NodeB);                          // add sender address
  LoRa.write(outgoingMessage.length());       // add payload length
  LoRa.print(outgoingMessage);                // add payload
  LoRa.endPacket();                           // finish packet and send it
}
