#include "LinearActuator.h"

LAD::LinearActuatorDriver(LAD_INFO* info):
    dir(info->dir),pwm(info->pwm),tolerance(info->tol),offset(info->offset){
    this->pinSetup();
    this->pinger = new Pinger(info->trig,info->echo);
    this->extension = this->getRawExtension();
    this->goal = this->extension;
    this->speed = 255;
    this->rolling_index = 0;
    this->window_size = 10;

    this->rolling_avg = (distance_t*)malloc(sizeof(distance_t)*this->window_size);
}

void LAD::pinSetup(){
    this->pinMode(this->pwm,OUTPUT);
    this->pinMode(this->dir,OUTPUT);
}

void LAD::setDigitalOut(void (*fn)(int,int)){
    this->digitalWrite = fn;
}

void LAD::setAnalogOut(void (*fn)(int,int)){
    this->analogWrite = fn;
}

void LAD::setPinMode(void (*fn)(int,int)){
    this->pinMode = fn;
}

distance_t LAD::getRawExtension(){
    return this->pinger->readDistance() + this->offset * PINGER_SCALE;
}

distance_t LAD::getExtension(){
    distance_t ext =  this->getRawExtension();
    this->rolling_avg[this->rolling_index] = ext;
    this->rolling_index = (this->rolling_index + 1 ) % this->window_size;
    int i;
    distance_t sum = 0;
    for(i = 0; i < this->window_size; i++){
        sum += this->rolling_avg[i] / this->window_size;
    }
    this->extension = sum;
    return this->extension;
}

void LAD::setGoal(distance_t goal){
    this->goal = goal * PINGER_SCALE;
}

bool LAD::evaluate(){
    distance_t diff = this->getExtension() - this->goal;
    return (abs(diff) > this->tolerance*PINGER_SCALE);
}

char LAD::update(){
    char dir = 0;
    // Serial.print(this->extension);
    // Serial.print(" -> ");
    // Serial.print(this->goal);
    // Serial.print(" : ");
    // Serial.print(this->extension-this->goal);
    // Serial.print(" (");
    // Serial.print(this->tolerance*PINGER_SCALE);
    // Serial.println(")");

    if(this->evaluate()){
        if((long)this->extension - (long)this->goal > 0){
            dir = 1;
        } else{
            dir = -1;
        }
    }else{
        // Serial.println("Done");
    }
    this->move(dir,this->speed);
    return dir;
}

void LAD::move(int dir,int speed){
    if(dir > 0){
        this->digitalWrite(this->dir,HIGH);
        this->analogWrite(this->pwm,speed); 
    } else if(dir < 0){
        this->digitalWrite(this->dir,LOW);
        this->analogWrite(this->pwm,speed); 
    } else{
        this->moving = false;
        this->analogWrite(this->pwm,0);
    }
}

