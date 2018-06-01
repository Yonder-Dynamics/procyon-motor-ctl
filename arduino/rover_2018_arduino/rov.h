#pragma once

#include <vector>
#include "Arduino.h"
#include "defines.h"
#include "motor.h"
#include "Common.h"

#define VERSION 2018

class ROV {
public:
    ROV() : killed(false) {};
    ~ROV() {};

    int setSpeeds(float turningRadius, float turningSpeed, float forwardVel);
    int setDuties(std::vector<int> duties);
    int setup(void);
    void kill();
    void unkill();
 protected:
    std::vector<MotorDriver> drivetrain; // assuming all wheels in train move the same
    bool killed;
};
