#pragma once
#include <Arduino.h>
#include "Pinger.h"

#define LAD LinearActuatorDriver

class LinearActuatorDriver{
    public:
        LinearActuatorDriver(distance_t tol,int dirPin,int pwmPin,int trigPin,int echoPin);
        void setGoal(distance_t goal);
        distance_t getExtension();
        void update();
    private:
        bool evaluate();
        void pinSetup();

        int pwm, dir; //actuator control pins
        distance_t tolerance;
        distance_t extension;
        distance_t goal;
        bool moving;
        int speed;
        Pinger* pinger;
};
