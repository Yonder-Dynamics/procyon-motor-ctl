#include "Common.h"

#define ARM_SERIAL Serial2

JointData joint_data;
const JointData* joint_buffer = &joint_data;

const int BUFFER_LEN = 64;
char buffer[BUFFER_LEN];

const int SIG_FIGS = 3;
const int SIG_EXP = pow(10,SIG_FIGS);
void printFloat(float f){
  Serial.print((int)f);
  Serial.print('.');
  Serial.print(((int)(f * SIG_EXP))%SIG_EXP);
}

void report_joint_data(JointData* joint){
  Serial.print(joint->ID);
  Serial.print(",");
  printFloat(joint->goal);
  Serial.print(",");
  printFloat(joint->angle);
  Serial.print("\n");
}

void setup(){
  Serial.begin(19200);
  joint_data = {
      3,0.5,0.6
  };
}

void loop(){
    // while(Serial.available()){
    //     int read = Serial.readBytes((char*)joint_buffer,JOINT_DATA_SIZE);
    //     if(read < JOINT_DATA_SIZE){ //incomplete, toss it out
    //         break;
    //     }
    //     report_joint_data(joint_buffer);
    // }
    delay(1000);
    while(ARM_SERIAL.available()){
        Serial.println("got arm data:");
        int read = ARM_SERIAL.readBytes(buffer,BUFFER_LEN);
        Serial.print(read);
        Serial.println(" bytes");
        buffer[read] = 0;
        Serial.println(buffer);
    }
    Serial.println("writing arm serial");
    ARM_SERIAL.write((char*)(joint_buffer),JOINT_DATA_SIZE);

}
