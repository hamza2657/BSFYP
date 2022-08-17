/*
  Master LoRa Node(Gateway)
*/
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <LoRa.h>
#include <NTPClient.h>
#include <WiFiUDP.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "de.pool.ntp.org", 18000, 60000); // Selecting Europe for getting real-time time stamps


volatile bool doRead = false; // Flag set by callback to perform read process in main loop
boolean flag1 = false, flag2 = false; // flag set for conforming that gate has recived data from every node


//Setting up LoRa Pins
#define nss 5  //GPIO 5
#define rst 14  //GPIO 14
#define dio0 4  //GPIO 4


//Setting up WiFi connectivity.
const char* ssid = "QAU";
const char* PASSWORD = "Abcd1dcbA2";

//setting up credentials for dataBase which in our case google excel sheets
const char* APP_SERVER = "script.google.com";
const char* key = "AKfycbyk4DJzRuIarOcUs07uNzmrpvUvuUK_EJCI6X_2FRTf9FjlCZQUn9MM2PpLjagG-1qq8w";
String serverTime = "";
String distPnt = "";
String path1 = "";
String path2 = "";
String path3 = "";
float theft;

//assigning addresses to the  gateway and senders
byte MasterNode = 0xFF;
byte NodeA = 0xBB;
byte NodeB = 0xCC;

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
int interval = 1 ;                        // updated every 1 second
int Secs = 0;

void setup()
{
  WiFi.begin(ssid, PASSWORD);
  pinMode(2, OUTPUT);
  while (WiFi.status() != WL_CONNECTED)
    digitalWrite(2, HIGH);               //if WiFi will not be connected LED will remain turn ON
  timeClient.begin();
  digitalWrite(2, LOW);                  //as the WiFi got connected LED will turn OFF
  LoRa.setPins(nss, rst, dio0);
  if (!LoRa.begin(433E6))
    while (1);

  // register the receive callback
  LoRa.onReceive(onReceive);
  // put the radio into receive mode
  LoRa.receive();
}

void loop()
{
  if (doRead)
  {
    readMessage();
    doRead = false;                 // Set flag back to false so next read will happen only after next ISR event
  }
  currentMillis = millis();
  currentsecs = currentMillis / 1000;
  if ((unsigned long)(currentsecs - previoussecs) >= interval)
  {
    if ( Secs >= 5 )
      Secs = 1;
    if (Secs == 2)
    {
      String message = "10";                    //an identifier msg act as ID for security
      sendMessage(message, MasterNode, NodeA);  //will send request to node A for values
      LoRa.onReceive(onReceive);
      LoRa.receive();                           //sets the Radio in recieve mode again
    }
    if (Secs == 4)
    {
      String message = "20";                    //an identifier msg act as ID for security
      sendMessage(message, MasterNode, NodeB);  //will send request to node B for values
      LoRa.onReceive(onReceive);
      LoRa.receive();                           //sets the Radio in recieve mode again
    }
    previoussecs = currentsecs;
    Secs = Secs + 1;
  }

  timeClient.update();
  serverTime = timeClient.getFormattedTime();    //get current time
}

//send a signal to each node for requesting the data of sensors.
void sendMessage(String outgoingRequestForData, byte MasterNode, byte otherNode) {
  LoRa.beginPacket();                         // start packet
  LoRa.write(otherNode);                      // add destination address
  LoRa.write(MasterNode);                     // add sender address
  LoRa.print(outgoingRequestForData);         // add payload
  LoRa.endPacket();                           // finish packet and send it

}

//call back function will confirm that here's some data on the gateway to be recieved
void onReceive(int packetSize)
{
  if (packetSize == 0)                        // if there's no packet, return
    return;
  doRead = true;
}

/*
   this function will read all the data and check if data is recived from both node and prepare
   the data to be uploaded to the sheets.
*/

void readMessage()
{
  // read packet header bytes:
  int recipient = LoRa.read();                // recipient address
  byte sender = LoRa.read();                  // sender address
  incomingValues = "";

  while (LoRa.available())
    incomingValues += (char)LoRa.read();      //reading datamessage

  if (sender == NodeA)                        //checking if data is from NodeA
  {
    int pos1 = incomingValues.indexOf('/');
    int pos2 = incomingValues.indexOf('#');
    path1 = incomingValues.substring(0, pos1);
    distPnt = incomingValues.substring(pos1 + 1, pos2);
    flag1 = true;
  }
  if (sender == NodeB)                        //checking if data is from NodeA
  {
    int pos1 = incomingValues.indexOf('/');
    int pos2 = incomingValues.indexOf('#');
    path2 = incomingValues.substring(0, pos1);
    path3 = incomingValues.substring(pos1 + 1, pos2);
    flag2 = true;
  }
  if (flag1 == true && flag2 == true)         //checking if data is recieved
  {
    theft = (path1.toFloat() + path2.toFloat() + path3.toFloat()) - distPnt.toFloat();
    accessToGoogleSheets();
    flag1 = false;
    flag2 = false;
  }
}

void accessToGoogleSheets()
{
  HTTPClient http;
  String URL = "https://script.google.com/macros/s/";
  URL += key;
  URL += "/exec?";
  URL += "time=";
  URL += serverTime;
  URL += "&distributionPoint=";
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
  http.GET();
}
