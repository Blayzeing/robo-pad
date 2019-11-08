#ifndef CONTROLLERCODE_H
#define CONTROLLERCODE_H
const char PAGE_HTML[] = "<html>\n\
<head>\n\
  <!--<meta http-equiv='refresh' content='5'/>!-->\n\
  <title>ESP8266 Demo</title>\n\
  <style>\n\
    body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\n\
  </style>\n\
  <script>\n\
\n\
var ledState = false;\n\
var theInterval;\n\
function init(){\n\
  console.log('init complete...');\n\
  var testSocket = new WebSocket(\"ws://192.168.4.1:81\");\n\
  console.log('connection attempted...');\n\
  testSocket.onopen = function (event) {\n\
    console.log('Sending data...');\n\
    testSocket.send(\"WE'RE IN.\");\n\
    /* Shutdown commands.*/\n\
    document.getElementById(\"disconnectBtn\").onclick=function(){\n\
      testSocket.close();\n\
      /*clearInterval(theInterval);*/\n\
    };\n\
    /* Control commands.*/\n\
    document.getElementById(\"LEDBtn\").onclick=function(){\n\
      ledState = !ledState;\n\
    }\n\
    console.log('setting interval...');\n\
    theInterval = setInterval(function(){console.log('sending data...'); sendData();},500);\n\
    function sendData(){\n\
      /* Should be an ArrayBuffer*/\n\
      testSocket.send(ledState?\"ledon\":\"ledoff\");\n\
      /*view[0] = ledState?1:0;\n\
      testSocket.send(view);*/\n\
    }\n\
  };\n\
}\n\
if(document.getElementById) window.onload=init;\n\
\n\
  </script>\n\
</head>\n\
<body>\n\
  <h1>This is the websocket page</h1>\n\
  <p>Bleh.</p>\n\
  <input id=\"disconnectBtn\" type=\"button\" value=\"Disconnect\"/>\n\
  <input id=\"LEDBtn\" type=\"button\" value=\"LED\"/>\n\
</body>\n\
</html>";
#endif
