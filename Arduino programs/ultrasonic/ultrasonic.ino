//https://randomnerdtutorials.com/complete-guide-for-ultrasonic-sensor-hc-sr04/

#include <Servo.h>

const int redLED = 3;
const int greenLED = 5;
const int trig = 9;
const int echo = 10;
const int servo1Pin = 6;

int brightness, angle1;
long duration, cm;
Servo servo1;

void setup() {
  // put your setup code here, to run once:
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  servo1.attach(servo1Pin);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trig, LOW);
  delayMicroseconds(5);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echo, INPUT);
  duration = pulseIn(echo, HIGH);
 
  // Convert the time into a distance
  cm = (duration/2) / 29.1;     // Divide duration by 2 (distance there and back) and divide by 29.1 to obtain distance in cm.
  if(cm > 400){
     cm = 400;
  }
  
  Serial.print(cm);
  Serial.print("cm");
  //Serial.print(duration);
  Serial.println();

  //dim the LED in comparison to the ultrasonic reading. 
  brightness = map(cm, 0, 400, 0, 255);
  analogWrite(redLED, 255-brightness);
  analogWrite(greenLED, brightness);

  //move the servo arm for haptic feedback. Responds to values below 20cm. 
  if(cm > 20){
    angle1 = 20;
  }else{
    angle1 = cm;
  }
  angle1 = map(angle1, 0, 20, 70, 110);
  servo1.write(angle1);
  
  delay(250);
}
