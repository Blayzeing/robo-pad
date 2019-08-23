/*
--------------------------------------------------------------------
Author: Blayze F Millward

This is the header file for BasicHBridgeMotor.cpp.
---------------------------------------------------------------------
*/
#include "GlobalRobopadConfig.h"// Include the robopad config info for PWMRANGE

#ifndef BASICHBRIDGEMOTOR_H // Guard.
#define BASICHBRIDGEMOTOR_H

#define FULLRANGE PWMRANGE*2

class BasicHBridgeMotor {
  private:
    int bPin, fPin;
    
  public:
    BasicHBridgeMotor(int backwardPin, int forwardPin);
    void drive(int value); // Drives the motor with a range between 0 and PWMRANGE*2, from full reverse to full forward
    void drive(long value, long lowerBound, long upperBound); // Drives the motor, but uses map() to map the input from between lowerbound and upper bound to between 0 and PWMRANGE*2 (The longs make me cringe)
    void stop(); // Stops the motor
};

#endif
