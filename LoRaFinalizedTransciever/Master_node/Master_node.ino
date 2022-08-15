/*
  Master Lora Node
*/
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>              
#include <LoRa.h>
#include <NTPClient.h>
#include <WiFiUDP.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "in.pool.ntp.org", 18000, 60000);




boolean flag1 = false,flag2 = false;


#define nss 5  //GPIO 5
#define rst 14  //GPIO 14
#define dio0 4  //GPIO 4


//Setting up WiFi connectivity.
const char* ssid = "hamza";
const char* PASSWORD = "12345678";

//setting up credentials for dataBase which in our case google excel sheets
const char* APP_SERVER = "script.google.com";
const char* key = "AKfycbx5xg6qZLJedWXvxT22P7kiQEIm3NV8IJ7V2zDZ8KWrAYsdxZnvoindoJS6mVurpbzOww";
String serverTime="";
String distPnt = "";
String path1= "";
String path2= "";
String path3 = "";
float theft;
             

//function for uploading data to the dataBase for monitoring
void accessToGoogleSheets() 
{
  HTTPClient http;
  String URL = "https://script.google.com/macros/s/";
  URL += key;
  URL += "/exec?";
  URL += "time=";
  URL += serverTime;
  URL +="&distributionPoint=";
  URL += distPnt;
  URL += "&path1=";
  URL += path1;
  URL += "&path2=";
  URL += path2;
  URL += "&path3=";
  URL += path3;
  URL += "&theft=";
  URL += theft;
  http.begin(URL);
  int code = http.GET();
  if(code == HTTP_CODE_OK)
  {
    digitalWrite(2,HIGH);
  }
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
  pinMode(2,OUTPUT);
  WiFi.begin(ssid, PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
      delay(100);
  timeClient.begin();
  LoRa.setPins(nss, rst, dio0);
  if (!LoRa.begin(433E6)) 
      while (1);
}

void loop() {
  digitalWrite(2,LOW);
  currentMillis = millis();
  currentsecs = currentMillis / 1000;
  if ((unsigned long)(currentsecs - previoussecs) >= interval) 
  {
    Secs = Secs + 1;
    if ( Secs >= 5 )
      Secs = 0;
    
    if ( (Secs >= 1) && (Secs <= 2) )
    {
      
      String message = "10";
      sendMessage(message, MasterNode, NodeA);
    }

    else if ( (Secs >= 3 ) && (Secs <= 4))
    {
      String message = "20";
      sendMessage(message, MasterNode, NodeB);
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
      path1 = incomingValues.substring(0, pos1);
      distPnt = incomingValues.substring(pos1 + 1, pos2);
      flag1 = true;
  }
  if(sender == NodeB)
  {
      int pos1 = incomingValues.indexOf('/');
      int pos2 = incomingValues.indexOf('#');
      path2 = incomingValues.substring(0, pos1);
      path3 = incomingValues.substring(pos1 + 1, pos2);
      flag2 = true;
  }
  if(flag1 == true && flag2 == true)
  {
    theft = (path1.toFloat() + path2.toFloat() + path3.toFloat()) - distPnt.toFloat();
    for(int i = 0 ; i<=5 ; i++ )
    {
      timeClient.update();
      String dummp = timeClient.getFormattedTime();
      if(i == 5)
      serverTime = timeClient.getFormattedTime();
    }
    accessToGoogleSheets();
    flag1 = false;
    flag2 = false;
    
  }
   
}
