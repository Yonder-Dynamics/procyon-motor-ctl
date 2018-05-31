#pragma once

const unsigned long BAUDRATE = 19200;

typedef struct{
    int ID;
    float goal;
    float angle;
}JointData;

#define MSG_HEADER_DELIM    '#'
#define MSG_CONTENTS_DELIM  ','
#define DRIVE_MSG_HEADER    0
#define ARM_MSG_HEADER      1

#define BASE_JOINT_ID   0
#define ELBOW_JOINT_ID  1
#define WRIST_JOINT_ID  2

#define DISTANCE_SCALE  1 //from cm

enum errorCode {
    ROV_OK,
    ROV_HEADING_OUT_OF_RANGE,
    ROV_DUTY_OUT_OF_RANGE,
};

void printFloat(float f);

const int JOINT_DATA_SIZE = sizeof(JointData);