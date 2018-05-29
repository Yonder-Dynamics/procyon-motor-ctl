#include "Common.h"
/* 
 * Yonder Dynamics 2018
 *
 * 
 */

#define ARM_SERIAL  Serial

#define BASE_ROT_ID     0
#define BASE_JOINT_ID   1
#define ELBOW_JOINT_ID  2
#define WRIST_JOINT_ID  3
#define WRIST_ROT_ID    4
#define HAND_ID         5


const int NUM_JOINTS = 6;

int pwms[NUM_JOINTS] = {0};
int speeds[NUM_JOINTS] = {0};
int dirs[NUM_JOINTS] = {0};

const int SIG_FIGS = 3;
const int SIG_EXP = pow(10,SIG_FIGS);
void printFloat(float f){
  ARM_SERIAL.print((int)f);
  ARM_SERIAL.print('.');
  ARM_SERIAL.print(abs(((int)(f * SIG_EXP))%SIG_EXP));
}

void setup(){
    ARM_SERIAL.begin(BAUDRATE);
}

unsigned long PRINT_PER = 1000;

void loop(){
    if(ARM_SERIAL.available()){
        int ID = ARM_SERIAL.read();
        if(ID == -1){
            return;
        }
        int dir = ARM_SERIAL.read();
        if(dir == -1){
            return;
        }
        analogWrite(dirs[ID],(char)dir);
        analogWrite(pwms[ID],speeds[ID]);
    }
}