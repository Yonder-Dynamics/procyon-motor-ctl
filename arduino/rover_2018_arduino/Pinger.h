#pragma once
#include "Common.h"

#define distance_t unsigned long //centimeters
#define PINGER_SCALE 10000
#define PINGER_MAGIC 0.034*PINGER_SCALE

class Pinger{
    public:
        Pinger(int trigPin,int echoPin);
        distance_t readDistance();
    private:
        int trig, echo;
};
