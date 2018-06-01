#include "EncodedJoint.h"

EncodedJoint::EncodedJoint(int ID,EncoderInfo* info):JointDriver(ID),info(info){
    make_encoder_interrupt(info->a,this->info->interrupt);
    this->info->ratio *= MOTOR_RATIO;
    this->cycles = 0;
    this->goal = 0;
}

void EncodedJoint::tare(){
    this->info->cycles = 0;
}

float EncodedJoint::getAngle(){
    return this->info->cycles / this->info->ratio * M_PI;
}

void EncodedJoint::setGoal(float goal){
    this->goal = goal;
}

float EncodedJoint::getGoal(){
    return this->goal;
}

char EncodedJoint::update(){
    float residual = this->goal - this->getAngle();
    residual = (abs(residual) > this->info->tolerance)?residual:0;
}

void EncodedJoint::move(float movement){
    int dir = (movement>0)?HIGH:LOW;
    int pwm = abs(movement);
    this->digitalWrite(this->info->dir,dir);
    this->analogWrite(this->info->pwm,pwm);
}

void make_encoder_interrupt(int pin,void (*fn)(void)){
    attachInterrupt(digitalPinToInterrupt(pin),fn,RISING);
}