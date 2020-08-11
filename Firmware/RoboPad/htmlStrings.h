#ifndef HTML_STRINGS_H
#define HTML_STRINGS_H
//===========================================================
const char SPINNAH_CONTROLLER_HTML[] = "<html class=\"maxheight\">\n\
<head>\n\
<title>ESP8266 Controller</title>\n\
<meta content='width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0' name='viewport' />\n\
<style>\n\
body{overscroll-behavior-y:contain;padding:0;margin:0;background-color:#cccccc;font-family:Arial,Helvetica,Sans-Serif;Color:#000088;}\n\
.row{\n\
display:flex;\n\
}\n\
.column{\n\
flex:50%;\n\
}\n\
.maxheight{\n\
height:100%;\n\
}\n\
.slider{\n\
margin:0 5%;\n\
background-color:#aaa;\n\
overflow:hidden;\n\
word-wrap:break-word;\n\
}\n\
.joystick{\n\
margin:0 5%;\n\
background-color:#aaa;\n\
overflow:hidden;\n\
word-wrap:break-word;\n\
}\n\
.wideStick{\n\
height:10%;\n\
width:100%;\n\
background-color:#777;\n\
position:relative;\n\
}\n\
.stick{\n\
height:10%;\n\
width:10%;\n\
background-color:#777;\n\
position:relative;\n\
}\n\
</style>\n\
<script>\n\
var testSocket=null;\n\
var ledState=false;\n\
var data=new ArrayBuffer(4);\n\
var int8View=new Int8Array(data);\n\
var heartBeatData=new ArrayBuffer(1);\n\
function initConnection(){\n\
console.log('init connection complete...');\n\
testSocket=new WebSocket('ws://192.168.4.1:81');\n\
var heartbeatSender;\n\
console.log('connection attempted...');\n\
int8View[0]=0;\n\
int8View[1]=255;\n\
int8View[2]=128;\n\
int8View[3]=128;\n\
testSocket.onopen=function(event){\n\
console.log('Sending data...');\n\
testSocket.send('WE ARE IN.');\n\
heartbeatSender=setInterval(sendHeartbeat,1000);\n\
testSocket.onclose=function(event){\n\
document.getElementById('statusSpan').innerHTML=\"Disconnected\";\n\
clearInterval(heartbeatSender);\n\
}\n\
function sendHeartbeat()\n\
{\n\
if(testSocket.readyState==WebSocket.OPEN)\n\
testSocket.send(heartBeatData);\n\
}\n\
}\n\
}\n\
lookuptable={\"vertSlider1\":0,\"vertSlider2\":3,\"joystick1x\":2,\"joystick1y\":3};\n\
var sliderWidth=10;\n\
var midpoint=(50-sliderWidth/2)+\"%\";\n\
function setSlider(name)\n\
{\n\
console.log(\"SETTING SLIDER \"+name);\n\
var slider=document.getElementById(name);\n\
slider.onmousemove=moveTracker;\n\
slider.ontouchmove=moveTracker;\n\
function moveTracker(e)\n\
{\n\
console.log(\"MOVEMENT for \"+name);\n\
var rect=slider.getBoundingClientRect();\n\
var clientY=e.clientY||e.targetTouches[0].pageY;\n\
var y=(clientY-rect.top)/rect.height;\n\
var posFloat=(y*100-sliderWidth/2);\n\
var pos=posFloat+\"%\";\n\
slider.getElementsByClassName(\"wideStick\")[0].style.top=pos;\n\
if(testSocket!=null&&testSocket.readyState==WebSocket.OPEN)\n\
{\n\
int8View[lookuptable[name]]=Math.round(Math.min(1,Math.max(0,posFloat/(100-sliderWidth)))*254)\n\
testSocket.send(data);\n\
}\n\
}\n\
}\n\
function unsetSlider(name)\n\
{\n\
console.log(\"UNSETTING \"+name);\n\
var slider=document.getElementById(name);\n\
slider.onmousemove=null;\n\
slider.ontouchmove=null;\n\
slider.getElementsByClassName(\"wideStick\")[0].style.top=midpoint;\n\
if(testSocket!=null&&testSocket.readyState==WebSocket.OPEN)\n\
{\n\
int8View[lookuptable[name]]=128;\n\
testSocket.send(data);\n\
}\n\
}\n\
function unsetStaticSlider(name)\n\
{\n\
console.log(\"UNSETTING \"+name);\n\
var slider=document.getElementById(name);\n\
slider.onmousemove=null;\n\
slider.ontouchmove=null;\n\
}\n\
var joystickSize=10;\n\
var joystickMidpoint=(50-joystickSize/2)+\"%\";\n\
function setJoystick(name)\n\
{\n\
console.log(\"SETTING JOYSTICK \"+name);\n\
var joystick=document.getElementById(name);\n\
joystick.onmousemove=moveTracker;\n\
joystick.ontouchmove=moveTracker;\n\
function moveTracker(e)\n\
{\n\
console.log(\"MOVEMENT for \"+name);\n\
var rect=joystick.getBoundingClientRect();\n\
var clientY=e.clientY||e.targetTouches[0].pageY;\n\
var clientX=e.clientX||e.targetTouches[0].pageX;\n\
var y=(clientY-rect.top)/rect.height;\n\
var x=(clientX-rect.left)/rect.width;\n\
var posYfloat=(y*100-joystickSize/2);\n\
var posXfloat=(x*100-joystickSize/2);\n\
var posY=posYfloat+\"%\";\n\
var posX=posXfloat+\"%\";\n\
posYfloat=(posYfloat/100)-0.5;\n\
posXfloat=(posXfloat/100)-0.5;\n\
leftPower=-posXfloat/2+posYfloat;\n\
rightPower=posXfloat/2+posYfloat;\n\
joystick.getElementsByClassName(\"stick\")[0].style.top=posY;\n\
joystick.getElementsByClassName(\"stick\")[0].style.left=posX;\n\
if(testSocket!=null&&testSocket.readyState==WebSocket.OPEN)\n\
{\n\
int8View[lookuptable[name+\"x\"]]=Math.round(Math.min(1,Math.max(0,rightPower+0.5))*254);\n\
int8View[lookuptable[name+\"y\"]]=Math.round(Math.min(1,Math.max(0,leftPower+0.5))*254);\n\
testSocket.send(data);\n\
}\n\
}\n\
}\n\
function unsetJoystick(name)\n\
{\n\
console.log(\"UNSETTING \"+name);\n\
var slider=document.getElementById(name);\n\
slider.onmousemove=null;\n\
slider.ontouchmove=null;\n\
slider.getElementsByClassName(\"stick\")[0].style.top=joystickMidpoint;\n\
slider.getElementsByClassName(\"stick\")[0].style.left=joystickMidpoint;\n\
if(testSocket!=null&&testSocket.readyState==WebSocket.OPEN)\n\
{\n\
int8View[lookuptable[name+\"x\"]]=128;\n\
int8View[lookuptable[name+\"y\"]]=128;\n\
testSocket.send(data);\n\
}\n\
}\n\
if(document.getElementById)window.onload=initConnection;\n\
</script>\n\
</head>\n\
<body class=\"maxheight\">\n\
<div class=\"row maxheight\">\n\
<div class=\"column slider\" id=\"vertSlider1\" style=\"height: 100%\" onmousedown=\"setSlider('vertSlider1');\" onmouseup=\"unsetStaticSlider('vertSlider1');\" ontouchstart=\"setSlider('vertSlider1');\" ontouchend=\"unsetStaticSlider('vertSlider1');\">\n\
<div class=\"wideStick\"></div>\n\
</div>\n\
<div class=\"column joystick\" id=\"joystick1\" style=\"height: 100%\" onmousedown=\"setJoystick('joystick1');\" onmouseup=\"unsetJoystick('joystick1');\" ontouchstart=\"setJoystick('joystick1');\" ontouchend=\"unsetJoystick('joystick1');\">\n\
<div class=\"stick\"></div>\n\
</div>\n\
</div>\n\
</body>\n\
<body>\n\
</body>\n\
</html>\n\
\n";
//===========================================================
const char TANK_DRIVE_CONTROLLER_HTML[] = "<html class=\"maxheight\">\n\
<head>\n\
<title>ESP8266 Controller</title>\n\
<meta content='width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0' name='viewport' />\n\
<style>\n\
body{overscroll-behavior-y:contain;padding:0;margin:0;background-color:#cccccc;font-family:Arial,Helvetica,Sans-Serif;Color:#000088;}\n\
.row{\n\
display:flex;\n\
}\n\
.column{\n\
flex:50%;\n\
}\n\
.maxheight{\n\
height:100%;\n\
}\n\
.slider{\n\
margin:0 5%;\n\
background-color:#aaa;\n\
overflow:hidden;\n\
word-wrap:break-word;\n\
}\n\
.stick{\n\
height:10%;\n\
background-color:#777;\n\
position:relative;\n\
}\n\
</style>\n\
<script>\n\
var testSocket=null;\n\
var ledState=false;\n\
var data=new ArrayBuffer(4);\n\
var int8View=new Int8Array(data);\n\
var heartBeatData=new ArrayBuffer(1);\n\
function initConnection(){\n\
console.log('init connection complete...');\n\
testSocket=new WebSocket('ws://192.168.4.1:81');\n\
var heartbeatSender;\n\
console.log('connection attempted...');\n\
int8View[0]=0;\n\
int8View[1]=255;\n\
int8View[2]=128;\n\
int8View[3]=128;\n\
testSocket.onopen=function(event){\n\
console.log('Sending data...');\n\
testSocket.send('WE ARE IN.');\n\
heartbeatSender=setInterval(sendHeartbeat,1000);\n\
testSocket.onclose=function(event){\n\
document.getElementById('statusSpan').innerHTML=\"Disconnected\";\n\
clearInterval(heartbeatSender);\n\
}\n\
function sendHeartbeat()\n\
{\n\
if(testSocket.readyState==WebSocket.OPEN)\n\
testSocket.send(heartBeatData);\n\
}\n\
}\n\
}\n\
lookuptable={\"vertSlider1\":2,\"vertSlider2\":3};\n\
var sliderWidth=10;\n\
var midpoint=(50-sliderWidth/2)+\"%\";\n\
function setSlider(name)\n\
{\n\
console.log(\"SETTING \"+name);\n\
var slider=document.getElementById(name);\n\
slider.onmousemove=moveTracker;\n\
slider.ontouchmove=moveTracker;\n\
function moveTracker(e)\n\
{\n\
console.log(\"MOVEMENT for \"+name);\n\
var rect=slider.getBoundingClientRect();\n\
var clientY=e.clientY||e.targetTouches[0].pageY;\n\
var y=(clientY-rect.top)/rect.height;\n\
var posFloat=(y*100-sliderWidth/2);\n\
var pos=posFloat+\"%\";\n\
slider.getElementsByClassName(\"stick\")[0].style.top=pos;\n\
if(testSocket!=null&&testSocket.readyState==WebSocket.OPEN)\n\
{\n\
int8View[lookuptable[name]]=Math.round(Math.min(1,Math.max(0,posFloat/(100-sliderWidth)))*254)\n\
testSocket.send(data);\n\
}\n\
}\n\
}\n\
function unsetSlider(name)\n\
{\n\
console.log(\"UNSETTING \"+name);\n\
var slider=document.getElementById(name);\n\
slider.onmousemove=null;\n\
slider.ontouchmove=null;\n\
slider.getElementsByClassName(\"stick\")[0].style.top=midpoint;\n\
if(testSocket!=null&&testSocket.readyState==WebSocket.OPEN)\n\
{\n\
int8View[lookuptable[name]]=128;\n\
testSocket.send(data);\n\
}\n\
}\n\
if(document.getElementById)window.onload=initConnection;\n\
</script>\n\
</head>\n\
<body class=\"maxheight\">\n\
<div class=\"row maxheight\">\n\
<div class=\"column slider\" id=\"vertSlider1\" style=\"height: 100%\" onmousedown=\"setSlider('vertSlider1');\" onmouseup=\"unsetSlider('vertSlider1');\" ontouchstart=\"setSlider('vertSlider1');\" ontouchend=\"unsetSlider('vertSlider1');\">\n\
<div class=\"stick\"></div>\n\
</div>\n\
<div class=\"column slider\" id=\"vertSlider2\" style=\"height: 100%\" onmousedown=\"setSlider('vertSlider2');\" onmouseup=\"unsetSlider('vertSlider2');\" ontouchstart=\"setSlider('vertSlider2');\" ontouchend=\"unsetSlider('vertSlider2');\">\n\
<div class=\"stick\"></div>\n\
</div>\n\
</div>\n\
</body>\n\
<body>\n\
</body>\n\
</html>\n\
\n";
#endif
