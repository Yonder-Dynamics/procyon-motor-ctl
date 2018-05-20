#include <ArduinoSTL.h>

/* 
 * Yonder Dynamics 2018
 *
 * 
 */
//#include <StandardCplusplus.h>
#include "LinearActuator.h"
#include "ActuatedJoint.h"
#include "Common.h"

#include <vector>

#define MAX_SPEED 255

#define ARM_SERIAL  Serial

const int NUM_ACTUATORS = 2;

// const int BUFFER_LEN = 64;
// char buffer[BUFFER_LEN];

float goals[NUM_ACTUATORS+1];

JointData joint_data;
JointData* joint_buffer = &joint_data;

ActuatedJoint** drivers;

const int SIG_FIGS = 3;
const int SIG_EXP = pow(10,SIG_FIGS);
void printFloat(float f){
  ARM_SERIAL.print((int)f);
  ARM_SERIAL.print('.');
  ARM_SERIAL.print(((int)(f * SIG_EXP))%SIG_EXP);
}

void make_driver(ActuatedJoint** driver,distance_t tol,distance_t offset,int dir,int pwm,int trig,int echo,
                    float left_aligned, float left_perp, float right_aligned, float right_perp){
    mount_ptr mount = malloc(sizeof(mount_t));
    *mount = {
        left_aligned,left_perp,
        right_aligned,right_perp,
        offset,
        0,0,0,0
    };
    *driver = new ActuatedJoint(new LinearActuatorDriver(tol,dir,pwm,trig,echo),mount);
}

void report_joint_data(JointData* joint){
  ARM_SERIAL.print(joint->ID);
  ARM_SERIAL.print(",");
  printFloat(joint->goal);
  ARM_SERIAL.print(",");
  printFloat(joint->angle);
  ARM_SERIAL.print("\n");
}

void setup(){
    ARM_SERIAL.begin(BAUDRATE);
    drivers = calloc(NUM_ACTUATORS,sizeof(ActuatedJoint*));
}

void loop(){
    while(ARM_SERIAL.available()){
        int read = ARM_SERIAL.readBytes((char*)joint_buffer, JOINT_DATA_SIZE);
        if(read < JOINT_DATA_SIZE){
            return;
        }
        int ID = joint_buffer->ID;
        if(ID < NUM_ACTUATORS && drivers[ID]){
            drivers[ID]->setGoal(joint_buffer->goal);
        }
    }
    int i;
    for(i = 0; i < NUM_ACTUATORS; i++){
        if(!drivers[i]){
            continue;
        }
        joint_buffer->ID = i;
        joint_buffer->goal = drivers[i]->getGoal();
        joint_buffer->angle = drivers[i]->getAngle();
        ARM_SERIAL.write((char*)joint_buffer,JOINT_DATA_SIZE);
    }  
}