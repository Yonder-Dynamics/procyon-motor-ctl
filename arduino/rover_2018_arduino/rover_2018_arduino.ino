#include "Arduino.h"
#include <ArduinoSTL.h>

/* 
 * Yonder Dynamics 2018
 *
 * 
 */
//#include <StandardCplusplus.h>
#include <vector>
#include <cmath>
#include "Common.h"
#include "rov.h"
#include "RoverArmSpec.h"
#include "JointDriver.h"
#include "Pinger.h"
#include "Proxy.h"

#define MAX_SPEED 255

#define ARM_SERIAL  Serial1
#define PI_SERIAL   Serial

#define DRILL_DIR         51
#define DRILL_PWM         2
#define DRILL_ACT_DIR     13
#define DRILL_ACT_PWM     6
#define DRILL_SERVO_POS   11

//wrist: dir=0 >> up
//hand: dir=0  >> clockwise
//base: pwm: limit to analogWrite(51)

ROV rover;

unsigned long AUTO_KILL_TIME = 500;
unsigned long prev_message_time = 0;

const int BUFFER_LEN = 64;
char buffer[BUFFER_LEN];
char delim = ',';

//const int num_joint_data_vals = 2;
const int NUM_JOINTS = 4;
// JointData joint_data[NUM_JOINTS+1];
// JointData* joint_buffer = joint_data+NUM_JOINTS;

const int NUM_ACTUATORS = 2;

float goals[NUM_JOINTS+1];

int joint_map[NUM_JOINTS];

JointData joint_data;
JointData* joint_buffer = &joint_data;

JointDriver** drivers;

bool killed;

const int SIG_FIGS = 3;
const int SIG_EXP = pow(10,SIG_FIGS);
void printFloat(float f){
  PI_SERIAL.print((int)f);
  PI_SERIAL.print('.');
  PI_SERIAL.print(abs(((int)(f * SIG_EXP))%SIG_EXP));
}

void kill(){
  killed = !killed;
  PI_SERIAL.println("#KILL#");
  for(int i = 0; i < NUM_ACTUATORS; i++){
    drivers[i]->kill();
  }
  rover.kill();
}

void make_linear_actuated(int ID,ActuatedJoint** drivers,ActuatorMount* mount,LAD_INFO* info){
    *(drivers+ID) = new ActuatedJoint(ID,new LinearActuatorDriver(info),mount);
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

void report_joint_data(JointDriver* joint){
  PI_SERIAL.print("#ARM#");
  PI_SERIAL.print(joint->getID());
  PI_SERIAL.print(",");
  printFloat(joint->getGoal());
  PI_SERIAL.print(",");
  printFloat(joint->getAngle());
  PI_SERIAL.print("\n");
}

// void set_joint_goal(int ID,float goal){
//   JointData* joint = joint_data+ID;
//   joint->goal = goal;
//   report_joint_data(joint);
//   ARM_SERIAL.write((char*)joint,JOINT_DATA_SIZE);
// }

int getDelim(char* msg,char delim){
  int i;
  for(i = 0; msg[i] != 0; i++ ){
    if(msg[i] == delim){
      break;
    }
  }
  return i;
}

int parseInt(char** msg,char delim,int* dest){
  int field_end = getDelim(*msg,delim);
  if(field_end){
    *(*msg+field_end) = 0;
    *dest = atoi(*msg);
    field_end += 1;
  }
  *msg += field_end;
  return field_end;
}

int parseFloat(char** msg,char delim,float* dest){
  int field_end = getDelim(*msg,delim);
  if(field_end){
    *(*msg+field_end) = 0;
    *dest = atof(*msg);
    field_end += 1;
  }
  *msg += field_end;
  return field_end;
}

int parseArmCommand(char* input){
  int i;
  char* start = input;
  int   joint_id;
  float joint_angle;
  for(i=0;i<NUM_JOINTS;i++){
    if(!parseInt(&input,MSG_CONTENTS_DELIM,&joint_id)){
      return input-start;
    }
    if(!parseFloat(&input,MSG_CONTENTS_DELIM,&joint_angle)){
      return input - start;
    }
    drivers[joint_id]->setGoal(joint_angle);
    // set_joint_goal(joint_id,joint_angle);
  }
  return input - start;
}

int parseDriveCommand(char* input){
  int duty_vals = 6;
  std::vector<int> duties;
  int   duty_val;
  int i;
  char* start = input;
  PI_SERIAL.print("#DRIVE#");
  for(i=0;i<duty_vals;i++){
    if(!parseInt(&input,MSG_CONTENTS_DELIM,&duty_val)){
      return input - start;
    }
    duties.push_back(duty_val);
    PI_SERIAL.print((int)duties[i]);
    PI_SERIAL.print(", ");
  }
  PI_SERIAL.println();
  rover.setDuties(duties);
  return input - start;
}

int parseDrillCommand(char* input){
  int i;
  char* start = input;
  //drill speed, actuator speed, actuator dir, servo position
  int drill_dir,drill_speed, act_speed, act_dir, servo_pos;
  // if(!parseInt(&input,MSG_CONTENTS_DELIM,&act_dir))     return start - input;
  if(!parseInt(&input,MSG_CONTENTS_DELIM,&act_speed))   return start - input;
  // if(!parseInt(&input,MSG_CONTENTS_DELIM,&drill_dir))   return start - input;
  if(!parseInt(&input,MSG_CONTENTS_DELIM,&drill_speed)) return start - input;
  if(!parseInt(&input,MSG_CONTENTS_DELIM,&servo_pos))   return start - input;

  drill_dir = (drill_speed>0)?HIGH:LOW;
  drill_speed = abs(drill_speed);
  act_dir = (act_speed>0)?HIGH:LOW;
  act_speed = abs(act_speed);

  Serial.println(drill_dir);
  Serial.println(drill_speed);
  Serial.println(act_dir);
  Serial.println(act_speed);
  Serial.println(servo_pos);

  digitalWrite(DRILL_DIR,drill_dir);
  analogWrite(DRILL_PWM,drill_speed);
  proxyAnalogWrite(DRILL_ACT_PWM,act_speed);
  proxyAnalogWrite(DRILL_SERVO_POS,servo_pos);
  proxyDigitalWrite(DRILL_ACT_DIR,act_dir);
  return start - input;
}

int parseCommand(char* input) {
  char* start = input;
  int i = 0;

  while(input[i] != 0 && input[i] != '#') i++;
  if(!input[i]) return 0;
  input += ++i;
  int ID;
  if(!parseInt(&input,MSG_HEADER_DELIM,&ID)){
    return i;
  }
  int read;
  switch(ID){
    case ARM_MSG_HEADER:
      return i + parseArmCommand(input);
    case DRIVE_MSG_HEADER:
      return i + parseDriveCommand(input);
    case DRILL_MSG_HEADER:
      return i + parseDrillCommand(input);
    case KILL_HEADER:
    default:
      kill();
      return i;  
  }
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
      Serial.print(duty);
      Serial.print(",");
      duties.push_back(duty);
      j = i + 1;
      arg += 1;
      if(arg == 6){
        break;
      }
    }
  }
  Serial.println();
  rover.setDuties(duties);
  return i;
}

