#pragma once

#define distance_t unsigned long //centimeters
#define PINGER_MAGIC 0.034

class Pinger{
    public:
        Pinger(int trigPin,int echoPin);
        distance_t readDistance();
    private:
        int trig, echo;
};