#pragma once

#include "Common.h"

#define ROV_HEADING_MAX 90
#define ROV_HEADING_MIN -90
#define ROV_DUTY_MAX 255
#define ROV_DUTY_MIN -255

class Wheel {
public:

    Wheel(int pwm, int dir, float x, float y)
      : vel(0), coef(1), flipped(false), pin_pwm(pwm), pin_dir(dir), x(x), y(y) {
      analogWrite(pin_pwm, 0);
    };

    float vel;
    float coef;
    void setFlipped(void) { flipped = true; };

    virtual void updateDir(void) {};
    virtual void updateDuty(void) {};

protected:
    boolean flipped;
    int pin_pwm;
    int pin_dir;
    float x, y; // position relative to center in meters

};

// Mainly right side
class LittleMotor : public Wheel {
public:
    LittleMotor(int dir, int en, int pwm) : Wheel(pwm, dir), pin_en(en) {
      digitalWrite(pin_en, HIGH);
    };

    int pin_en;

    void updateDir(void) {
      int value;
      // Set Direction
      // CW == Forward
      if (vel >= 0) {

        if (!flipped) value = HIGH;
        else value = LOW;

      } else if (vel < 0) {

        if (!flipped) value = LOW;
        else value = HIGH;

      }

      if (value == HIGH) {
        if (!flipped) Serial.print(" ^ ");
        else Serial.print(" v ");
      } else {
        if (!flipped) Serial.print(" v ");
        else Serial.print(" ^ ");
      }
      digitalWrite(pin_dir,value);
    };

    void updateDuty(void) {

      float prod = vel*coef;
      Serial.print(prod);
      Serial.print(" " );
      analogWrite(pin_pwm, abs(prod));
    };
};

// Mainly left side
class BigMotor : public Wheel {
public:
    BigMotor(int dir, int freewheel, int rstop, int pwm) : Wheel(pwm, dir), pin_freewheel(freewheel), pin_runstop(rstop) {
      digitalWrite(pin_freewheel, HIGH);
      digitalWrite(pin_runstop, LOW);
    };

    int pin_freewheel;
    int pin_runstop;


    void updateDir(void) {

      int value;
      // Set Direction
      // CW == Backward
      if (vel >= 0) {

        if (!flipped) value = LOW;
        else value = HIGH;

      } else if (vel < 0) {

        if (!flipped) value = HIGH;
        else value = LOW;

      }

      if (value == LOW) {
        if (!flipped) Serial.print(" ^ ");
        else Serial.print(" v ");
      } else {
        if (!flipped) Serial.print(" v ");
        else Serial.print(" ^ ");
      }
      digitalWrite(pin_dir,value);
    };

    void updateDuty(void) {

      float prod = vel*coef;
      Serial.print(prod);
      Serial.print(" " );
      analogWrite(pin_pwm, abs(prod));
    };

};
