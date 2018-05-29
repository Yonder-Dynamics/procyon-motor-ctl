#pragma once

#include "common.h"
#include "ActuatedJoint.h"

/*
typedef struct{
    // supplied info
    float left_mount_aligned;
    float left_mount_perp;
    float right_mount_aligned;
    float right_mount_perp;
    float offset;

    // calculated info
    float left_hyp;
    float left_angle;
    float right_hyp;
    float right_angle;
} ActuatorMount;
*/

ActuatorMount base_mount = {
    .left_mount_aligned=    13.55*DISTANCE_SCALE,
    .left_mount_perp=       3.68*DISTANCE_SCALE,
    .right_mount_aligned=   7.7*DISTANCE_SCALE,
    .right_mount_perp=      4.445*DISTANCE_SCALE,
    .flipped=true
};

LAD_INFO base_info = {
    .dir=       0,
    .pwm=       0,
    .trig=      47,
    .echo=      46,
    .tol=       0*DISTANCE_SCALE,
    .offset=    13*DISTANCE_SCALE
};

//base length 35.56cm
//seg1 length 24.9cm

ActuatorMount elbow_mount = {
    .left_mount_aligned=    21.59*DISTANCE_SCALE,
    .left_mount_perp=       -5.08*DISTANCE_SCALE,
    .right_mount_aligned=   9.9*DISTANCE_SCALE,
    .right_mount_perp=      3.903*DISTANCE_SCALE, 
    .flipped=false
};

LAD_INFO elbow_info = {
    .dir=       0,
    .pwm=       0,
    .trig=      49,
    .echo=      48,
    .tol=       0*DISTANCE_SCALE,
    .offset=    18.5*DISTANCE_SCALE
};

ActuatorMount wrist_mount = {
    .left_mount_aligned=    0*DISTANCE_SCALE,
    .left_mount_perp=       0*DISTANCE_SCALE,
    .right_mount_aligned=   0*DISTANCE_SCALE,
    .right_mount_perp=      0*DISTANCE_SCALE,
    .flipped=false
};

LAD_INFO wrist_info = {
    .dir=       0,
    .pwm=       0,
    .trig=      0,
    .echo=      0,
    .tol=       0*DISTANCE_SCALE,
    .offset=    0*DISTANCE_SCALE
};
