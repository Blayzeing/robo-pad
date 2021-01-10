#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPUpdateServer.h>

#include <Servo.h>

#include "htmlStrings-compressed.h"
#include "RoboPadCore.h"
#include "ConfigManager.h"

//#define PPM
//#define PPM_MIXING
#define DEBUG

const char* ssid = "robo-pad-dev";
const char* password = "roboteers";
const char* VERSION_NUMBER = "1.6b";
#define SAFEBOOT_HTML F("ROBOPAD IS IN SAFEBOOT MODE, CLICK <a href='update'>HERE</a> TO UPLOAD NEW FIRMWARE.")
#define ENCODING_ERROR F("Error: Unfortunately, RoboPad does not support devices that do not support gzip encoding.")

const char* HOME_HTML_FORMAT = "version: %s<br><a href='tankDrive'>Tank Drive Controller</a><br><a href='spinner'>Joystick Controller</a><br><a href='update'>updater</a>\0";
char home_html[200];

unsigned long lastHeartbeat;
unsigned int connectionCount = 0;
bool safeboot = false;
#define RADIO_SILENCE_AUTO_CUTOFF 3000 // If there's no data in 3 seconds then shut down.
#define SAFEBOOT_PIN 16 // Used as a switch (pulled low by circuit, pull high to safeboot)

#ifdef PPM
#define PPM_CHANNELS 8
#define PPM_PIN 3
#define PPM_MIN_PULSE_DURATION 550
#define PPM_MAX_PULSE_DURATION 1850
volatile unsigned char channels[PPM_CHANNELS];
#endif

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater(true);
WebSocketsServer webSocketServer = WebSocketsServer(81);

//For spinner
Servo esc;

ConfigManager configManager;

void setup() {
  
  // Setting up Serial and access point
  Serial.begin(115200);
  Serial.println();
  Serial.println(F("Booting Sketch..."));
  for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
  }

#ifdef DEBUG
//  // Checks for fletcher checksum functionality (mostly future-proofing)
//  Serial.print("Wrap-around confirmation (should equal 0): ");
//  uint8_t checkValue1 = 255;
//  Serial.println(++checkValue1);
//  Serial.println("Debug fletcher check-byte processing check:");
//  checkValue1 = 0;
//  uint8_t checkValue2 = 0;
//  char testString0[] = "abcde";
//  char testString1[] = "abcdef";
//  char testString2[] = "abcdefgh";
//  size_t i;
//  for (i = 0; i<5; i++)
//    ConfigManager::ConfigData::fletcherProcessByte((uint8_t)testString0[i], checkValue1, checkValue2);
//  Serial.println("\tExpected:\t");
//  Serial.print("\tReturned:\t");
//  Serial.print(checkValue1, HEX);
//  Serial.print("\t");
//  Serial.print(checkValue2, HEX);
//  Serial.println();
//  checkValue1 = 0; checkValue2 = 0;
//
//  for (i = 0; i<6; i++)
//    ConfigManager::ConfigData::fletcherProcessByte((uint8_t)testString1[i], checkValue1, checkValue2);
//  Serial.println("\tExpected:\t");
//  Serial.print("\tReturned:\t");
//  Serial.print(checkValue1, HEX);
//  Serial.print("\t");
//  Serial.print(checkValue2, HEX);
//  Serial.println();
//  checkValue1 = 0; checkValue2 = 0;
//  //for (i = 0; i<6; i++)
//  //  ConfigManager::ConfigData::fletcherProcessByte((uint8_t)testString1[i], checkValue1, checkValue2);
//  //checkValue1 = 0; checkValue2 = 0;
//  //for (i = 0; i<8; i++)
//  //  ConfigManager::ConfigData::fletcherProcessByte((uint8_t)testString2[i], checkValue1, checkValue2);
#endif

  Serial.print(F("version: "));
  Serial.println(VERSION_NUMBER);

  // Load the user config
  Serial.println(F("Attempting to load saved configuration..."));
  configManager.loadFromEEPROM();
  
  // Setting up the updater
  Serial.println(F("Initialising OTA updater..."));
  httpUpdater.setup(&httpServer);
  Serial.println(F("Starting access point..."));
  // Set up the http webserver in AP mode:
  WiFi.softAP(ssid, password);
  
  Serial.println(F("Checking safeboot..."));
  pinMode(SAFEBOOT_PIN, INPUT);
  /*if(digitalRead(SAFEBOOT_PIN) == 1)
  {
    Serial.println("Safeboot activated! Ceasing operations bar updater. Please navigate to /update");
    httpServer.onNotFound(handleSafeboot);
    httpServer.begin();
    safeboot = true;
    return;
  }*/

  // Set up the websocket server:
  Serial.println(F("Booting Websocket server..."));
  webSocketServer.begin();
  webSocketServer.onEvent(webSocketEvent);
  Serial.println(F("WebSocket server started."));

  // Configure the page listeners
  Serial.println(F("Booting web server..."));
  const char* headerKeys[] = {"Accept-Encoding"};
  httpServer.collectHeaders(headerKeys, 1);
  sprintf(home_html, HOME_HTML_FORMAT, VERSION_NUMBER);
  httpServer.on("/", handleConnection);
  httpServer.on("/tankDrive", handleTankDriveController);
  httpServer.on("/spinner", handleSpinnerController);
  httpServer.on("/info", handleInfo);
  httpServer.begin();
  IPAddress myIP = WiFi.softAPIP();
  Serial.print(F("Webserver ready! Open http://"));
  Serial.print(myIP);
  Serial.println(F(" in your browser."));


  // Configure IO
  configureIO();
  
  #ifdef PPM
  attachInterrupt(PPM_PIN, interruptPPM, CHANGE);
  Serial.println("PPM interrupt attatched");
  #endif
  
  #ifndef DEBUG
  Serial.println("Shutting down serial...");
  Serial.end();
  #endif
}

