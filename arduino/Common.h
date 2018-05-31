#pragma once

const unsigned long BAUDRATE = 57600;

typedef struct{
    int ID;
    float goal;
    float angle;
}JointData;

#define PINMODE_RAW_HEADER      '#'
#define PINMODE_ASCII_HEADER    '@'
#define PINMODE_ASCII_DELIM     ','
#define PINOP_RAW_HEADER        '*'
#define PINOP_ASCII_HEADER      '$'
#define WIPE_HEADER             '!'
#define SERVO_HEADER            '%'
#define DUMP_HEADER             '?'

#define UNSET_OP                 -1
#define DIGITAL_OP               0
#define ANALOG_OP                1
#define SERVO_OP                 2

#define MSG_HEADER_DELIM    '#'
#define MSG_CONTENTS_DELIM  ','
#define KILL_HEADER         -1
#define DRIVE_MSG_HEADER    0
#define ARM_MSG_HEADER      1
#define DRILL_MSG_HEADER    2

#define BASE_JOINT   0
#define ELBOW_JOINT  1
#define WRIST_JOINT  2

#define BASE_ROT_ID     0
#define BASE_JOINT_ID   1
#define ELBOW_JOINT_ID  2
#define WRIST_JOINT_ID  3
#define WRIST_ROT_ID    4
#define HAND_ID         5

#define NANO_A0     14
#define NANO_A1     15
#define NANO_A2     16
#define NANO_A3     17
#define NANO_A4     18
#define NANO_A5     19
#define NANO_A6     20
#define NANO_A7     21

#define DISTANCE_SCALE  1 //from cm

// #define DEBUG

#ifdef DEBUG
#define pinMode(x,y) \
do{                                 \
    pinMode(x,y);                   \
    Serial.println("Pin mode: ");   \
    Serial.print(x);                \
    Serial.print(' ');              \
    Serial.println(y);              \
} while(0);
#endif

enum errorCode {
    ROV_OK,
    ROV_HEADING_OUT_OF_RANGE,
    ROV_DUTY_OUT_OF_RANGE,
};

void printFloat(float f);

const int JOINT_DATA_SIZE = sizeof(JointData);