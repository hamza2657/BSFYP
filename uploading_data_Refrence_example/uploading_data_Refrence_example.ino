#include <HTTPClient.h>

//local varriable for testing
int i = 5;

// Unique ID
String UniqueID = "ESP_1";

// Connecting WiFi Settings
const char* SSID = "QAU";           // WiFi SSID
const char* PASSWORD = "Abcd1dcbA2";   // WiFi Password
// Accessed Google Script Settings
const char* APP_SERVER = "script.google.com";
const char* KEY = "AKfycbyDiKErYXNIXeoWTIScAMduoyLj8XZyHgDHYP4HqJY7Biz4eELw0ZERU7P7RsD7EDeUfQ";

// Device sleep time (sec) to reduce Joule heat
uint64_t DEEP_SLEEP_TIME_SEC = 5;


void accessToGoogleSheets(char *temperature, char *humidity, char *illumination) {
    HTTPClient http;
    String URL = "https://script.google.com/macros/s/";

    URL += KEY;
    URL += "/exec?";
    URL += "UniqueID=";
    URL += UniqueID;
    URL += "&temperature=";
    URL += temperature;
    URL += "&humidity=";
    URL += humidity;
    URL += "&illumination=";
    URL += illumination;

    Serial.println("[HTTP] begin...");
    Serial.println(URL);
    // access to your Google Sheets
    Serial.println();
    // configure target server and url
    http.begin(URL);

    Serial.println("[HTTP] GET...");
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if(httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.print("[HTTP] GET... code: ");
        Serial.println(httpCode);

        // file found at server
        if(httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            Serial.println(payload);
        }
    } else {
        Serial.print("[HTTP] GET... failed, error: ");
        Serial.println(http.errorToString(httpCode).c_str());
    }
}

//const char* getTemperature() {
//    
//    //String temperature = "HAM";
//    return "HAM" /*temperature*/;
//}
//
//const char* getHumidity() {
////    String humidity = "RIZ";
////    return humidity;
//      return "HAM";
//}
//
//const char* getillumination() {
////    String illumination = "MUS";
////    return illumination;
//      return "HAM";
//}
//
void espDeepSleep() 
{
    esp_sleep_enable_timer_wakeup(DEEP_SLEEP_TIME_SEC * 1000 * 1000);  // set deep sleep time
    esp_deep_sleep_start();   // enter deep sleep
}

void setup() {

    
    // WiFi connection controlling parameters
    int statusCheckCounter = 0;
    const int CHECK_NUM_MAX = 100;

    Serial.begin(115200);
    WiFi.begin(SSID, PASSWORD);
    Serial.print("WiFi connecting");
    while (WiFi.status() != WL_CONNECTED) {
        if(statusCheckCounter > CHECK_NUM_MAX) {
            WiFi.disconnect(true);
            Serial.println("failed");
            Serial.println("Deepsleep Start");
            espDeepSleep();
        }
        Serial.print(WiFi.status());
        delay(500);
        statusCheckCounter++;
    }
    Serial.println("\nconnected");

}

void loop() {

//    char temperature[] = getTemperature();
//    char humidity[] = getHumidity();
//    char illumination[] = getillumination();

    char temperature[] = "Hamza";
    char humidity[] = "Rizwan";
    char illumination[] = "Mustajab";

//    Serial.print("\ntemperature : ");
//    Serial.println(temperature);
//    Serial.print("humidity    : ");
//    Serial.println(humidity);
//    Serial.print("illumination  : ");
//    Serial.println(illumination);

    // send sensor values to google sheets
    accessToGoogleSheets(temperature, humidity, illumination);

    // WiFi Connection killed
    Serial.println("\nWiFi is disconnected");
    WiFi.disconnect(true);

    // ESP32 start deep sleep
    // After calling "esp_deep_sleep_start" function, any following codes will not be executed
    // When restarting ESP32, all variables are restored and the program will start from the beginning
    Serial.println("Deepsleep Start");
    espDeepSleep();

}
