#pragma once

const unsigned long BAUDRATE = 19200;

typedef struct{
    int ID;
    float goal;
    float angle;
}JointData;

enum errorCode {
    ROV_OK,
    ROV_HEADING_OUT_OF_RANGE,
    ROV_DUTY_OUT_OF_RANGE,
};

const int JOINT_DATA_SIZE = sizeof(JointData);