/*
  Master Lora Node
*/
#include <SPI.h>              
#include <LoRa.h>


#define nss 5  //GPIO 5
#define rst 14  //GPIO 14
#define dio0 4  //GPIO 4

byte MasterNode = 0xFF;
byte NodeA = 0xBB;
byte NodeB = 0xCC;

String SenderNode = "";
String outgoingRequestForData;              // outgoingRequestForData message

String incomingValues = "";

// Tracks the time since last event fired
unsigned long previousMillis = 0;
unsigned long int previoussecs = 0;
unsigned long int currentsecs = 0;
unsigned long currentMillis = 0;
int interval = 1 ; // updated every 1 second
int Secs = 0;

void setup() 
{
  Serial.begin(9600);                   // initialize serial
  delay(500);
  while (!Serial);
  LoRa.setPins(nss, rst, dio0);
  if (!LoRa.begin(433E6)) 
      while (1);
}

void loop() {
  currentMillis = millis();
  currentsecs = currentMillis / 1000;
  if ((unsigned long)(currentsecs - previoussecs) >= interval) 
  {
    Secs = Secs + 1;
    if ( Secs >= 11 )
      Secs = 0;
    
    if ( (Secs >= 1) && (Secs <= 5) )
    {
      if (Secs == 3)
      {
      String message = "10";
      sendMessage(message, MasterNode, NodeA);
      }
    }

    if ( (Secs >= 6 ) && (Secs <= 10))
    {
      if (Secs == 8)
      {
        String message = "20";
        sendMessage(message, MasterNode, NodeB);
      }
      else
       return;
     }
    previoussecs = currentsecs;
  }

  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());

}
void sendMessage(String outgoingRequestForData, byte MasterNode, byte otherNode) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(otherNode);              // add destination address
  LoRa.write(MasterNode);             // add sender address
  LoRa.write(outgoingRequestForData.length());        // add payload length
  LoRa.print(outgoingRequestForData);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  
}
void onReceive(int packetSize) 
{
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingValuesLength = LoRa.read();    // incomingValues msg length
  incomingValues = "";
  while (LoRa.available()) 
    incomingValues += (char)LoRa.read();      //reading datamessage
  Serial.println(incomingValues);
  

  if (incomingValuesLength != incomingValues.length())    // check length for error
    return;                                   // skip rest of function
  

  if (sender != NodeA &&sender!= NodeB && recipient != MasterNode) 
    return;                             // skip rest of function
  
}
