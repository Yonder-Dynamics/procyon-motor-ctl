#include "LinearActuator.h"

LAD::LinearActuatorDriver(distance_t tol,int dirPin,int pwmPin,int trigPin,int echoPin):
    dir(dirPin),pwm(pwmPin),tolerance(tol){
    this->pinSetup();
    this->pinger = new Pinger(trigPin,echoPin);
    this->moving = false;
    this->extension = this->getExtension();
    this->goal = this->extension;
    this->speed = 255;
}

void LAD::pinSetup(){
    pinMode(this->pwm,OUTPUT);
    pinMode(this->dir,OUTPUT);
}

distance_t LAD::getExtension(){
    if(this->moving){
        this->extension = this->pinger->readDistance();
    }
    return this->extension;
}

void LAD::setGoal(distance_t goal){
    this->goal = goal;
}

bool LAD::evaluate(){
    distance_t diff = this->getExtension() - this->goal;
    return (abs(diff) > this->tolerance);
}

void LAD::update(){
    int dir = 0;
    if(this->evaluate()){
        if(this->extension - this->goal > 0){
            dir = 1;
        } else{
            dir = -1;
        }
    }
    this->move(dir,this->speed);
}

void LAD::move(int dir,int speed){
    this->moving = true;
    if(dir > 0){
        digitalWrite(this->dir,HIGH);
        analogWrite(this->pwm,speed); 
    } else if(dir < 0){
        digitalWrite(this->dir,LOW);
        analogWrite(this->pwm,speed); 
    } else{
        this->moving = false;
        analogWrite(this->pwm,0);
    }
}

