#include <ArduinoSTL.h>
#include "Arduino.h"
#include <Servo.h>
#include "Common.h"
/* 
 * Yonder Dynamics 2018
 *
 * Configures an arduino to act as an extension of a "host" controller,
 * via serial. Currently only suppots basic pinout functionality
 * 
 */
#define SERIAL                  Serial

typedef struct{
    int op;
    int mode;
    unsigned long timestamp;
} PinInfo;

const int NUM_PINS = NUM_DIGITAL_PINS+NUM_ANALOG_INPUTS;
PinInfo pins[NUM_PINS];

Servo servo;

void setup_pins(){
    int i;
    for(i = 0; i < NUM_PINS;i++){
        pins[i].mode = UNSET_MODE;
    }
}

void setup(){
    SERIAL.begin(BAUDRATE);
    setup_pins();
}

bool hasPin(int pin){
    return (pins[pin].mode!=UNSET_MODE)?true:false;
}


int getMode(int pin){
    return pins[pin].mode;
}

void servo_attach(int pin){
    pins[pin].mode = SERVO_MODE;
    servo.attach(pin);
}

void specialPinSetup(int pin,int mode){
    switch(mode){
        case SERVO_MODE:
            servo_attach(pin);
            break;
        default:
            break;
    }
}

void setPinMode(int pin,int mode){
    pins[pin].mode = mode;
    if(mode < 0){ //special modes, not mapped to arduino pin modes
        specialPinSetup(pin,mode);
    }
    pinMode(pin,mode);
    pins[pin].mode = mode;
}

void hang(){
    SERIAL.println("ERROR");
}

int read_raw_pinmode(){
    int pin;
    int read = SERIAL.readBytes((char*)(&pin),sizeof(int));
    if(read < sizeof(int)){
        return read;
    }
    int mode;
    read += SERIAL.readBytes((char*)(&mode),sizeof(int));
    if(read < sizeof(int)*2){
        return read;
    }
    setPinMode(pin,mode);
    return read;
}

int read_ascii_pinmode(){
    int bufsize = 8;
    char buf[bufsize];
    int tot = 0;

    int read = SERIAL.readBytesUntil(PINMODE_ASCII_DELIM,buf,bufsize);
    tot += read;
    if(!read) return tot;
    buf[read] = 0;
    int pin = atoi(buf);

    read = SERIAL.readBytesUntil(PINMODE_ASCII_DELIM,buf,bufsize);
    tot += read;
    if(!read) return tot;
    buf[read] = 0;
    int mode = atoi(buf);
    setPinMode(pin,mode);
    // SERIAL.print("Set pin ");
    // SERIAL.print(pin);
    // SERIAL.print(" to mode ");
    // SERIAL.println(mode);
    return tot;
}

void doPinRead(int pin,int mode){
    SERIAL.println("doing pin read");
    SERIAL.println(pin);
    int value = 0;
    switch(mode){
        case ANALOG_OP:
            value = analogRead(pin);
            SERIAL.println(value);
            break;
        case DIGITAL_OP:
            value = digitalRead(pin);
            SERIAL.println(value);
            break;
        default:
            break;
    }
}

void doPinWrite(int pin,int value,int mode){
    // SERIAL.println("doing pin write");
    // SERIAL.println(pin);
    // SERIAL.println(value);

    pins[pin].timestamp = millis();
    switch(mode){
        case ANALOG_OP:
            // SERIAL.println("ANALOG");
            analogWrite(pin,value);
            pins[pin].op = ANALOG_OP;
            break;
        case DIGITAL_OP:
            // SERIAL.println("DIGITAL");
            digitalWrite(pin,value);
            pins[pin].op = DIGITAL_OP;
            break;
        case -2:
            servo.write(value);
            break;
        default:
            SERIAL.println(mode);
            break;
    }
}

