#pragma once
#include <cmath>
#include "LinearActuator.h"
#include "JointDriver.h"
#include "common.h"

#define mount_t ActuatorMount
#define mount_ptr mount_t*

typedef struct{
    // supplied info
    float left_mount_aligned;
    float left_mount_perp;
    float right_mount_aligned;
    float right_mount_perp;
    bool flipped;

    // calculated info
    float left_hyp;
    float left_angle;
    float right_hyp;
    float right_angle;
} ActuatorMount;

float cosines(float a, float b, float c);
float inv_cosines(float A, float b, float c);


class ActuatedJoint:public JointDriver{
    public:
        ActuatedJoint(LAD* lad, mount_ptr mount);
        float getAngle();
        void setGoal(float goal);
        float getGoal();
        char update();
        void move(float movement);
    private:
        void calcMountInfo();
        float calcGoalExt(float goal_angle);

        float goal;
        float angle;
        bool flipped;
        mount_ptr mountInfo;
        LAD* actuator;
};
