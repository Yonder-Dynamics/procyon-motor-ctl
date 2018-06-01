#include "EncodedJoint.h"

EncodedJoint::EncodedJoint(int ID,EncoderInfo* info):JointDriver(ID),info(info){
    this->info->ratio *= MOTOR_RATIO;
    this->goal = 0;
    this->waiting = true;
    this->activated = 0;
}

void EncodedJoint::tare(){
    this->info->cycles = 0;
}

float EncodedJoint::getAngle(){
    return (float)this->info->cycles / this->info->ratio * M_PI;
}

void EncodedJoint::setGoal(float goal){
    this->goal = goal;
}

float EncodedJoint::getGoal(){
    return this->goal;
}

char EncodedJoint::update(){
    if(this->waiting){
        if(millis() - this->activated > MOTOR_STARTUP_WAIT){
            waiting = false;
            this->info->cycles = 0;
        }
    }
    float residual = this->goal - this->getAngle();
    residual = (abs(residual) > this->info->tolerance)?residual:0;
    move(residual);
    return 0; //TODO: return the dir
}

void EncodedJoint::move(float movement){
    bool flip = (movement>0);
    int pwm = 0;
    if(movement != 0){
        if(this->info->flip){
            flip = !flip;
        }
        pwm = this->info->speed;
    }
    int dir = (flip)?HIGH:LOW;
    this->digitalWrite(this->info->dir,dir);
    this->analogWrite(this->info->pwm,pwm);
    Serial.println(dir);
    Serial.println(pwm);
}

void EncodedJoint::activate(){
    this->pinMode(this->info->dir,OUTPUT);
    this->pinMode(this->info->pwm,OUTPUT);
    make_encoder_interrupt(info->a,this->info->interrupt);
    this->activated = millis();
}

void make_encoder_interrupt(int pin,void (*fn)(void)){
    attachInterrupt(digitalPinToInterrupt(pin),fn,RISING);
}