// GENERATOR: The below will have to change depending on features used (Arduino, by default will not allow any web stuff, only reciever data)
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>


#include "ControllerCode.h"
#include "GlobalRobopadConfig.h"
#include "BasicHBridgeMotor.h"

const char* ssid = "robo-padd";
const char* password = "roboteers";

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

  // Set up the http webserver in AP mode:
  WiFi.softAP(ssid, password);
  httpServer.on("/", handleConnection);
  httpServer.on("/controller", handleController);
  httpServer.begin();
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("HTTPUpdateServer ready! Open http://");
  Serial.print(myIP);
  Serial.print(" in your browser.");

  // Set up the websocket server:
  webSocketServer.begin();
  webSocketServer.onEvent(webSocketEvent);

  // Configure IO
  configureIO();
}

void loop() {
  httpServer.handleClient();
  webSocketServer.loop();
}

/// HTTP METHODS
void handleController() {
  Serial.println("HTTP '/controller' Request recieved.");
  httpServer.send(200, "text/html", PAGE_HTML);
}
void handleConnection() {
  Serial.println("HTTP '/' Request recieved.");
  httpServer.send(200, "text/html", "<a href='controller'>controller</a><br><a href='update'>updater</a>");
}

/// WEBSOCKET METHODS:
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocketServer.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);

      break;
    case WStype_BIN:
      Serial.printf("[%u] get binary length: %u\n", num, length);
      Serial.printf("[%u] Data[0]: %u\n", num, payload[0]);
      Serial.printf("[%u] Data[1]: %u\n", num, payload[1]);
      Serial.printf("[%u] Data[2]: %u\n", num, payload[2]);
      Serial.printf("[%u] Data[3]: %u\n", num, payload[3]);
      //hexdump(payload, length);
      //parseData(payload);
      break;
  }

}

/// IO METHODS:
// GNERATOR: Cycles through every element in the physical config layer, generates a variable based on that. The below names are generated based on the names of the elements in the physical config layer.
BasicHBridgeMotor leftMotor(4,5);
BasicHBridgeMotor rightMotor(12,14);
void configureIO() {
  // GENERATOR: Cycles through every element in the physical config layer, generates a section down here based on that (Mostly this will just be setting up servos and LEDs
}
