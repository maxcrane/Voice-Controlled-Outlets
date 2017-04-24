#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>
#include <functional>
#include "switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"

// wifi credentials
const char* ssid = "your network name";
const char* password = "your network pass";

// callbacks
void turnOnOutlet();
void turnOffOutlet();

// Define your switches here. Max 14
// Format: Alexa invocation name, local port no, on callback, off callback
#define NUMSWITCHES  1
Switch *allSwitches[] = {
                          new Switch("lamp", 80, turnOnOutlet, turnOffOutlet)
                        };
                        
// The pin on the ESP8266 that is connected to the NeoPixels
#define PIN            D3

// Global variables
boolean wifiConnected = false;
UpnpBroadcastResponder upnpBroadcastResponder;

// prototypes
boolean connectWifi();

void setup() {
  ESP.wdtDisable();
  ESP.wdtEnable(WDTO_8S);
  Serial.begin(9600);
  pinMode(PIN, OUTPUT);
  // Initialise wifi connection
  wifiConnected = connectWifi();
  
  if(wifiConnected){
    upnpBroadcastResponder.beginUdpMulticast();
   
    Serial.println("Adding switches upnp broadcast responder");
    for(int i = 0; i < NUMSWITCHES; i++){
      upnpBroadcastResponder.addDevice(*allSwitches[i]);
    }
  }  
}

void loop() {
  ESP.wdtFeed();

  if(wifiConnected){
    upnpBroadcastResponder.serverLoop();
    for(int i = 0; i < NUMSWITCHES; i++){
      allSwitches[i]->serverLoop();
    } 
  }
}
      
// connect to wifi – returns true if successful or false if not
boolean connectWifi(){
  boolean state = true;
  int i = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 10){
      state = false;
      break;
    }
    i++;
  }
  
  if (state){
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  
  return state;
}

void turnOnOutlet() {
  digitalWrite(PIN, HIGH);
}

void turnOffOutlet() {
  digitalWrite(PIN, LOW);
}