#ifdef PPM
long counter = 0;
long startMicros = 0;
long minSync = 2500;
volatile bool ppmActive = false;

ICACHE_RAM_ATTR void interruptPPM () {
  if (digitalRead(PPM_PIN)) {
    startMicros = micros();
  } else {
    long duration = micros() - startMicros;
    if (duration > minSync) {
      counter = 0;
    } else {
      channels[counter] = map(duration, PPM_MIN_PULSE_DURATION, PPM_MAX_PULSE_DURATION, 0.0, 255.0);
      counter += 1;
    }
  }

  ppmActive = true;
}
#endif

BasicHBridgeMotor leftMotor(14,15);
BasicHBridgeMotor rightMotor(12,13);
void loop() {
  httpServer.handleClient();

  if(safeboot) return;// Make sure we don't do anything when safeboot is operational.

  #ifdef PPM
  if (ppmActive) {
    esc.writeMicroseconds(map(channels[2], 0, 255, 1000, 2000));
    
    #ifdef PPM_MIXING
    leftMotor.drive(constrain((channels[1]-127)+(channels[0]-127), -127, 127)+127, 0, 255);
    rightMotor.drive(constrain((channels[1]-127)-(channels[0]-127), -127, 127)+127, 0, 255);
    #else
    leftMotor.drive(channels[0], 0, 255);
    rightMotor.drive(channels[1], 0, 255);
    #endif
    
    ppmActive = false;
  }
  #endif
  
  webSocketServer.loop();

  // Make sure we shutdown if we haven't recieved data recently or if we've lost connection to all controllers
  if((connectionCount < 1 || millis() - lastHeartbeat > RADIO_SILENCE_AUTO_CUTOFF)
  #ifdef PPM
  // Additional check to see if we've also not had a PPM update recently
  && (!ppmActive && micros() - startMicros > RADIO_SILENCE_AUTO_CUTOFF*1000)
  #endif
  )
  {
    //Serial.println("EMERGENCY STOP ENGUAGED.");
    leftMotor.stop();
    rightMotor.stop();
    esc.writeMicroseconds(1000);
    //delay(10000);
  }
}

/// HTTP METHODS
void handleSafeboot() {
  #ifdef DEBUG
  Serial.println(F("HTTP '/' Request recieved while in safemode."));
  #endif
  httpServer.send(200, "text/html", SAFEBOOT_HTML);
}
void handleConnection() {
  #ifdef DEBUG
  Serial.println("HTTP '/' Request recieved.");
  #endif
  httpServer.send(200, "text/html", home_html);
}

