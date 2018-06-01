#include "JointDriver.h"

void pinModeDummy(int pin,int mode){
    pinMode(pin,mode);
}

void analogWriteDummy(int pin,int value){
    analogWrite(pin,value);
}

void digitalWriteDummy(int pin,int value){
    digitalWrite(pin,value);
}

JointDriver::JointDriver(int ID):ID(ID){
    this->digitalWrite = digitalWriteDummy;
    this->analogWrite = analogWriteDummy;
    this->pinMode = pinModeDummy;
};

void JointDriver::setDigitalOut(void (*fn)(int,int)){
    this->digitalWrite = fn;
}
void JointDriver::setAnalogOut(void (*fn)(int,int)){
    this->analogWrite = fn;
}
void JointDriver::setPinMode(void (*fn)(int,int)){
    this->pinMode = fn;
}