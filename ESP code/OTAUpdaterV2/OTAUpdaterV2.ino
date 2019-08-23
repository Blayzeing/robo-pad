#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WebSocketsServer.h>

#include <Servo.h>

#include "ControllerCode.h"
//#include <OTAUpdaterV2.h>

//TODO:
// - Try MDNS
// - JS generator minification
// -- minification
// -- comment removal
// -- 
// - Stop the robot from doing anything else while it's updating (does it already do this?)
// - Define the ArrayBuffer contents dynamically, configure which bits mean what
// -- Should probably be the minimum length of the data being sent, aka bools use 1 bit, drive commands 1 byte.
// -- Booleans should probably be bundled into groups of 4, so they can be sent as one byte.
// - Add yeild() command, to let TCP stack things happen
// - Create motor driver class to handle reversals etc

const char* ssid = "robo-padd";
const char* password = "roboteers";

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater(true);
WebSocketsServer webSocket = WebSocketsServer(81);

unsigned long lastTime = 0;

void setup(void){

  // Setting up Serial and access point
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting Sketch...");

  WiFi.softAP(ssid, password);

  // Setting up the updater
  //httpUpdater.setup(&httpServer);

  // Setting up the websocket controller
  httpServer.on("/controller", handleController);

  httpServer.begin();

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("HTTPUpdateServer ready! Open http://");
  Serial.print(myIP);
  Serial.println("/update in your browser");

  // Setting up the websocket stuff
  /*webSocket.begin();
  webSocket.onEvent(webSocketEvent);*/

  configureHardware();// Configure the hardware connections
}

void loop(void){
  if(millis() - lastTime > 6000)
  {
    lastTime = millis();
    Serial.println("Running...");
  }
  httpServer.handleClient();
  webSocket.loop();
  yield();// A little wait to do stuff
  // TODO: datastructure.runTheThings()
  // For now though we'll just run the motors independantly
}

void handleController() {
  httpServer.send(200, "text/html", PAGE_HTML);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
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
      parseData(payload);
      break;
  }

}

/////// THE BELOW CODE WILL BE MOSTLY AUTO-GENERATED
// Some kind of datastructure that defines the format of the data packet should go here
// For now, just a definition of a motor

Servo testServo;
void configureHardware()
{
  // Based on the datastructure, configure the pinouts and what each thing does etc
  //pinMode(14, OUTPUT);
  
  // Servo configuration
  testServo.attach(15);
  pinMode(LED_BUILTIN, OUTPUT);

  // Drive motor configuration
  // Left Motor(s)
  pinMode(9, OUTPUT);
  pinMode(5, OUTPUT);

  // Right Motor(s)
  pinMode(12, OUTPUT);
  pinMode(16, OUTPUT);
}
void parseData(uint8_t * data)
{
  Serial.println("Parsing data...");
  // Byte 0
  int val = map(data[0], 0, 255, 0, 180);
  testServo.write(val);
  // Byte 1
  if(data[1] > 0)
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }else{
    digitalWrite(LED_BUILTIN, LOW);
  }

  //bytes 2&3 (left motor drive speed/direction)
  int leftMotorDrive = map(data[2], 0, 255, 0, 2048) - 1024;
  int rightMotorDrive = map(data[3], 0, 255, 0, 2048) - 1024;

  if(leftMotorDrive < 0)
  {
    // Drive the left motor backwards
    analogWrite(4, abs(leftMotorDrive));
  }else{
    // Drive the left motor forwards
    analogWrite(5, leftMotorDrive);
  }
  if(rightMotorDrive < 0)
  {
    // Drive the left motor backwards
    analogWrite(12, abs(rightMotorDrive));
  }else{
    // Drive the left motor forwards
    analogWrite(14, rightMotorDrive);
  }
}

// send message to client
//webSocket.sendTXT(num, "Connected");
// send message to client
// webSocket.sendTXT(num, "message here");
// send data to all connected clients
// webSocket.broadcastTXT("message here");
// send message to client
// webSocket.sendBIN(num, payload, length);