void sendCompressedHTML(const uint8_t* compressedHTML, size_t stringLength)
{
  // Guards against clients that do not accept gzip encoded pages:
  if(httpServer.hasHeader("Accept-Encoding"))
  {
    #ifdef DEBUG
    Serial.println(F("Client accepts encodings:"));
    Serial.println(httpServer.header("Accept-Encoding"));
    #endif
    if(httpServer.header("Accept-Encoding").indexOf("gzip")==-1)
    {
      // Return error
      #ifdef DEBUG
      Serial.println(F("WARN: Client does not accept gzip encoding."));
      #endif
      httpServer.send(200, "text/html", ENCODING_ERROR);
      return;
    }
  }else{
    // Return error
    #ifdef DEBUG
    Serial.println(F("WARN: Client did not send an 'Accept-Encoding' header."));
    #endif
    httpServer.send(200, "text/html", ENCODING_ERROR);
    return;
  }

  WiFiClient wc = httpServer.client();
  wc.println(F("HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Encoding: gzip\nConnection: close"));
  wc.println("Content-Length: " + String(stringLength));
  wc.println();

  uint8_t* stagedHTML = (uint8_t*)malloc(sizeof(uint8_t)*stringLength);
  memcpy_P(stagedHTML, compressedHTML, sizeof(uint8_t)*stringLength);
  wc.write(stagedHTML, stringLength);
  free(stagedHTML);
}

void handleTankDriveController(){
  #ifdef DEBUG
  Serial.println(F("HTTP '/tankDrive' Request recieved."));
  #endif
  sendCompressedHTML(TANK_DRIVE_CONTROLLER_HTML, TANK_DRIVE_CONTROLLER_HTML_SIZE);
}
void handleSpinnerController(){
  #ifdef DEBUG
  Serial.println(F("HTTP '/spinner' Request recieved."));
  #endif
  sendCompressedHTML(SPINNAH_CONTROLLER_HTML, SPINNAH_CONTROLLER_HTML_SIZE);
}
void handleManagement(){
  #ifdef DEBUG
  Serial.println(F("HTTP '/mgnt' Request recieved."));
  #endif
  //sendCompressedHTML(MANAGEMENT_PAGE_HTML, MANAGEMENT_PAGE_HTML_SIZE);
}
void handleInfo(){
  // Returns configured settings
  #ifdef DEBUG
  Serial.println(F("HTTP '/info' Request recieved."));
  #endif
  // Perhaps this should be allowed to set infos
  //configManager.writeToWifiClient(httpServer.client());
}

/// WEBSOCKET METHODS:
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_BIN:
      lastHeartbeat = millis();
      if(length == 1)// This is just a heartbeat bump
      {
        #ifdef DEBUG
        Serial.printf("[%u] Heartbeat recieved.\n", num);
        #endif
        //TODO: Respond with battery level report
      }else{
        #ifdef DEBUG
        Serial.printf("[%u] get binary length: %u\n", num, length);
        Serial.printf("[%u] Data[0]: %u\n", num, payload[0]);
        Serial.printf("[%u] Data[1]: %u\n", num, payload[1]);
        Serial.printf("[%u] Data[2]: %u\n", num, payload[2]);
        Serial.printf("[%u] Data[3]: %u\n", num, payload[3]);
        #endif
        parseData(payload);
      }
      break;
    case WStype_DISCONNECTED:
      #ifdef DEBUG
      Serial.printf("[%u] Disconnected!\n", num);
      #endif
      connectionCount --;
      break;
    case WStype_CONNECTED:
      {
        #ifdef DEBUG
        IPAddress ip = webSocketServer.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        #endif
        connectionCount++;
        lastHeartbeat = millis();
      }
      break;
    case WStype_TEXT:
      #ifdef DEBUG
      Serial.printf("[%u] get Text: %s\n", num, payload);
      #endif
      break;
  }
}


/// IO METHODS:
// GNERATOR: Cycles through every element in the physical config layer, generates a variable based on that. The below names are generated based on the names of the elements in the physical config layer.
///BasicHBridgeMotor leftMotor(D7,D8);
///BasicHBridgeMotor rightMotor(D6,D0);
void configureIO() {
  // GENERATOR: Cycles through every element in the physical config layer, generates a section down here based on that (Mostly this will just be setting up servos and LEDs

  #ifdef PPM
  pinMode(PPM_PIN, INPUT);
  #endif

  esc.attach(16);
  esc.writeMicroseconds(1000);
}

void parseData(uint8_t * data) {
  // Byte 0
  /// This is the servo NOPE SPINNAH
  esc.writeMicroseconds(map(255-data[0], 0, 255, 1000,2000));
  
  // Byte 1
  if(data[1] > 0) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
  
  // Motor Bytes
  leftMotor.drive(data[2], 0, 255);
  rightMotor.drive(data[3], 0, 255);
}
