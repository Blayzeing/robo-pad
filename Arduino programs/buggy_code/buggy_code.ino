#include <Servo.h>
#define neut 90

Servo pan, tilt;

//pins for motor H-bridge connections. 
const int L1 = 8;
const int L2 = 9;
const int R1 = 10;
const int R2 = 11;
int i=0;

//pins for ultrasonic sensor.
const int trigPin = 3;
const int echoPin = 2;

//ultrasonic variables.
long duration, cm;
int distance;

//motor variables.
int lPow, rPow, center = 90, turn;

//servo variables
int pos = 90, inc = 2;
  
void setup() {
  // put your setup code here, to run once:
  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
  pinMode(R1,OUTPUT);
  pinMode(R2, OUTPUT);
  
  pan.attach(5);
  tilt.attach(6);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
  
}

void loop() {
  // put your main code here, to run repeatedly:

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = duration*0.034/2;
  cm = (duration/2) / 29.1;     // Divide duration by 2 (distance there and back) and divide by 29.1 to obtain distance in cm.

  //Rotate the servo. Increment inc variable each loop iteration. 
  pan.write(pos);
  tilt.write(90);
  pos+= inc;
  if((pos>neut +45) || (pos<neut -45)){
    inc = inc*-1;
  }

  turn = pos;
  center = neut;

  //Rotate the motors in accordance to what the position and neut variables are. 
  //go straight
  if((distance>10) && (turn>center-30) && (turn<center+30)){
    lPow = 200;
    rPow = 200;
  //turn left?
  }else if((distance>10) && (turn>center) && (turn<center+60)){
    lPow=200;
    rPow = 100;
  //turn right?
  }else if((distance>10) && (turn>center-60) && (turn<center)){
    lPow=100;
    rPow = 200;
  //reverse
  }else if(distance<10){
    Stop();
    delay(50);
    lPow = -250;
    rPow = -250;
  }

  //write to the wheels
  WheelSpeed(lPow, rPow);

  Serial.print("Distance: "); Serial.print(distance); Serial.print(" cm.   ");
  Serial.print("pos: "); Serial.print(pos); Serial.print(" neut: "); Serial.print(neut); Serial.print("  ");
  Serial.print(lPow); Serial.print("\t"); Serial.println(rPow);

  delay(10);

}

//take the motor powers and convert them to H-bridge values. 
void WheelSpeed(int lPower, int rPower){
  int l1pow, l2pow, r1pow, r2pow;
  
  if(lPower>0){
    l1pow = abs(lPower);
    l2pow = 0;
  }else if(lPower<0){
    l1pow = 0;
    l2pow = abs(lPower);
  }
  if(rPower>0){
    r1pow = abs(rPower);
    r2pow = 0;
  }else if(rPower<0){
    r1pow = 0;
    r2pow = abs(rPower);
  }
    analogWrite(L1, l1pow);
    analogWrite(L2, l2pow);
    analogWrite(R1, r1pow);
    analogWrite(R2, r2pow);
}

//break
void Stop(){
  analogWrite(L1, 0);
  analogWrite(L2, 0);
  analogWrite(R1, 0);
  analogWrite(R2, 0);
}
