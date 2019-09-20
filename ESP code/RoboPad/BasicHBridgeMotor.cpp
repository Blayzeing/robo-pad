#include "Arduino.h"
/*
--------------------------------------------------------------------
Author: Blayze F Millward

A basic H-bridge motor wrapper class that drives a two-pin variable-
voltage motor H-bridge.
Uses PWMRANGE to find the range which to use to generate PWM signals.
---------------------------------------------------------------------
*/

#include "BasicHBridgeMotor.h"

BasicHBridgeMotor::BasicHBridgeMotor(int backwardPin, int forwardPin)
{
  bPin = backwardPin;
  fPin = forwardPin;
  pinMode(fPin, OUTPUT);
  pinMode(bPin, OUTPUT);
}

/**
 * Takes in a range between 0 and PWMRANGE*2, and drives motor from fully backward to fully forward.
 */
void BasicHBridgeMotor::drive(int value)
{
  value = value - PWMRANGE;
  if(value >= 0)
  {
    analogWrite(fPin, value);
    analogWrite(bPin, 0);
  }else{
    analogWrite(fPin, 0);
    analogWrite(bPin, abs(value));
  }
}

/**
 * Takes in a range between lowerBound and upperBound, and drives motor from fully backward to fully forward.
 */
void BasicHBridgeMotor::drive(long value, long lowerBound, long upperBound)
{
  drive(map(value, lowerBound, upperBound, 0, PWMRANGE*2));
}

/**
 * Stops the motor.
 */
void BasicHBridgeMotor::stop()
{
  analogWrite(fPin, 0);
  analogWrite(bPin, 0);
}
