#pragma once

#define C_TUNING ControllerTuning

typedef struct{
    float kp;
    float ki;
    float kd;
    float z;
} ControllerTuning;

#define C_CONSTRAINTS ControllerConstraints

typedef struct{
    int lower;
    int upper;
    unsigned long interval;
} ControllerConstraints;

class Controller{ //Damped PID controller for motor control signals
    public:
        Controller(C_TUNING* tuning, C_CONSTRAINTS* constraints);
        void reset();
        int step(float sv);
    private:
        C_TUNING* tuning;
        C_CONSTRAINTS* constraints;
};