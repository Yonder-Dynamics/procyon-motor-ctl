#include <Arduino.h>
#include "Pinger.h"

Pinger::Pinger(int trigPin,int echoPin):trig(trigPin),echo(echoPin){
    pinMode(trig,OUTPUT);
    pinMode(echo,INPUT);

    // Serial.println(trig);
    // Serial.println(echo);
}

distance_t Pinger::readDistance(){
    // Clears the trigPin
    digitalWrite(this->trig, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(this->trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(this->trig, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    distance_t duration = pulseIn(this->echo, HIGH);
    distance_t distance= duration*PINGER_MAGIC/2;

    // Serial.println(distance);

    return distance;
}