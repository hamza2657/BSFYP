/*
  Lora NodeB
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

//pins for sensors connected to Node B for currunt reading
#define H_2 34
#define H_3 26



//taking two instances from the library for our sensor to get values
EnergyMonitor emon1;
EnergyMonitor emon2;

// Flag set by callback to perform read process in main loop
volatile bool doRead = false;


//assign addresses for LoRa Transmission for master node and Node A and two varriablefor authentication request
byte MasterNode = 0xFF;
byte NodeB = 0xCC;
int recipient;
byte sender;

//defining varriable for currunt reading and incoming request from gateway
String incoming = "";
String currentReadings = "";

//variable store currunt readings
float house3, house2;

void setup()
{
  LoRa.setPins(nss, rst, dio0);
  while (!LoRa.begin(433E6))
    while (1);

  LoRa.onReceive(onReceive);             // register the receive callback
  LoRa.receive();                        // put the radio into receive mode

  //instances setting up pins and calibration value
  emon1.current(H_2, 4.9);
  emon2.current(H_3, 2.45);
}

void loop()
{
  //Dummy varriables to get value from sensor for making the value stable
  float dummy1   = emon1.calcIrms(1480);
  float dummy2   = emon2.calcIrms(1480);
 
 // parse for a packet, and call onReceive with the result:
  if (doRead)
  {
    readMessage();
    doRead = false;               // Set flag back to false so next read will happen only after next ISR event
    LoRa.onReceive(onReceive);
    LoRa.receive();               //again put the radio in recieve mode
  }
}

void onReceive(int packetSize)
{
  if (packetSize == 0)            // if there's no packet, return
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
  if (recipient != NodeB && sender != MasterNode)
    return;                             // skip rest of function
  int Val = incoming.toInt();
  if (Val == 20)                        //check if the request is for the node
  {
    house2   = emon1.calcIrms(1480);
    house3   = emon2.calcIrms(1480);
    currentReadings = house2;
    currentReadings +=  "/";
    currentReadings += house3;
    currentReadings += "#";
    sendMessage(currentReadings, MasterNode, NodeB);
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
  LoRa.print(outgoingMessage);                // add payload
  LoRa.endPacket();                           // finish packet and send it
}
