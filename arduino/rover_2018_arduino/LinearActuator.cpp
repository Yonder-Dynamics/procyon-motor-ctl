#include "LinearActuator.h"

LAD::LinearActuatorDriver(LAD_INFO* info):
    dir(info->dir),pwm(info->pwm),tolerance(info->tol),offset(info->offset){
    this->pinSetup();
    this->pinger = new Pinger(info->trig,info->echo);
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
    // Serial.println("Offset: ");
    // Serial.println(this->offset);
    this->extension = this->pinger->readDistance() + this->offset * PINGER_SCALE;
    return this->extension;
}

void LAD::setGoal(distance_t goal){
    this->goal = goal;
}

bool LAD::evaluate(){
    distance_t diff = this->getExtension() - this->goal;
    return (abs(diff) > this->tolerance);
}

char LAD::update(){
    char dir = 0;
    if(this->evaluate()){
        if(this->extension - this->goal > 0){
            dir = 1;
        } else{
            dir = -1;
        }
    }
    return dir;
    // this->move(dir,this->speed);
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

