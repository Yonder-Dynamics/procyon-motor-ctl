#include "Proxy.h"

void proxyInit(int pin,int mode){
    ProxyInitSerialized p = { PINMODE_RAW_HEADER, pin, mode};
    Serial1.write((char*)&p,sizeof(ProxyInitSerialized));
}

void servoInit(int pin){
    ProxyServoInitSerialized p = { SERVO_HEADER, pin};
    Serial1.write((char*)&p,sizeof(ProxyServoInitSerialized));
}

void proxyWrite(int pin, int value, int mode){
    ProxyWriteSerialized s = { PINOP_RAW_HEADER, pin, value, mode };
    Serial1.write((char*)&s,sizeof(ProxyWriteSerialized));
}

void proxyDigitalWrite(int pin, int value){
    proxyWrite(pin,value,DIGITAL_OP);
}

void proxyAnalogWrite(int pin, int value){
    proxyWrite(pin,value,ANALOG_OP);
}

// void proxyServoWrite(int pin, int value){
//     ProxyWriteSerialized s = { SERVO_HEADER, pin, value, mode };
//     Serial1.write((char*)&s,sizeof(ProxyWriteSerialized));
// }