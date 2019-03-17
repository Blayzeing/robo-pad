#include <ArduinoWebsockets.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
using namespace websockets;

const char* ssid = "robo-padd";
const char* password = "roboteers";

ESP8266WebServer httpServer(1337);
ESP8266HTTPUpdateServer httpUpdater(true);
WebsocketsServer server;

unsigned long lastTime = 0;

void setup(void){

  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting Sketch...");

  WiFi.softAP(ssid, password);

  httpUpdater.setup(&httpServer);
  httpServer.begin();

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("HTTPUpdateServer ready! Open http://");
  Serial.print(myIP);
  Serial.println("/update in your browser");

  server.listen(80);
  Serial.print("Server status: ");
  Serial.println(server.available());
}

void loop(void){
  if(millis() - lastTime > 1000)
  {
    lastTime = millis();
    /*Serial.println("Entered.");
    // This happens every second
    auto client = server.accept();
    if(client.available()) {
      auto msg = client.readBlocking();
      Serial.print("Got Message: ");
      Serial.println(msg.data());
      client.send("Echo: " + msg.data());
      client.close();
    }*/
    Serial.println("hi");
  }
  httpServer.handleClient();
}

/*
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <FS.h>   // Include the SPIFFS library
#include <ArduinoOTA.h>

#ifndef APSSID
#define APSSID "robo-pad"
#define APPSK  "roboteers"
#endif

const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer server(80);
File fsUploadFile;              // a File object to temporarily store the received file

// Just a little test message.  Go to http://192.168.4.1 in a web browser
// connected to this access point to see it.

void handleRoot() {
  String data = "<h1>RoboPad</h1><br><form method=\"post\" enctype=\"multipart/form-data\">\
  <input type=\"file\" name=\"name\">\
  <input class=\"button\" type=\"submit\" value=\"Upload\">\
  </form>";
  server.send(200, "text/html", data);
}
void handleSuccess() {
  String data = "<h1>The file's uploaded!</h1>";
  server.send(200, "text/html", data);
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/upload", HTTP_POST,                       // if the client posts to the upload page
    [](){ server.send(200); },                          // Send status 200 (OK) to tell the client we are ready to receive
    handleFileUpload                                    // Receive and save the file
  );
  server.on("/", handleRoot);
  server.on("/success", handleSuccess);
  server.begin();
  Serial.println("HTTP server started");
}

void handleFileUpload(){ // upload a new file to the SPIFFS
  // Upload the file
  HTTPUpload& upload = server.upload();
  if(upload.status == UPLOAD_FILE_START){
    String filename = upload.filename;
    if(!filename.startsWith("/")) filename = "/"+filename;
    Serial.print("handleFileUpload Name: "); Serial.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");            // Open the file for writing in SPIFFS (create if it doesn't exist)
    filename = String();
  } else if(upload.status == UPLOAD_FILE_WRITE){
    if(fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
  } else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile) {                                    // If the file was successfully created
      fsUploadFile.close();                               // Close the file again
      Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
      server.sendHeader("Location","/success");      // Redirect the client to the success page
      server.send(303);
    } else {
      server.send(500, "text/plain", "500: couldn't create file");
    }
  }

  // Reflashing the chip?
  //WiFiClient stream = http.getStream();
  //if(Update.begin(updateLength)) {
  //  Serial.printf("starting OTA may take a minute or two...\n");
  //  Update.writeStream(stream);
  //  if(Update.end()) {
  //    Serial.printf("update done, now finishing...\n");
  //    if(Update.isFinished()) {
  //      Serial.printf("update successfully finished; rebooting...\n\n");
  //      ESP.restart();
  //    } else {
  //      Serial.printf("update didn't finish correctly :(\n");
  //    }
  //  } else {
  //    Serial.printf("an update error occurred, #: %d\n" + Update.getError());
  //  }
  //} else {
  //  Serial.printf("not enough space to start OTA update :(\n");
  //}
  //stream.flush();
}

void loop() {
  server.handleClient();
}
*/
