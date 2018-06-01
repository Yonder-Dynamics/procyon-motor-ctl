#pragma once

#include <ArduinoSTL.h>
#include <cmath>
#include <Arduino.h>
#include "Common.h"
#include "JointDriver.h"
#include "defines.h"

struct EncoderInfo{
    int dir;
    int pwm;
    int a;
    int b;
    float tolerance;
    long ratio;
    long cycles;
    void (*interrupt)();
    bool flip;
    int speed;
};

void make_encoder_interrupt(int pin,void(*fn)(void));

class EncodedJoint:public JointDriver{
    public:
        EncodedJoint(int ID,EncoderInfo* info);
        float getAngle();
        void setGoal(float goal);
        float getGoal();
        char update();
        void move(float movement);
        void tare();
        virtual void activate();
    private:
        EncoderInfo* info;
        float angle;
        float goal;
        bool waiting;
        unsigned long activated;
};