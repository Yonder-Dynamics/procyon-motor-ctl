#include <math.h>
#include "motor.h"

void Motor::setSpeed(float turningRadius, float turningSpeed, float maxTurningSpeed, float forwardVel) {
  // The math checks out. We did the trig.
  setDuty(turningSpeed*((turningRadius-x)/maxTurningSpeed) + forwardVel);
}

float Motor::getTurningSpeed(float turningRadius, float turningSpeed) {
  return turningSpeed*(turningRadius-x);
}

void MotorDriver::setDir(bool newDir) {
  if (newDir != dir) {
    dir = newDir;
    digitalWrite(dirPin, newDir);
  }
}

void MotorDriver::setDuty(float speed) {
  if (isFlipped) {
    speed = -speed;
  }
  setDir(speed > 0);
  
  //Serial.print("Motor: ");
  //Serial.print(ID);
  //Serial.print(" speed: ");
  //Serial.println(speed);
  analogWrite(pwmPin, abs((int)speed));
}
