#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WebSocketsServer.h>

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
  httpUpdater.setup(&httpServer);

  // Setting up the websocket controller
  httpServer.on("/controller", handleController);

  httpServer.begin();

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("HTTPUpdateServer ready! Open http://");
  Serial.print(myIP);
  Serial.println("/update in your browser");

  // Setting up the websocket stuff
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop(void){
  //if(millis() - lastTime > 60000)
  //{
  //  lastTime = millis();
  //  Serial.println("hi");
  //}
  httpServer.handleClient();
  webSocket.loop();
}

void handleController() {
  String page = 
           "<html>\n\
            <head>\n\
              <!--<meta http-equiv='refresh' content='5'/>!-->\n\
              <title>ESP8266 Demo</title>\n\
              <style>\n\
                body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\n\
              </style>\n\
              <script>\n\
                var ledState = false;\n\
                var theInterval;\n\
                function init(){\n\
                  console.log('init complete...');\n\
                  var testSocket = new WebSocket(\"ws://192.168.4.1:81\");\n\
                  console.log('connection attempted...');\n\
                  testSocket.onopen = function (event) {\n\
                    console.log('Sending data...');\n\
                    testSocket.send(\"WE'RE IN.\");\n\
                    // Shutdown commands.\n\
                    document.getElementById(\"disconnectBtn\").onclick=function(){\n\
                      testSocket.close();\n\
                      //clearInterval(theInterval);\n\
                    };\n\
                    // Control commands.\n\
                    document.getElementById(\"LEDBtn\").onclick=function(){\n\
                      ledState = !ledState;\n\
                    }\n\
                    console.log('setting interval...');\n\
                    theInterval = setInterval(function(){console.log('sending data...'); sendData();},500);\n\
                    function sendData(){\n\
                      // Should be an ArrayBuffer\n\
                      testSocket.send(ledState?\"ledon\":\"ledoff\");\n\
                    }\n\
                  };\n\
                }\n\
                if(document.getElementById) window.onload=init;\n\
              </script>\n\
            </head>\n\
            <body>\n\
              <h1>This is the websocket page</h1>\n\
              <p>Bleh.</p>\n\
              <input id=\"disconnectBtn\" type=\"button\" value=\"Disconnect\"/>\n\
              <input id=\"LEDBtn\" type=\"button\" value=\"LED\"/>\n\
            </body>\n\
          </html>";

  httpServer.send(200, "text/html", page);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch(type) {
    case WStype_DISCONNECTED:
      //USE_SERIAL.printf("[%u] Disconnected!\n", num);
      Serial.print(num);
      Serial.println(" Disconnected!");
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
    
        // send message to client
        webSocket.sendTXT(num, "Connected");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);

      // send message to client
      // webSocket.sendTXT(num, "message here");

      // send data to all connected clients
      // webSocket.broadcastTXT("message here");
      break;
    case WStype_BIN:
      Serial.printf("[%u] get binary length: %u\n", num, length);
      hexdump(payload, length);

      // send message to client
      // webSocket.sendBIN(num, payload, length);
      break;
  }

}
