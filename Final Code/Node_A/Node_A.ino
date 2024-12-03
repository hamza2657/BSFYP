/*
  LoRa Node A
  Current value below 0.25 is not measurable.
*/

// including libraries
#include <SPI.h>              
#include <LoRa.h>
#include <EmonLib.h>

//pin configuration for LoRa SX-1278
#define nss 5
#define rst 14
#define dio0 2

//pins for sensors connected to Node A for currunt reading
#define H_1 34
#define _main 27


//taking two instances from the library for our sensor to get values
EnergyMonitor emon1;
EnergyMonitor emon2;

// Flag set by callback to perform read process in main loop
volatile bool doRead = false; 


//assign addresses for LoRa Transmission for master node and Node A and two varriablefor authentication request
byte MasterNode = 0xFF;
byte NodeA = 0xBB;
int recipient;
byte sender;

//defining varriable for currunt reading and incoming request from gateway
String currentReadings = "";
String incoming = "";

//variable store currunt readings
float house1,Main;

void setup()
{
  LoRa.setPins(nss, rst, dio0);
  if (!LoRa.begin(433E6))
    while (1);
  
  LoRa.onReceive(onReceive);            // register the receive callback
  LoRa.receive();                       // put the radio into receive mode

  //instances setting up pins and calibration value
  emon1.current(H_1, 4.9);
  emon2.current(_main, 4.9);
}
void loop()
{
  //Dummy varriables to get value from sensor for making the value stable
  float dummy1 = emon1.calcIrms(1480);
  float dummy2 = emon2.calcIrms(1480);
  
  // parse for a packet, and call onReceive with the result:
  if (doRead)
  {
    readMessage();
    doRead = false;                     // Set flag back to false so next read will happen only after next ISR event
    LoRa.onReceive(onReceive);
    LoRa.receive();                     //again put the radio in recieve mode
  }
}
void onReceive(int packetSize) 
{
  if (packetSize == 0)                  // if there's no packet, return
    return;          
  doRead = true;
}
void readMessage()
{
  incoming = "";
  recipient = LoRa.read();
  sender = LoRa.read();
  while (LoRa.available())
    incoming += (char)LoRa.read();
  if (recipient != NodeA && sender != MasterNode)
    return;                             // skip rest of function
  int Val = incoming.toInt();
  if (Val == 10)                        //check if the request is for the node
  {
    house1 = emon1.calcIrms(1480);
    Main   = emon2.calcIrms(1480);
    currentReadings = house1;
    currentReadings +=  "/";
    currentReadings += Main;
    currentReadings += "#";
    sendMessage(currentReadings, MasterNode, NodeA);
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
  LoRa.print(outgoingSendingMessage);            // add payload
  LoRa.endPacket();                              // finish packet and send it
}
