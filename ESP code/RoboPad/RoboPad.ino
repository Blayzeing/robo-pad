// GENERATOR: The below will have to change depending on features used (Arduino, by default will not allow any web stuff, only reciever data)
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>


#include "ControllerCode.h"
#include "GlobalRobopadConfig.h"
#include "BasicHBridgeMotor.h"

#define DEBUG

const char* ssid = "robo-padd";
const char* password = "roboteers";
const char* VERSION_NUMBER = "1.0";
const char* HOME_HTML_FORMAT = "version: %s<br><a href='controller'>controller</a><br><a href='update'>updater</a>\0";
char home_html[100];

unsigned long lastHeartbeat;
unsigned int connectionCount = 0;
#define RADIO_SILENCE_AUTO_CUTOFF 3000 // If there's no data in 3 seconds then shut down.

ESP8266WebServer httpServer(80);
WebSocketsServer webSocketServer = WebSocketsServer(81);

void setup() {
  
  // Setting up Serial and access point
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting Sketch...");
  for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
  }

  Serial.print("version: ");
  Serial.println(VERSION_NUMBER);
  // Configure the version string
  sprintf(home_html, HOME_HTML_FORMAT, VERSION_NUMBER);

  // Set up the http webserver in AP mode:
  WiFi.softAP(ssid, password);
  httpServer.on("/", handleConnection);
  httpServer.on("/controller", handleController);
  httpServer.begin();
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("HTTPUpdateServer ready! Open http://");
  Serial.print(myIP);
  Serial.print(" in your browser.\n");

  // Set up the websocket server:
  webSocketServer.begin();
  webSocketServer.onEvent(webSocketEvent);

  // Configure IO
  configureIO();
}

BasicHBridgeMotor leftMotor(D7,D8);
BasicHBridgeMotor rightMotor(D6,D0);
void loop() {
  httpServer.handleClient();
  webSocketServer.loop();

  // Make sure we shutdown if we haven't recieved data recently or if we've lost connection to all controllers
  if(connectionCount < 1 || millis() - lastHeartbeat > RADIO_SILENCE_AUTO_CUTOFF)
  {
    //Serial.println("EMERGENCY STOP ENGUAGED.");
    leftMotor.stop();
    rightMotor.stop();
    //delay(10000);
  }
}

/// HTTP METHODS
void handleController() {
  Serial.println("HTTP '/controller' Request recieved.");
  httpServer.send(200, "text/html", PAGE_HTML);
}
void handleConnection() {
  Serial.println("HTTP '/' Request recieved.");
  httpServer.send(200, "text/html", home_html);
}

/// WEBSOCKET METHODS:
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      connectionCount --;
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocketServer.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        connectionCount++;
        lastHeartbeat = millis();
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);

      break;
    case WStype_BIN:
      lastHeartbeat = millis();
      if(length == 1)// This is just a heartbeat bump
      {
#ifdef DEBUG
        Serial.printf("[%u] Heartbeat recieved.\n", num);
#endif
        break;
      }
#ifdef DEBUG
      Serial.printf("[%u] get binary length: %u\n", num, length);
      Serial.printf("[%u] Data[0]: %u\n", num, payload[0]);
      Serial.printf("[%u] Data[1]: %u\n", num, payload[1]);
      Serial.printf("[%u] Data[2]: %u\n", num, payload[2]);
      Serial.printf("[%u] Data[3]: %u\n", num, payload[3]);
#endif
      parseData(payload);
      break;
  }
}


/// IO METHODS:
// GNERATOR: Cycles through every element in the physical config layer, generates a variable based on that. The below names are generated based on the names of the elements in the physical config layer.
///BasicHBridgeMotor leftMotor(D7,D8);
///BasicHBridgeMotor rightMotor(D6,D0);
void configureIO() {
  // GENERATOR: Cycles through every element in the physical config layer, generates a section down here based on that (Mostly this will just be setting up servos and LEDs
}

void parseData(uint8_t * data)
{
  // Byte 0
  /// This is the servo
  
  // Byte 1
  if(data[1] > 0)
  {
  digitalWrite(LED_BUILTIN, HIGH);
  }else{
  digitalWrite(LED_BUILTIN, LOW);
  }
  
  // Motor Bytes
  leftMotor.drive(data[2], 0, 255);
  rightMotor.drive(data[3], 0, 255);
}
