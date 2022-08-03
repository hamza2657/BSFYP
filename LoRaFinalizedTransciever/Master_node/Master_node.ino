/*
  Master Lora Node
*/
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>              
#include <LoRa.h>


#define nss 5  //GPIO 5
#define rst 14  //GPIO 14
#define dio0 4  //GPIO 4


//Setting up WiFi connectivity.
const char* ssid = "hamza";
const char* PASSWORD = "12345678";

//setting up credentials for dataBase which in our case google excel sheets
const char* APP_SERVER = "script.google.com";
const char* key = "AKfycbz0RrOwM1eoqAZckkSjgMm4QiPVTIKIttz-_QBXyIwQT15THQ2YMwI85ARof0DGcme1-Q";
             

//function for uploading data to the dataBase for monitoring
void accessToGoogleSheets(String reading, String value) 
{
  HTTPClient http;
  String URL = "https://script.google.com/macros/s/";
  URL += key;
  URL += "/exec?";
  URL += reading;
  URL +="=";
  URL += value;
  http.begin(URL);
  http.GET();
}


//assigning addresses to the  gateway and senders
byte MasterNode = 0xFF;
byte NodeA = 0xBB;
byte NodeB = 0xCC;

String SenderNode = "";
String outgoingRequestForData;              // outgoingRequestForData message

//varriable for storing and for sending data to cloud  
String incomingValues = "";
String currentMain = "";
String currentA = "";
String currentB = "";
String currentC = ""; 

// Tracks the time since last event fired
unsigned long previousMillis = 0;
unsigned long int previoussecs = 0;
unsigned long int currentsecs = 0;
unsigned long currentMillis = 0;
int interval = 1 ; // updated every 1 second
int Secs = 0;

void setup() 
{
  LoRa.setPins(nss, rst, dio0);
  if (!LoRa.begin(433E6)) 
      while (1);
  WiFi.begin(ssid, PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
      delay(100);   
      
}

void loop() {
  currentMillis = millis();
  currentsecs = currentMillis / 1000;
  if ((unsigned long)(currentsecs - previoussecs) >= interval) 
  {
    Secs = Secs + 1;
    if ( Secs >= 20 )
      Secs = 0;
    
    if ( (Secs >= 1) && (Secs <= 10) )
    {
      if (Secs == 5)
      {
      String message = "10";
      sendMessage(message, MasterNode, NodeA);
      }
    }

    if ( (Secs >= 11 ) && (Secs <= 20))
    {
      if (Secs == 15)
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
  if(sender == NodeA)
  {
      int pos1 = incomingValues.indexOf('/');
      int pos2 = incomingValues.indexOf('#');
      currentB = incomingValues.substring(0, pos1);
      accessToGoogleSheets("currentB", currentB);
      currentC = incomingValues.substring(pos1 + 1, pos2);
      accessToGoogleSheets("currentC", currentC);
  }
  if(sender == NodeB)
  {
      int pos1 = incomingValues.indexOf('/');
      int pos2 = incomingValues.indexOf('#');
      currentMain = incomingValues.substring(0, pos1);
      accessToGoogleSheets("currentMain", currentMain);
      currentA = incomingValues.substring(pos1 + 1, pos2);
      accessToGoogleSheets("currentA", currentA);
      Serial.print(currentMain);
   }
      
  
  

  if (incomingValuesLength != incomingValues.length())    // check length for error
    return;                                   // skip rest of function
  

  if (sender != NodeA &&sender!= NodeB && recipient != MasterNode) 
    return;                             // skip rest of function
  
}