int pin_op_ascii(){
    int bufsize = 8;
    char buf[bufsize];
    int tot = 0;

    int read = SERIAL.readBytesUntil(PINMODE_ASCII_DELIM,buf,bufsize);
    tot += read;
    if(!read) return 0;
    buf[read] = 0;
    int pin = atoi(buf);

    if(!hasPin(pin)){
        SERIAL.print("CRITICAL ERROR: Unset pin: ");
        SERIAL.println(pin);
        hang();        
        return tot;
    }

    read = SERIAL.readBytesUntil(PINMODE_ASCII_DELIM,buf,bufsize);
    tot += read;
    if(!read) return tot;
    buf[read] = 0;
    int value = atoi(buf);

    if(getMode(pin) != OUTPUT){
        doPinRead(pin,value);
        return tot;
    }

    read = SERIAL.readBytesUntil(PINMODE_ASCII_DELIM,buf,bufsize);
    tot += read;
    if(!read) return tot;
    buf[read] = 0;
    int mode = atoi(buf);

    doPinWrite(pin,value,mode);
    return tot;
}

int pin_op_raw(){
    int bufsize = 8;
    char buf[bufsize];
    int tot = 0;

    int pin;
    int read = SERIAL.readBytes((char*)(&pin),sizeof(int));
    tot += read;
    if(read < sizeof(int)){
        return tot;
    }

    if(!hasPin(pin)){
        SERIAL.print("CRITICAL ERROR: Unset pin: ");
        SERIAL.println(pin);
        hang();
        return tot;
    }

    int value;
    read = SERIAL.readBytes((char*)(&value),sizeof(int));
    tot += read;
    if(read < sizeof(int)){
        return tot;
    }

    if(getMode(pin) == INPUT){
        doPinRead(pin,value);
        return tot;
    }

    int mode;
    read = SERIAL.readBytes((char*)(&mode),sizeof(int));
    tot += read;
    if(read < sizeof(int)){
        return tot;
    }

    doPinWrite(pin,value,mode);
    return tot;
}

void wipe(){
    int i;
    for(i = 0; i < NUM_PINS;i++){
        if(pins[i].mode){
            digitalWrite(i,0);
        }
    }
    setup_pins();
}

void cull(int max_age){
    // SERIAL.print("CULLING from ");
    // SERIAL.println(max_age);

    unsigned long curr_time = millis();
    int i;
    for(i = 0; i < NUM_PINS; i++){
        if(pins[i].mode == OUTPUT && pins[i].op == ANALOG_OP){
            if(curr_time-pins[i].timestamp > max_age){
                // Serial.println(i);
                digitalWrite(i,0);
            }
        }
    }
}

void start_cull_raw(){
    int max_age;
    int read = SERIAL.readBytes((char*)(&max_age),sizeof(int));
    if(read < sizeof(int)){
        return read;
    }
    cull(max_age);
}

void start_cull_ascii(){
    int bufsize = 8;
    char buf[bufsize];
    int tot = 0;

    int max_age;
    int read = SERIAL.readBytes(buf,bufsize);
    tot += read;
    buf[read] = 0;
    if(!read){
        return tot;
    }
    max_age = atoi(buf);
    cull(max_age);
}

void dump_pins(){
    SERIAL.print("PIN MODES (");
    SERIAL.print(NUM_PINS);
    SERIAL.print(" total) @ ");
    SERIAL.print(millis());
    SERIAL.println(": ");
    int i;
    for(i = 0; i < NUM_PINS;i++){
        if(pins[i].mode == UNSET_MODE) continue;
        SERIAL.print(i);
        SERIAL.print(": ");
        SERIAL.print(pins[i].mode);
        SERIAL.print(" : ");
        SERIAL.print(pins[i].op);
        SERIAL.print(" : ");
        SERIAL.println(pins[i].timestamp);
    }
}

void process_header(){
    char first = SERIAL.read();
    switch(first){
        case PINMODE_RAW_HEADER:
            read_raw_pinmode();
            break;
        case PINMODE_ASCII_HEADER:
            read_ascii_pinmode();
            break;
        case PINOP_ASCII_HEADER:
            pin_op_ascii();
            break;
        case PINOP_RAW_HEADER:
            pin_op_raw();
            break;
        case WIPE_HEADER:
            wipe();
            break;
        case DUMP_HEADER:
            dump_pins();
            break;
        case CULL_ASCII_HEADER:
            start_cull_ascii();
            break;
        case CULL_RAW_HEADER:
            start_cull_raw();
            break;
        default:
            break;
    }
}

void loop(){
    if(SERIAL.available()){
        process_header();
    }
}