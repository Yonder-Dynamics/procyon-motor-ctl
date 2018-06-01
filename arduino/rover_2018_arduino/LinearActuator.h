#pragma once
#include <Arduino.h>
#include "Pinger.h"

#define LAD LinearActuatorDriver

typedef struct{
    int dir;
    int pwm;
    int trig;
    int echo;
    distance_t tol;
    distance_t offset;
} LinearActuatorDriverInfo;

#define LAD_INFO LinearActuatorDriverInfo

class LinearActuatorDriver{
    public:
        LinearActuatorDriver(LAD_INFO* info);
        void setGoal(distance_t goal);
        distance_t getExtension();
        distance_t getRawExtension();
        char update();
        void move(int dir,int speed);
        void setDigitalOut(void (*fn)(int,int));
        void setAnalogOut(void (*fn)(int,int));
        void setPinMode(void (*fn)(int,int));
    private:
        bool evaluate();
        void pinSetup();

        int pwm, dir; //actuator control pins
        distance_t tolerance;
        distance_t extension;
        distance_t goal;
        distance_t offset;
        void (*digitalWrite)(int,int);
        void (*analogWrite)(int,int);
        void (*pinMode)(int,int);
        bool moving;
        int speed;
        int rolling_index;
        int window_size;
        distance_t* rolling_avg;
        Pinger* pinger;
};
