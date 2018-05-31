#include <ArduinoSTL.h>
#include "Common.h"
#include <map>
/* 
 * Yonder Dynamics 2018
 *
 * 
 */
#define SERIAL                  Serial

#define LA_MD_1_DIR     A0
#define LA_MD_1_PWM     9

#define LA_MD_2_DIR     A1
#define LA_MD_2_PWM     10

#define HAND_MD_1_DIR   11
#define HAND_MD_1_PWM   3

#define HAND_MD_2_DIR   12
#define HAND_MD_2_PWM   5

#define HAND_MD_3_DIR   13
#define HAND_MD_3_PWM   6

const int NUM_PINS = 20;
int pins[NUM_PINS];

void setup(){
    SERIAL.begin(BAUDRATE);
    int i;
    for(i = 0; i < NUM_PINS;i++){
        pins[i] = -1;
    }
}

bool hasPin(int pin){
    return (pins[pin]!=-1)?true:false;
}


int getMode(int pin){
    return pins[pin];
}

void setPinMode(int pin,int mode){
    pins[pin] = mode;
    pinMode(pin,mode);
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
    // SERIAL.println("doing pin read");
    int value = 0;
    switch(mode){
        case ANALOG_OP:
            value = analogRead(pin);
            break;
        case DIGITAL_OP:
            value = digitalRead(pin);
            break;
        default:
            break;
    }
}

void doPinWrite(int pin,int value,int mode){
    // SERIAL.println("doing pin write");
    switch(mode){
        case ANALOG_OP:
            // SERIAL.println("ANALOG");
            analogWrite(pin,value);
            break;
        case DIGITAL_OP:
            // SERIAL.println("DIGITAL");
            digitalWrite(pin,value);
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
    read = SERIAL.readBytes((char*)(&pin),sizeof(int));
    tot += read;
    if(read < sizeof(int)){
        return tot;
    }

    if(!hasPin(pin)){
        return tot;
    }

    int value;
    read = SERIAL.readBytes((char*)(&value),sizeof(int));
    tot += read;
    if(read < sizeof(int)){
        return tot;
    }

    if(getMode(pin) != OUTPUT){
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
            default:
                break;
        }
    }
}

// const int NUM_JOINTS = 6;

// int pwms[NUM_JOINTS] = {0};
// int speeds[NUM_JOINTS] = {0};
// int dirs[NUM_JOINTS] = {0};

// const int SIG_FIGS = 3;
// const int SIG_EXP = pow(10,SIG_FIGS);
// void printFloat(float f){
//   ARM_SERIAL.print((int)f);
//   ARM_SERIAL.print('.');
//   ARM_SERIAL.print(abs(((int)(f * SIG_EXP))%SIG_EXP));
// }

// unsigned long PRINT_PER = 1000;
// unsigned long RUN_PER   = 100;
// unsigned long running = 0;

// const int BUFFER_LEN = 10;
// char buffer[BUFFER_LEN];

// void setup(){
//     ARM_SERIAL.begin(BAUDRATE);

//     pwms[BASE_JOINT_ID] = 255;
//     pwms[ELBOW_JOINT_ID] = 255;

//     pinMode(LA_MD_2_DIR,OUTPUT);
//     pinMode(LA_MD_2_PWM,OUTPUT);

//     delay(1000);
// }


// void loop(){
//     if(ARM_SERIAL.available()){
//         int read = ARM_SERIAL.readBytesUntil('\n',buffer,BUFFER_LEN);
//         if(!read){
//             return;
//         }
//         buffer[read] = 0;
//         int dir = atoi(buffer);
//         if(!dir){
//             return;
//         }
//         Serial.print("Running motor: ");
//         Serial.println(dir);
//         digitalWrite(LA_MD_2_DIR,(dir==-1)?LOW:HIGH);
//         analogWrite(LA_MD_2_PWM,255);
//         running = millis();
//     }
//     if(millis() - running > RUN_PER){
//         analogWrite(LA_MD_2_PWM,0);
//     } else{
//         Serial.println("Motor is running");
//     }
//     // if(ARM_SERIAL.available()){
//     //     int ID = ARM_SERIAL.read();
//     //     if(ID == -1){
//     //         return;
//     //     }
//     //     int dir = ARM_SERIAL.read();
//     //     if(dir == -1){
//     //         return;
//     //     }
//     //     analogWrite(dirs[ID],(char)dir);
//     //     analogWrite(pwms[ID],speeds[ID]);
//     // }
// }