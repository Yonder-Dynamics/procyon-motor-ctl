#pragma once

#include "Arduino.h"
#include "Common.h"

class Motor {
 public:
 Motor(int ID,float x, float y, float maxSpeed) : ID(ID),x(x), y(y), maxSpeed(maxSpeed) {}
  void setSpeed(float turningRadius, float turningSpeed, float maxTurningSpeed, float forwardVel);
  float getTurningSpeed(float turningRadius, float turningSpeed);
 protected:
  float x, y, maxSpeed;
  int ID;
  virtual void setDuty(float speed);
};

class MotorDriver : public Motor {
 public:
 MotorDriver(int ID,float x, float y, float maxSpeed, bool isFlipped,
             int pwmPin, int freewheelPin, int rstopPin, int dirPin)
   : Motor(ID,x, y, maxSpeed), pwmPin(pwmPin), isFlipped(isFlipped),
     dirPin(dirPin),rstopPin(rstopPin),freewheelPin(freewheelPin),dir(isFlipped) {
    pinMode(pwmPin, OUTPUT);
    pinMode(rstopPin, OUTPUT);
    pinMode(freewheelPin, OUTPUT);
    pinMode(dirPin, OUTPUT);
    digitalWrite(rstopPin, 0);
    digitalWrite(freewheelPin, 1);
    digitalWrite(dirPin, isFlipped);
    analogWrite(pwmPin, 0);
  }
  void setDuty(float speed);
  void setDir(bool newDir);
 protected:
  int pwmPin, dirPin, freewheelPin, rstopPin;
  bool isFlipped, dir; // true is forward, false is back
};
