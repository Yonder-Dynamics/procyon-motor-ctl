#pragma once

#include <vector>
#include "Arduino.h"
#include "defines.h"
#include "motor.h"
#include "common.h"

#define VERSION 2018

class ROV {
public:
    ROV() {};
    ~ROV() {};

    int setSpeeds(float turningRadius, float turningSpeed, float forwardVel);
    int setDuties(std::vector<int> duties);
    int setup(void);
 protected:
    std::vector<MotorDriver> drivetrain; // assuming all wheels in train move the same
};
