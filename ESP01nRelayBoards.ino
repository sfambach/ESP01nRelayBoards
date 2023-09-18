/******************************************************************************
* Test for relay boards with ESP01
* The test programm switch all relays one after another to on. After that all will be switched off sequentially. 
* Libs:
* None specific
*
* Licence: AGPL3
* Author: S. Fambach
* Visit http://www.fambach.net if you want
******************************************************************************/
#include <Arduino.h>

// choose what you want 
#define WIFI_ACTIVE
#define OTA_ACTIVE

//#define ONE_RELAY
#define TWO_RELAY
//#define FOUR_RELAY
//#define EIGHT_RELAY

/** Debuging *****************************************************************/
#define DEBUG
#ifdef DEBUG
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif

/** Wifi *****************************************************************/

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#elif defined ESP32
#include <WiFi.h>
#include <ESPmDNS.h>
#else
//#error Unknown controller
#endif


#ifdef WIFI_ACTIVE
#ifndef SSID
#define SSID "NODES"
#define SSID_PASSWD "HappyNodes1234"
#endif
#endif  // WIFI_ACTIVE


/** OTA *****************************************************************/
#ifdef OTA_ACTIVE
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

void setupOTA() {

  // Port defaults to 8266 / 3232
#ifdef ESP8266
  ArduinoOTA.setPort(8266);
#elif defined ESP32
  //ArduinoOTA.setPort(3232);
#endif
  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("NewOTAClient");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("ef84180df45cf2f22993e3a03dc71a27");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }                                                                                                                                                                                                                                                                                                                                                              
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
#endif  // OTA_ACTIVE

/** Relay *****************************************************************/



#ifdef ONE_RELAY

const uint8_t RELAY_COUNT = 1;
const uint8_t RELAY_PIN = 0  ;
//const uint8_t LED = ?;

#elif defined TWO_RELAY
const uint8_t RELAY_COUNT = 2;
//const uint8_t LED = ?;

#elif defined FOUR_RELAY
const uint8_t RELAY_COUNT = 4;
//const uint8_t LED = ?;

#elif defined EIGHT_RELAY
const uint8_t RELAY_COUNT = 8;
//const uint8_t LED = ?;

#endif

/** Set Relay state
* Relay = 1 .... n
* state = HIGH / LOW
*/
void setRelay(uint8_t relay, bool state) {
  
  if (relay < 1 || relay > RELAY_COUNT) {
    DEBUG_PRINT("Wrong Relay Index: ");
    DEBUG_PRINTLN(relay);
  }

#ifdef ONE_RELAY
  digitalWrite(RELAY_PIN, state);

#else
  uint8_t iState = (state?1:0);
  Serial.write(0xA0);
  Serial.write(0x00 | relay);
  Serial.write(0x00 | iState);
  Serial.write(0xA0 | (relay+state));

#endif
}

void setupRelays() {

#ifdef ONE_RELAY
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

#else
  for (int i = 1; i < RELAY_COUNT; i++) {
    setRelay(i, LOW);
  }

#endif  // ONE_RELAY
}



long currMillis = 0;
long delayMS = 1000;
uint8_t counter = 1;
bool state = HIGH;

void testRelays() {

  long remaining = millis() - (currMillis + delayMS);

  if (remaining >= 0) {
    counter++;
    currMillis = millis();
    if (counter > RELAY_COUNT) {
      counter = 1;
      state = !state;
    }

   /* DEBUG_PRINT("relay: ");
    DEBUG_PRINT(counter);
    DEBUG_PRINT(" state : ");
    DEBUG_PRINTLN((state ? "HIGH" : "LOW"));*/
    setRelay(counter, state);
  }
}

/** Main Programm ************************************************************/
void setup() {
  Serial.begin(115200);
 //  DEBUG_PRINTLN("Setup");

#ifdef WIFI_ACTIVE
  // wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, SSID_PASSWD);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  //Serial.println(WiFi.localIP);
#endif  // WIFI_ACTIVE


// do the ota
#ifdef OTA_ACTIVE
  setupOTA();
#endif  //  OTA_ACTIVE

  // relays
  setupRelays();
}


void loop() {
//DEBUG_PRINTLN("Main Loop");

// handle ota
#ifdef OTA_ACTIVE
  ArduinoOTA.handle();
#endif  // OTA_ACTIVE

  testRelays();
}


/** Rest of implementations **************************************************/
