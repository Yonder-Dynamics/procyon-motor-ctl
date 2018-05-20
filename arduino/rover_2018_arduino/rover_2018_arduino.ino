#include <ArduinoSTL.h>

/* 
 * Yonder Dynamics 2018
 *
 * 
 */
//#include <StandardCplusplus.h>
#include <vector>
#include "Arduino.h"
#include "rov.h"
#include "Common.h"

#define MAX_SPEED 255

#define ARM_SERIAL  Serial1
#define PI_SERIAL   Serial

ROV rover;

char val; 

const int BUFFER_LEN = 64;
char buffer[BUFFER_LEN];
char delim = ',';

const int num_joint_data_vals = 2;
const int NUM_JOINTS = 4;
JointData joint_data[NUM_JOINTS+1];
JointData* joint_buffer = joint_data+NUM_JOINTS;


const int SIG_FIGS = 3;
const int SIG_EXP = pow(10,SIG_FIGS);
void printFloat(float f){
  PI_SERIAL.print((int)f);
  PI_SERIAL.print('.');
  PI_SERIAL.print(((int)(f * SIG_EXP))%SIG_EXP);
}

int parsePhoneCommand(char input) {
  std::vector<int> duties;
  switch(input) {
  case '0':
    duties.push_back(255);
    duties.push_back(255);
    duties.push_back(255);
    duties.push_back(255);
    duties.push_back(255);
    duties.push_back(255);
    break;
  case '1':
    duties.push_back(-255);
    duties.push_back(-255);
    duties.push_back(-255);
    duties.push_back(255);
    duties.push_back(255);
    duties.push_back(255);
    break;
  case '2':
    duties.push_back(-255);
    duties.push_back(-255);
    duties.push_back(-255);
    duties.push_back(-255);
    duties.push_back(-255);
    duties.push_back(-255);
    break;
  case '3':
    duties.push_back(255);
    duties.push_back(255);
    duties.push_back(255);
    duties.push_back(-255);
    duties.push_back(-255);
    duties.push_back(-255);
    break;
  default:
    duties.push_back(0);
    duties.push_back(0);
    duties.push_back(0);
    duties.push_back(0);
    duties.push_back(0);
    duties.push_back(0);
    break;
  }
  rover.setDuties(duties);
  return 1;
}

void report_joint_data(JointData* joint){
  PI_SERIAL.print(joint->ID);
  PI_SERIAL.print(",");
  printFloat(joint->goal);
  PI_SERIAL.print(",");
  printFloat(joint->angle);
  PI_SERIAL.print("\n");
}

void set_joint_goal(int ID,float goal){
  JointData* joint = joint_data+ID;
  joint->goal = goal;
  // report_joint_data(joint);
  ARM_SERIAL.write((char*)joint,JOINT_DATA_SIZE);
}

int getComma(char* msg){
  int i;
  for(i = 0; msg[i] != 0; i++ ){
    if(msg[i] == ','){
      break;
    }
  }
  return i;
}

int parseInt(char** msg,int* dest){
  int comma = getComma(*msg);
  if(comma){
    *(*msg+comma) = 0;
    *dest = atoi(*msg);
    comma += 1;
  }
  *msg += comma;
  return comma;
}

int parseFloat(char** msg,float* dest){
  int comma = getComma(*msg);
  if(comma){
    *(*msg+comma) = 0;
    *dest = atof(*msg);
    comma += 1;
  }
  *msg += comma;
  return comma;
}

int parseCommand(char* input) {
  char* start = input;
  int i = 0;

  int duty_vals = 6;
  std::vector<int> duties;
  int   duty_val;
  int   joint_ids[NUM_JOINTS];
  float joint_angles[NUM_JOINTS];

  for(i=0;i<duty_vals;i++){
    if(!parseInt(&input,&duty_val)){
      return input - start;
    }
    duties.push_back(duty_val);
    PI_SERIAL.print((int)duties[i]);
    PI_SERIAL.print(", ");
  }
  PI_SERIAL.println();

  rover.setDuties(duties);

  for(i=0;i<NUM_JOINTS;i++){
    if(!parseInt(&input,joint_ids+i)){
      return input-start;
    }
    if(!parseFloat(&input,joint_angles+i)){
      break;
    }
    set_joint_goal(joint_ids[i],joint_angles[i]);
  }
  PI_SERIAL.println();

  return input - start;
}

int parseMotorDuties(char* input) {
  int i = 0;
  int j = 0;
  int arg = 0;
  std::vector<int> duties;
  for (; input[i] != 0; i++) {
    if (input[i] == delim) {
      input[i] = 0;
      int duty = atoi(input+j);
      //Serial.print(duty);
      //Serial.print(",");
      duties.push_back(duty);
      j = i + 1;
      arg += 1;
      if(arg == 6){
        break;
      }
    }
  }
  //Serial.println();
  rover.setDuties(duties);
  return i;
}

void setup_joints(){
  int i;
  JointData* joint;
  for(i=0;i<NUM_JOINTS;i++){
    joint = joint_data + i;
    joint->ID     = i;
    joint->goal   = 0;
    joint->angle  = 0;
  }
}

void setup() {
  PI_SERIAL.begin(BAUDRATE);
  ARM_SERIAL.begin(BAUDRATE);
  
  if (rover.setup() == ROV_OK) {
    //Serial.println("Failed to fail.");
  } else {
    PI_SERIAL.println("Failed.");
    while(1) {}
  }

  setup_joints();
}

void loop() {
  if (PI_SERIAL.available()){
    /*
    char read = Serial.read();
    Serial.println(read);
    parsePhoneCommand(read);
    */
    int read = PI_SERIAL.readBytesUntil('\n', buffer, BUFFER_LEN);
    buffer[read] = 0; //null terminate the string just in case
    //Serial.write(buffer);
    int parsed = parseCommand(buffer); //what if there's more?
    // int parsed = parseMotorDuties(buffer); //what if there's more?

    //write goals to joints
  }
  while(ARM_SERIAL.available()){
    //read arm driver status
    int read = ARM_SERIAL.readBytes((char*)(joint_buffer),JOINT_DATA_SIZE);

    if(read < JOINT_DATA_SIZE){
      break; //toss it out
    }

    //write joint data to pi
    report_joint_data(joint_buffer);
  }

  // while(ARM_SERIAL.available()){
  //   int read = ARM_SERIAL.readBytes(buffer,BUFFER_LEN);
  //   buffer[read] = 0;
  //   PI_SERIAL.println(buffer);
  // }

  // request_joint_goals();


}
