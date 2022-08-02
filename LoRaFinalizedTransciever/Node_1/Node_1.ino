/*
  Lora Node1
*/
#include <SPI.h>              // include libraries
#include <LoRa.h>

#define ss 5
#define rst 14
#define dio0 2

String outgoingSendingMessage;              // outgoingSendingMessage message

byte MasterNode = 0xFF;
byte Node1 = 0xBB;



String Mymessage = "";

void setup() 
{
  Serial.begin(9600);                   // initialize serial
  while (!Serial);
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) 
    while (1);
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
  if (recipient != Node1 && sender != MasterNode) 
    return;                             // skip rest of function
  int Val = incoming.toInt();
  if (Val == 10)
  {
    Mymessage = "Node 1";
    sendMessage(Mymessage, MasterNode, Node1);
    delay(100);
    Mymessage = "";
  }
  else
    return;

}
void sendMessage(String outgoingSendingMessage, byte MasterNode, byte Node1) 
{
  LoRa.beginPacket();                            // start packet
  LoRa.write(MasterNode);                        // add destination address
  LoRa.write(Node1);                             // add sender address
  LoRa.write(outgoingSendingMessage.length());   // add payload length
  LoRa.print(outgoingSendingMessage);            // add payload
  LoRa.endPacket();                              // finish packet and send it
}