// void setup_joints(){
//   int i;
//   JointData* joint;
//   for(i=0;i<NUM_JOINTS;i++){
//     joint = joint_data + i;
//     joint->ID     = i;
//     joint->goal   = 0;
//     joint->angle  = 0;
//   }
// }

Pinger* pinger;

void setup() {
  joint_map[BASE_JOINT] = BASE_JOINT_ID;
  joint_map[ELBOW_JOINT] = ELBOW_JOINT_ID;
  joint_map[WRIST_JOINT] = WRIST_JOINT_ID;
  PI_SERIAL.begin(BAUDRATE);
  ARM_SERIAL.begin(BAUDRATE);

  //pinger = new Pinger(47,46);
  killed = true;

  //drivers = calloc(NUM_ACTUATORS,sizeof(JointDriver*));

  // make_linear_actuated(BASE_JOINT_ID,drivers,&base_mount,&base_info);
  make_linear_actuated(ELBOW_JOINT_ID,drivers,&elbow_mount,&elbow_info);

  proxyReset();
  // pinMode(DRILL_PWM, OUTPUT);
  // pinMode(DRILL_DIR, OUTPUT);
  // proxyInit(DRILL_ACT_DIR, OUTPUT);
  // proxyInit(DRILL_ACT_PWM, OUTPUT);
  // servoInit(DRILL_SERVO_POS);
  
  if (rover.setup() == ROV_OK) {
    Serial.println("#SETUP# Completed.");
  } else {
    PI_SERIAL.println("#SETUP# Failed. Hanging indefinitely...");
    while(1);
  }

  // setup_joints();
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
    int parsed = parseCommand(buffer); //what if there's more?
    prev_message_time = millis();
  }

  if (millis()-prev_message_time > AUTO_KILL_TIME) {
    kill();
  }

  int i;
  for(i = 0; i < NUM_ACTUATORS; i++){
    if(!drivers[i]) continue;
    drivers[i]->update();
    report_joint_data(drivers[i]);
  }

  // while(ARM_SERIAL.available()){
  //   int read = ARM_SERIAL.readBytes(buffer,BUFFER_LEN);
  //   buffer[read] = 0;
  //   PI_SERIAL.println(buffer);
  // }

  // request_joint_goals();


}
