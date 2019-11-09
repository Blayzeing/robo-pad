// GENERATOR: The below will have to change depending on features used (Arduino, by default will not allow any web stuff, only reciever data)
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPUpdateServer.h>

#include <Servo.h>

#include "ControllerCode.h"
#include "RoboPadCore.h"

//#define PPM
//#define PPM_MIXING
#define DEBUG

const char* ssid = "robo-pad";
const char* password = "roboteers";
const char* VERSION_NUMBER = "1.3c";
const char* SAFEBOOT_HTML = "ROBOPAD IS IN SAFEBOOT MODE, CLICK <a href='update'>HERE</a> TO UPLOAD NEW FIRMWARE.";
const char* HOME_HTML_FORMAT = "version: %s<br><a href='controller'>controller</a><br><a href='newController'>Web Controller</a><br><a href='spinner'>SPINNAH</a><br><a href='update'>updater</a>\0";
char home_html[190];
const char* NEW_CONTROLLER_HTML = "<html class=\"maxheight\">\n"
"<head>\n"
"  <title>ESP8266 Controller</title>\n"
"  <meta content='width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0' name='viewport' />\n"
"  <style>\n"
"    body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\n"
"    body { padding: 0; margin: 0; background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\n"
"    .row {\n"
"\t    display: flex;\n"
"    }\n"
"    .column {\n"
"\t    flex:50%;\n"
"    }\n"
"    .maxheight {\n"
"\t    height: 100%;\n"
"    }\n"
"    .slider{\n"
"      margin: 0 5%;\n"
"      background-color: #aaa;\n"
"      overflow:hidden;\n"
"      word-wrap:break-word;\n"
"    }\n"
"    .stick{\n"
"      height: 10%;\n"
"      background-color: #777;\n"
"      position: relative;\n"
"    }\n"
"  </style>\n"
"  <script>\n"
"  var testSocket = null;\n"
"  var ledState = false;\n"
"  var data = new ArrayBuffer(4);// 2 bytes, one for each wheel drive\n"
"  var int8View = new Int8Array(data);// View it\n"
"  var heartBeatData = new ArrayBuffer(1);\n"
"  function initConnection(){\n"
"    console.log('init connection complete...');\n"
"    testSocket = new WebSocket('ws://192.168.4.1:81');\n"
"    var heartbeatSender;\n"
"    console.log('connection attempted...');\n"
"\n"
"    // The data ArrayBuffer\n"
"    int8View[0] = 0;  // Servo\n"
"    int8View[1] = 255;// LED\n"
"    int8View[2] = 128;// Left Motor\n"
"    int8View[3] = 128;// Right Motor\n"
"\n"
"    testSocket.onopen = function (event) {\n"
"      console.log('Sending data...');\n"
"      testSocket.send('WE ARE IN.');\n"
"      heartbeatSender = setInterval(sendHeartbeat,1000);// Send a heartbeat every second\n"
"\n"
"      testSocket.onclose = function (event) {\n"
"        document.getElementById('statusSpan').innerHTML = \"Disconnected\";\n"
"        clearInterval(heartbeatSender);\n"
"      }\n"
"      function sendHeartbeat ()\n"
"      {\n"
"      if(testSocket.readyState == WebSocket.OPEN)\n"
"        testSocket.send(heartBeatData);\n"
"      }\n"
"    }\n"
"  }\n"
"\n"
"///////// UI ELEMENTS\n"
"\n"
"    lookuptable = {\"vertSlider1\":2,\"vertSlider2\":3};\n"
"\n"
"    // This slider should probably be a class, allowing for universal configurations such as slider reset to be made there instead of in-code.\n"
"    // Can probably transfer the setting of all those onmousedown etc stuff there too...\n"
"    var sliderWidth = 10;//percent\n"
"    var midpoint = (50-sliderWidth/2)+\"%\";\n"
"    function setSlider(name)\n"
"    {\n"
"      console.log(\"SETTING \" + name);\n"
"      var slider = document.getElementById(name);\n"
"      slider.onmousemove = moveTracker;\n"
"      slider.ontouchmove = moveTracker;\n"
"      function moveTracker(e)\n"
"      {\n"
"        console.log(\"MOVEMENT for \" + name);\n"
"        var rect = slider.getBoundingClientRect();\n"
"        var clientY = e.clientY || e.targetTouches[0].pageY;\n"
"        var y = (clientY - rect.top)/rect.height;\n"
"        var posFloat = (y*100 - sliderWidth/2);\n"
"        var pos = posFloat + \"%\";\n"
"        slider.getElementsByClassName(\"stick\")[0].style.top=pos;\n"
"\n"
"        // Send the data\n"
"        if(testSocket != null && testSocket.readyState == WebSocket.OPEN)\n"
"        {\n"
"          int8View[lookuptable[name]] = Math.round(Math.min(1,Math.max(0,posFloat/(100-sliderWidth))) * 254)\n"
"          testSocket.send(data);\n"
"        }\n"
"      }\n"
"    }\n"
"    function unsetSlider(name)\n"
"    {\n"
"      console.log(\"UNSETTING \" + name);\n"
"      var slider = document.getElementById(name);\n"
"      slider.onmousemove = null;\n"
"      slider.ontouchmove = null;\n"
"      slider.getElementsByClassName(\"stick\")[0].style.top=midpoint;\n"
"\n"
"      if(testSocket != null && testSocket.readyState == WebSocket.OPEN)\n"
"      {\n"
"        int8View[lookuptable[name]] = 128; //The stop signal.\n"
"        testSocket.send(data);\n"
"      }\n"
"    }\n"
"\n"
"    if(document.getElementById) window.onload = initConnection;\n"
"\n"
"  </script>\n"
"</head>\n"
"<body class=\"maxheight\">\n"
"<!--<canvas id=\"controller\" width=\"1000\" height=\"100%\"></canvas>-->\n"
"  <div class=\"row maxheight\">\n"
"      <div class=\"column slider\" id=\"vertSlider1\" style=\"height: 100%\" onmousedown=\"setSlider('vertSlider1');\" onmouseup=\"unsetSlider('vertSlider1');\" ontouchstart=\"setSlider('vertSlider1');\" ontouchend=\"unsetSlider('vertSlider1');\">\n"
"      <div class=\"stick\"></div>\n"
"    </div>\n"
"    <div class=\"column slider\" id=\"vertSlider2\" style=\"height: 100%\" onmousedown=\"setSlider('vertSlider2');\" onmouseup=\"unsetSlider('vertSlider2');\" ontouchstart=\"setSlider('vertSlider2');\" ontouchend=\"unsetSlider('vertSlider2');\">\n"
"      <div class=\"stick\"></div>\n"
"    </div>\n"
"  </div>\n"
"</body>\n"
"<body>\n"
"</body>\n"
"</html>\n"
"";

