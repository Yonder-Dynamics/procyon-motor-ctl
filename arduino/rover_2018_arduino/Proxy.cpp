#include "Proxy.h"

void proxyInit(int pin,int mode){
    ProxyInitSerialized p = { PINMODE_RAW_HEADER, pin, mode};
    PROXY_SERIAL.write((char*)&p,sizeof(ProxyInitSerialized));
}

void servoInit(int pin){
    proxyInit(pin,SERVO_MODE);
}

void proxyWrite(int pin, int value, int mode){
    // Serial.println("uncomment move stuff fool");
    ProxyWriteSerialized s = { PINOP_RAW_HEADER, pin, value, mode };
    PROXY_SERIAL.write((char*)&s,sizeof(ProxyWriteSerialized));
}

void proxyDigitalWrite(int pin, int value){
    proxyWrite(pin,value,DIGITAL_OP);
}

void proxyAnalogWrite(int pin, int value){
    proxyWrite(pin,value,ANALOG_OP);
}

void proxyReset(){
    PROXY_SERIAL.write('!');
}

void proxyCull(int timeout){
    ProxyCullSerialized s = { CULL_RAW_HEADER, timeout };
    PROXY_SERIAL.write((char*)&s,sizeof(ProxyCullSerialized));
}
