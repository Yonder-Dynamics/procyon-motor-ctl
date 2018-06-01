#pragma once
// MOTOR DEFINES

// LEFT TRAIN
// top left
#define ROV_TL_PIN_DIR       37
#define ROV_TL_PIN_FREEWHEEL 38
#define ROV_TL_PIN_RSTOP     39
#define ROV_TL_PIN_PWM       6

// mid left
#define ROV_ML_PIN_DIR       31
#define ROV_ML_PIN_FREEWHEEL 32
#define ROV_ML_PIN_RSTOP     33
#define ROV_ML_PIN_PWM       7

// back left
#define ROV_BL_PIN_DIR       25
#define ROV_BL_PIN_FREEWHEEL 26
#define ROV_BL_PIN_RSTOP     27
#define ROV_BL_PIN_PWM       8

// top right
#define ROV_TR_PIN_DIR       34
#define ROV_TR_PIN_FREEWHEEL 35
#define ROV_TR_PIN_RSTOP     36
#define ROV_TR_PIN_PWM       9

// mid right
#define ROV_MR_PIN_DIR       28
#define ROV_MR_PIN_FREEWHEEL 29
#define ROV_MR_PIN_RSTOP     30
#define ROV_MR_PIN_PWM       10

// back right
#define ROV_BR_PIN_DIR       22
#define ROV_BR_PIN_FREEWHEEL 23
#define ROV_BR_PIN_RSTOP     24
#define ROV_BR_PIN_PWM       11


//ARM
#define MOTOR_RATIO             4320

#define ARM_BASE_ROT_DIR        0
#define ARM_BASE_ROT_PWM        0
#define ARM_BASE_ENC_A          0
#define ARM_BASE_ENC_B          0
#define ARM_BASE_RATIO          1


#define ARM_BASE_JOINT_DIR      0
#define ARM_BASE_JOINT_PWM      0
#define ARM_BASE_JOINT_ECHO     0
#define ARM_BASE_JOINT_TRIG     0

#define ARM_ELBOW_DIR           0
#define ARM_ELBOW_PWM           0
#define ARM_ELBOW_ECHO          0
#define ARM_ELBOW_TRIG          0

#define ARM_WRIST_DIR           0
#define ARM_WRIST_PWM           0
#define ARM_WRIST_ENC_A         0
#define ARM_WRIST_ENC_B         0
#define ARM_WRIST_RATIO         0

#define ARM_TWIST_DIR           0
#define ARM_TWIST_PWM           0
#define ARM_TWIST_ENC_A         0
#define ARM_TWIST_ENC_B         0
#define ARM_TWIST_RATIO         3

#define HAND_DIR                0
#define HAND_PWM                0
#define HAND_ENC_A              0
#define HAND_ENC_B              0
#define HAND_ENC_RATIO          1