const char* NEW_SPINNER_CONTROLLER_HTML = "<html class=\"maxheight\">\n"
"<head>\n"
"  <title>ESP8266 Controller</title>\n"
"  <meta content='width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0' name='viewport' />\n"
"  <style>\n"
"    body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\n"
"    body { padding: 0; margin: 0; background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\n"
"    .row {\n"
"\t    display: flex;\n"
"    }\n"
"    .column {\n"
"\t    flex:50%;\n"
"    }\n"
"    .maxheight {\n"
"\t    height: 100%;\n"
"    }\n"
"    .slider{\n"
"      margin: 0 5%;\n"
"      background-color: #aaa;\n"
"      overflow:hidden;\n"
"      word-wrap:break-word;\n"
"    }\n"
"    .joystick{\n"
"      margin: 0 5%;\n"
"      background-color: #aaa;\n"
"      overflow:hidden;\n"
"      word-wrap:break-word;\n"
"    }\n"
"    .wideStick{\n"
"      height: 10%;\n"
"      width: 100%;\n"
"      background-color: #777;\n"
"      position: relative;\n"
"    }\n"
"    .stick{\n"
"      height: 10%;\n"
"      width: 10%;\n"
"      background-color: #777;\n"
"      position: relative;\n"
"    }\n"
"  </style>\n"
"  <script>\n"
"  var testSocket = null;\n"
"  var ledState = false;\n"
"  var data = new ArrayBuffer(4);// 2 bytes, one for each wheel drive\n"
"  var int8View = new Int8Array(data);// View it\n"
"  var heartBeatData = new ArrayBuffer(1);\n"
"  function initConnection(){\n"
"    console.log('init connection complete...');\n"
"    testSocket = new WebSocket('ws://192.168.4.1:81');\n"
"    var heartbeatSender;\n"
"    console.log('connection attempted...');\n"
"\n"
"    // The data ArrayBuffer\n"
"    int8View[0] = 0;  // Servo\n"
"    int8View[1] = 255;// LED\n"
"    int8View[2] = 128;// Left Motor\n"
"    int8View[3] = 128;// Right Motor\n"
"\n"
"    testSocket.onopen = function (event) {\n"
"      console.log('Sending data...');\n"
"      testSocket.send('WE ARE IN.');\n"
"      heartbeatSender = setInterval(sendHeartbeat,1000);// Send a heartbeat every second\n"
"\n"
"      testSocket.onclose = function (event) {\n"
"        document.getElementById('statusSpan').innerHTML = \"Disconnected\";\n"
"        clearInterval(heartbeatSender);\n"
"      }\n"
"      function sendHeartbeat ()\n"
"      {\n"
"      if(testSocket.readyState == WebSocket.OPEN)\n"
"        testSocket.send(heartBeatData);\n"
"      }\n"
"    }\n"
"  }\n"
"\n"
"///////// UI ELEMENTS\n"
"\n"
"    lookuptable = {\"vertSlider1\":0,\"vertSlider2\":3, \"joystick1x\":2, \"joystick1y\":3};\n"
"\n"
"///////// SLIDER\n"
"    // This slider should probably be a class, allowing for universal configurations such as slider reset to be made there instead of in-code.\n"
"    // Can probably transfer the setting of all those onmousedown etc stuff there too...\n"
"    var sliderWidth = 10;//percent\n"
"    var midpoint = (50-sliderWidth/2)+\"%\";\n"
"    function setSlider(name)\n"
"    {\n"
"      console.log(\"SETTING SLIDER \" + name);\n"
"      var slider = document.getElementById(name);\n"
"      slider.onmousemove = moveTracker;\n"
"      slider.ontouchmove = moveTracker;\n"
"      function moveTracker(e)\n"
"      {\n"
"        console.log(\"MOVEMENT for \" + name);\n"
"        var rect = slider.getBoundingClientRect();\n"
"        var clientY = e.clientY || e.targetTouches[0].pageY;\n"
"        var y = (clientY - rect.top)/rect.height;\n"
"        var posFloat = (y*100 - sliderWidth/2);\n"
"        var pos = posFloat + \"%\";\n"
"        slider.getElementsByClassName(\"wideStick\")[0].style.top=pos;\n"
"\n"
"        // Send the data\n"
"        if(testSocket != null && testSocket.readyState == WebSocket.OPEN)\n"
"        {\n"
"          int8View[lookuptable[name]] = Math.round(Math.min(1,Math.max(0,posFloat/(100-sliderWidth))) * 254)\n"
"          testSocket.send(data);\n"
"        }\n"
"      }\n"
"    }\n"
"    function unsetSlider(name)\n"
"    {\n"
"      console.log(\"UNSETTING \" + name);\n"
"      var slider = document.getElementById(name);\n"
"      slider.onmousemove = null;\n"
"      slider.ontouchmove = null;\n"
"      slider.getElementsByClassName(\"wideStick\")[0].style.top=midpoint;\n"
"\n"
"      if(testSocket != null && testSocket.readyState == WebSocket.OPEN)\n"
"      {\n"
"        int8View[lookuptable[name]] = 128; //The stop signal.\n"
"        testSocket.send(data);\n"
"      }\n"
"    }\n"
"    function unsetStaticSlider(name)\n"
"    {\n"
"      console.log(\"UNSETTING \" + name);\n"
"      var slider = document.getElementById(name);\n"
"      slider.onmousemove = null;\n"
"      slider.ontouchmove = null;\n"
"    }\n"
"\n"
"\n"
"///////// JOYSTICK\n"
"    var joystickSize = 10;//percent\n"
"    var joystickMidpoint = (50-joystickSize/2)+\"%\";\n"
"    function setJoystick(name)\n"
"    {\n"
"      console.log(\"SETTING JOYSTICK \" + name);\n"
"      var joystick = document.getElementById(name);\n"
"      joystick.onmousemove = moveTracker;\n"
"      joystick.ontouchmove = moveTracker;\n"
"      function moveTracker(e)\n"
"      {\n"
"        console.log(\"MOVEMENT for \" + name);\n"
"        var rect = joystick.getBoundingClientRect();\n"
"        var clientY = e.clientY || e.targetTouches[0].pageY;\n"
"        var clientX = e.clientX || e.targetTouches[0].pageX;\n"
"        var y = (clientY - rect.top)/rect.height;\n"
"        var x = (clientX - rect.left)/rect.width;\n"
"        var posYfloat = (y*100 - joystickSize/2);\n"
"        var posXfloat = (x*100 - joystickSize/2);\n"
"        var posY = posYfloat + \"%\";\n"
"        var posX = posXfloat + \"%\";\n"
"\n"
"        posYfloat = (posYfloat/100) - 0.5;\n"
"        posXfloat = (posXfloat/100) - 0.5;\n"
"\n"
"\n"
"        //chan 4 = x\n"
"        //chan 3 = y\n"
"        leftPower = -posXfloat/2+posYfloat;\n"
"        rightPower = posXfloat/2+posYfloat;\n"
"\n"
"\n"
"        joystick.getElementsByClassName(\"stick\")[0].style.top=posY;\n"
"        joystick.getElementsByClassName(\"stick\")[0].style.left=posX;\n"
"\n"
"        // Send the data\n"
"        if(testSocket != null && testSocket.readyState == WebSocket.OPEN)\n"
"        {\n"
"          //int8View[lookuptable[name + \"x\"]] = Math.round(Math.min(1,Math.max(0,rightPower/(100-joystickSize))) * 254);\n"
"          //int8View[lookuptable[name + \"y\"]] = Math.round(Math.min(1,Math.max(0,leftPower/(100-joystickSize))) * 254);\n"
"          int8View[lookuptable[name + \"x\"]] = Math.round(Math.min(1,Math.max(0,rightPower+0.5)) * 254);\n"
"          int8View[lookuptable[name + \"y\"]] = Math.round(Math.min(1,Math.max(0,leftPower+0.5)) * 254);\n"
"          testSocket.send(data);\n"
"        }\n"
"      }\n"
"    }\n"
"    function unsetJoystick(name)\n"
"    {\n"
"      console.log(\"UNSETTING \" + name);\n"
"      var slider = document.getElementById(name);\n"
"      slider.onmousemove = null;\n"
"      slider.ontouchmove = null;\n"
"      slider.getElementsByClassName(\"stick\")[0].style.top=joystickMidpoint;\n"
"      slider.getElementsByClassName(\"stick\")[0].style.left=joystickMidpoint;\n"
"\n"
"      if(testSocket != null && testSocket.readyState == WebSocket.OPEN)\n"
"      {\n"
"        int8View[lookuptable[name + \"x\"]] = 128; //The stop signal.\n"
"        int8View[lookuptable[name + \"y\"]] = 128; //The stop signal.\n"
"        testSocket.send(data);\n"
"      }\n"
"    }\n"
"\n"
"    if(document.getElementById) window.onload = initConnection;\n"
"\n"
"  </script>\n"
"</head>\n"
"<body class=\"maxheight\">\n"
"  <div class=\"row maxheight\">\n"
"    <div class=\"column slider\" id=\"vertSlider1\" style=\"height: 100%\" onmousedown=\"setSlider('vertSlider1');\" onmouseup=\"unsetStaticSlider('vertSlider1');\" ontouchstart=\"setSlider('vertSlider1');\" ontouchend=\"unsetStaticSlider('vertSlider1');\">\n"
"      <div class=\"wideStick\"></div>\n"
"    </div>\n"
"    <div class=\"column joystick\" id=\"joystick1\" style=\"height: 100%\" onmousedown=\"setJoystick('joystick1');\" onmouseup=\"unsetJoystick('joystick1');\" ontouchstart=\"setJoystick('joystick1');\" ontouchend=\"unsetJoystick('joystick1');\">\n"
"      <div class=\"stick\"></div>\n"
"    </div>\n"
"  </div>\n"
"</body>\n"
"<body>\n"
"</body>\n"
"</html>\n"
"";

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
  
  // Setting up the updater
  Serial.println("Initialising OTA updater...");
  httpUpdater.setup(&httpServer);
  Serial.println("Starting access point...");
  // Set up the http webserver in AP mode:
  WiFi.softAP(ssid, password);
  
  Serial.println("Checking safeboot...");
  pinMode(SAFEBOOT_PIN, INPUT);
  if(digitalRead(SAFEBOOT_PIN) == 1)
  {
    Serial.println("Safeboot activated! Ceasing operations bar updater. Please navigate to /update");
    httpServer.onNotFound(handleSafeboot);
    httpServer.begin();
    safeboot = true;
    return;
  }

  // Configure the page listeners
  sprintf(home_html, HOME_HTML_FORMAT, VERSION_NUMBER);
  httpServer.on("/", handleConnection);
  httpServer.on("/controller", handleController);
  httpServer.on("/newController", handleNewController);
  httpServer.on("/spinner", handleSpinnerController);
  httpServer.begin();
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("HTTPUpdateServer ready! Open http://");
  Serial.print(myIP);
  Serial.print(" in your browser.\n");

  // Set up the websocket server:
  webSocketServer.begin();
  webSocketServer.onEvent(webSocketEvent);
  Serial.println("WebSocket server started.");

  // Configure IO
  configureIO();

  #ifdef PPM
  attachInterrupt(PPM_PIN, interruptPPM, CHANGE);
  Serial.println("PPM interrupt attatched");
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
  Serial.println("HTTP '/' Request recieved while in safemode.");
  httpServer.send(200, "text/html", SAFEBOOT_HTML);
}
void handleController() {
  Serial.println("HTTP '/controller' Request recieved.");
  httpServer.send(200, "text/html", PAGE_HTML);
}
void handleConnection() {
  Serial.println("HTTP '/' Request recieved.");
  httpServer.send(200, "text/html", home_html);
}
void handleNewController(){
  Serial.println("HTTP '/' Request recieved.");
  httpServer.send(200, "text/html", NEW_CONTROLLER_HTML);
}
void handleSpinnerController(){
  Serial.println("HTTP '/' Request recieved.");
  httpServer.send(200, "text/html", NEW_SPINNER_CONTROLLER_HTML);
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
