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

#define MAX_SPEED 255

ROV rover;

char val; 

const int BUFFER_LEN = 64;
char buffer[BUFFER_LEN];
char delim = ',';

void setup() {
  Serial.begin(19200);

  //Serial.print("Setting rover wheels...");
  //Serial.begin(9600);
  
  if (rover.setup() == ROV_OK) {
    //Serial.println("Failed to fail.");
  } else {
    //Serial.println("Failed.");
    while(1) {}
  }
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

int parseCommand(char* input) {
  int i = 0;
  int j = 0;
  int arg = 0;
  int num_vals = 3;
  float vs[num_vals];
  for (; input[i] != 0; i++) {
    if (input[i] == delim) {
      input[i] = 0;
      vs[arg++] = atof(input+j);
      j = i + 1;
      if(arg == num_vals){
        break;
      }
    }
  }

  for(i=0;i<num_vals-1;i++){
    Serial.print((int)vs[i]);
    Serial.print(", ");
  }
  Serial.println((int)vs[i]);
  rover.setSpeeds(vs[0],vs[1],vs[2]);
  return i;
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

void loop() {
  if (Serial.available()) {
    /*
    char read = Serial.read();
    Serial.println(read);
    parsePhoneCommand(read);
    */
    int read = Serial.readBytesUntil('\n', buffer, BUFFER_LEN);
    buffer[read] = 0; //null terminate the string just in case
    //Serial.write(buffer);
    //int parsed = parseCommand(buffer); //what if there's more?
    int parsed = parseMotorDuties(buffer); //what if there's more?
  }
}
