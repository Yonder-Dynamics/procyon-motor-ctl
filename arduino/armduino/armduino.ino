#include <ArduinoSTL.h>
#include "Arduino.h"
#include <Servo.h>
/* 
 * Yonder Dynamics 2018
 *
 * Configures an arduino to act as an extension of a "host" controller,
 * via serial. Currently only suppots basic pinout functionality
 * 
 */
#define SERIAL                  Serial

const int NUM_PINS = NUM_DIGITAL_PINS+NUM_ANALOG_INPUTS;
int pins[NUM_PINS];

Servo servo;

void setup_pins(){
    int i;
    for(i = 0; i < NUM_PINS;i++){
        pins[i] = UNSET_OP;
    }
}

void setup(){
    SERIAL.begin(BAUDRATE);
    setup_pins();
}

bool hasPin(int pin){
    return (pins[pin]!=UNSET_OP)?true:false;
}


int getMode(int pin){
    return pins[pin];
}

void servo_attach(int pin){
    pins[pin] = SERVO_MODE;
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
    pins[pin] = mode;
    if(mode < 0){ //special modes, not mapped to arduino pin modes
        specialPinSetup(pin,mode);
    }
    pinMode(pin,mode);
}

void hang(){
    SERIAL.println("FATAL ERROR -> WIPE REQUIRED");
    while(1){
        if(SERIAL.available()){
            if(SERIAL.read() == WIPE_HEADER){
                break;
            }
        }
    }
    wipe();
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
    SERIAL.println("doing pin write");
    SERIAL.println(pin);
    SERIAL.println(value);
    switch(mode){
        case ANALOG_OP:
            SERIAL.println("ANALOG");
            analogWrite(pin,value);
            break;
        case DIGITAL_OP:
            SERIAL.println("DIGITAL");
            digitalWrite(pin,value);
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
    setup_pins();
}

void dump_pins(){
    SERIAL.print("PIN MODES (");
    SERIAL.print(NUM_PINS);
    SERIAL.println(" total): ");
    int i;
    for(i = 0; i < NUM_PINS;i++){
        if(pins[i] == UNSET_OP) continue;
        SERIAL.print(i);
        SERIAL.print(": ");
        SERIAL.println(pins[i]);
    }
}

void loop(){
    if(SERIAL.available()){
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
            default:
                break;
        }
    }
}