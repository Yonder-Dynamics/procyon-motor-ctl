#pragma once
#include "Arduino.h"
#include "Common.h"

typedef struct{
    char header;
    int pin;
    int mode;
} ProxyInitSerialized;

typedef struct{
    char header;
    int pin;
    int mode;
} ProxyReadSerialized;

typedef struct{
    char header;
    int pin;
    int value;
    int mode;
} ProxyWriteSerialized;

typedef struct{
    char header;
    int pin;
}ProxyServoInitSerialized;

void proxyReset();

void proxyInit(int pin, int mode);
void servoInit(int pin);

void proxyWrite(int pin,int value,int mode);
void proxyAnalogWrite(int pin,int value);
void proxyDigitalWrite(int pin,int value);
void proxyServoWrite(int pin, int value);