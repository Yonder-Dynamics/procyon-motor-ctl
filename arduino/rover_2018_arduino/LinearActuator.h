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
        char update();
        void move(int dir,int speed);
    private:
        bool evaluate();
        void pinSetup();

        int pwm, dir; //actuator control pins
        distance_t tolerance;
        distance_t extension;
        distance_t goal;
        distance_t offset;
        bool moving;
        int speed;
        Pinger* pinger;
};
