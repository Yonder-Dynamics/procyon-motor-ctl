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
#include "ActuatedJoint.h"
#include "EncodedJoint.h"
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

// typedef struct{
//   unsigned long
//   unsigned long timestamp;
//   void (*setter)(int,int);
//   int pin;
// } ManualControls;

// ManualControls manual;

unsigned long manual_signals[NUM_JOINTS] = {
  ARM_BASE_ROT_PWM,
  ARM_BASE_JOINT_PWM,
  ARM_ELBOW_PWM,
  ARM_WRIST_PWM,
  ARM_TWIST_PWM,
  HAND_PWM
};
unsigned long manual_timings[NUM_JOINTS] = {0};

ROV rover;

unsigned long AUTO_KILL_TIME = 50000000;
int PROXY_CULL_TIME = 200;
// unsigned long base_kill = 0;
unsigned long prev_message_time = 0;

const int BUFFER_LEN = 64;
char buffer[BUFFER_LEN];
char delim = ',';

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
  for(int i = 0; i < NUM_JOINTS; i++){
    drivers[i]->kill();
  }
  rover.kill();
}

void make_linear_actuated(int ID,ActuatedJoint** drivers,ActuatorMount* mount,LAD_INFO* info){
  drivers[ID] = new ActuatedJoint(ID,new LinearActuatorDriver(info),mount);
}

void make_proxied(ActuatedJoint* driver){
  driver->setPinMode(proxyInit);
  driver->setAnalogOut(proxyAnalogWrite);
  driver->setDigitalOut(proxyDigitalWrite);
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

int parseManualCommand(char* input){
  char* start = input;
  int joint = 0;
  while(joint < NUM_JOINTS){
    // if(!parseInt(&input,MSG_CONTENTS_DELIM,&joint)){
    //   return start - input;
    // }
    int dir;
    if(!parseInt(&input,MSG_CONTENTS_DELIM,&dir)){
      return start - input;
    }

    if(dir == 0){
      joint++;
      continue;
    }

    dir = (dir>0)?HIGH:LOW;

    manual_timings[joint] = millis() + PROXY_CULL_TIME;

    switch(joint){
      case BASE_ROT_ID:
        digitalWrite(ARM_BASE_ROT_DIR,dir);
        analogWrite(ARM_BASE_ROT_PWM,ARM_BASE_ROT_SPEED);
        break;
      case BASE_JOINT_ID:
        digitalWrite(ARM_BASE_JOINT_DIR,dir);
        analogWrite(ARM_BASE_JOINT_PWM,ARM_BASE_JOINT_SPEED);
        break;
      case ELBOW_JOINT_ID:
        digitalWrite(ARM_ELBOW_DIR,dir);
        analogWrite(ARM_ELBOW_PWM,ARM_ELBOW_JOINT_SPEED);
        break;
      case WRIST_JOINT_ID:
        digitalWrite(ARM_WRIST_DIR,dir);
        analogWrite(ARM_WRIST_PWM,ARM_WRIST_SPEED);
        break;
      case WRIST_ROT_ID:
        digitalWrite(ARM_TWIST_DIR,dir);
        analogWrite(ARM_TWIST_PWM,ARM_TWIST_SPEED);
        break;
      case HAND_ID:
        digitalWrite(HAND_DIR,dir);
        analogWrite(HAND_PWM,HAND_SPEED);
        break;
      default:
        PI_SERIAL.println("invalid joint ID");
        break;
    }
    joint++;
  }

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
    case MANUAL_CTRL_HEADER:
      return i + parseManualCommand(input);
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
  PI_SERIAL.begin(BAUDRATE);
  // ARM_SERIAL.begin(BAUDRATE);

  PI_SERIAL.println("#SETUP# Mega starting up");
  // proxyReset();

  killed = true;

  base_rot_enc.interrupt  = base_rot_enc_cycle;
  wrist_enc.interrupt     = wrist_enc_cycle;
  twist_enc.interrupt     = twist_enc_cycle;
  hand_enc.interrupt      = hand_enc_cycle;

  drivers = calloc(NUM_JOINTS,sizeof(JointDriver*));

  make_linear_actuated(BASE_JOINT_ID,drivers,&base_mount,&base_info);
  // make_proxied(drivers[BASE_JOINT_ID]);
  make_linear_actuated(ELBOW_JOINT_ID,drivers,&elbow_mount,&elbow_info);
  // make_proxied(drivers[ELBOW_JOINT_ID]);

  drivers[BASE_ROT_ID] = new EncodedJoint(BASE_ROT_ID,&base_rot_enc);
  drivers[BASE_ROT_ID]->activate();

  drivers[WRIST_JOINT_ID] = new EncodedJoint(WRIST_JOINT_ID,&wrist_enc);
  // make_proxied(drivers[WRIST_JOINT_ID]);
  drivers[WRIST_JOINT_ID]->activate();

  drivers[WRIST_ROT_ID] = new EncodedJoint(WRIST_ROT_ID,&twist_enc);
  // make_proxied(drivers[WRIST_ROT_ID]);
  drivers[WRIST_ROT_ID]->activate();

  drivers[HAND_ID] = new EncodedJoint(HAND_ID,&hand_enc);
  // make_proxied(drivers[HAND_ID]);
  drivers[HAND_ID]->activate();

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
  unsigned long curr_time = millis();
  if (PI_SERIAL.available()){
    /*
    char read = Serial.read();
    Serial.println(read);
    parsePhoneCommand(read);
    */
    int read = PI_SERIAL.readBytesUntil('\n', buffer, BUFFER_LEN);
    buffer[read] = 0; //null terminate the string just in case
    int parsed = parseCommand(buffer); //what if there's more?
    prev_message_time = curr_time;
  }

  if (curr_time-prev_message_time > AUTO_KILL_TIME) {
    kill();
  }

  int i;
  for(i = 0; i < NUM_JOINTS; i++){
    if(!drivers[i]) continue;
    drivers[i]->update();
    // report_joint_data(drivers[i]);
  }

  // proxyCull(PROXY_CULL_TIME);
  for(i=0;i<NUM_JOINTS;i++){
    if(manual_timings[i] && manual_timings[i] < curr_time){
      analogWrite(manual_signals[i],0);
      manual_timings[i] = 0;
    }
  }

  // while(ARM_SERIAL.available()){
  //   int read = ARM_SERIAL.readBytes(buffer,BUFFER_LEN);
  //   buffer[read] = 0;
  //   PI_SERIAL.println(buffer);
  // }

  // request_joint_goals();


}
