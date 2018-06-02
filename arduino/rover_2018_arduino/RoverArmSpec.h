#pragma once

#include <Arduino.h>
#include "Common.h"
#include "defines.h"
#include "ActuatedJoint.h"
#include "EncodedJoint.h"

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

/*
    int dir;
    int pwm;
    int a;
    int b;
} EncoderInfo;
*/

#define make_cycler(INFO_NAME) \
void INFO_NAME ## _cycle(){ \
  int b = digitalRead(INFO_NAME.b);              \
  if(!b){                                   \
    INFO_NAME.cycles++;                          \
  } else {                                  \
    INFO_NAME.cycles--;                          \
  }                                         \
  INFO_NAME.cycles = INFO_NAME.cycles % INFO_NAME.ratio;   \
}                                          
//end make_cycler


EncoderInfo base_rot_enc = {
    .dir = ARM_BASE_ROT_DIR,
    .pwm = ARM_BASE_ROT_PWM,
    .a   = ARM_BASE_ENC_A,
    .b   = ARM_BASE_ENC_B,
    .tolerance = 0.005,
    .ratio = ARM_BASE_RATIO,
    .cycles = 0,
    .interrupt = 0,
    .flip = false,
    .speed = ARM_BASE_ROT_SPEED
};

make_cycler(base_rot_enc);

EncoderInfo wrist_enc = {
    .dir = ARM_WRIST_DIR,
    .pwm = ARM_WRIST_PWM,
    .a   = ARM_WRIST_ENC_A,
    .b   = ARM_WRIST_ENC_B,
    .tolerance = 0.005, //radians
    .ratio = ARM_WRIST_RATIO,
    .cycles = 0,
    .interrupt = 0,
    .flip = false,
    .speed = 255
};

make_cycler(wrist_enc);

EncoderInfo twist_enc = {
    .dir = ARM_TWIST_DIR,
    .pwm = ARM_TWIST_PWM,
    .a   = ARM_TWIST_ENC_A,
    .b   = ARM_TWIST_ENC_B,
    .tolerance = 0.005,
    .ratio = ARM_TWIST_RATIO,
    .cycles = 0,
    .interrupt = 0,
    .flip = false,
    .speed = 127
};

make_cycler(twist_enc);


EncoderInfo hand_enc = {
    .dir = HAND_DIR,
    .pwm = HAND_PWM,
    .a   = HAND_ENC_A,
    .b   = HAND_ENC_B,
    .tolerance = 0.005,
    .ratio = HAND_ENC_RATIO,
    .cycles = 0,
    .interrupt = 0,
    .flip = false,
    .speed = 255
};
make_cycler(hand_enc);


ActuatorMount base_mount = {
    .left_mount_aligned=    13.55*DISTANCE_SCALE,
    .left_mount_perp=       3.68*DISTANCE_SCALE,
    .right_mount_aligned=   7.7*DISTANCE_SCALE,
    .right_mount_perp=      4.445*DISTANCE_SCALE,
    .flipped=true
};

LAD_INFO base_info = {
    .dir=       ARM_BASE_JOINT_DIR,
    .pwm=       ARM_BASE_JOINT_PWM,
    .trig=      ARM_BASE_JOINT_TRIG,
    .echo=      ARM_BASE_JOINT_ECHO,
    .tol=       1*DISTANCE_SCALE,
    .offset=    13*DISTANCE_SCALE,
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
    .dir=       ARM_ELBOW_DIR,
    .pwm=       ARM_ELBOW_PWM,
    .trig=      ARM_ELBOW_TRIG,
    .echo=      ARM_ELBOW_ECHO,
    .tol=       1*DISTANCE_SCALE,
    .offset=    18.5*DISTANCE_SCALE
};
