/*
  Lora Node2
*/
#include <SPI.h>              // include libraries
#include <LoRa.h>

#define nss 5
#define rst 14
#define dio0 2

String outgoingMessage;              // outgoingMessage message
byte MasterNode = 0xFF;
byte Node2 = 0xCC;



String Mymessage = "";

void setup() 
{
  LoRa.setPins(nss, rst, dio0);
  if (!LoRa.begin(433E6)) 
    while (true);
  
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
  if (recipient != Node2 && sender != MasterNode) 
    return;                             // skip rest of function
  
  
  int Val = incoming.toInt();
  if (Val == 20)
  {
    Mymessage = "Node 2";
    sendMessage(Mymessage, MasterNode, Node2);
    delay(100);
    Mymessage = "";
  }
  else
    return;

}
void sendMessage(String outgoingMessage, byte MasterNode, byte Node2) 
{
  LoRa.beginPacket();                         // start packet
  LoRa.write(MasterNode);                     // add destination address
  LoRa.write(Node2);                          // add sender address
  LoRa.write(outgoingMessage.length());       // add payload length
  LoRa.print(outgoingMessage);                // add payload
  LoRa.endPacket();                           // finish packet and send it
}
