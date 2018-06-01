#include "rov.h"

/**
 * @brief Creates wheels from defines
 * @return Error code defined in common.
 */
int ROV::setup(void) {
  drivetrain.push_back(MotorDriver(0,0.4218, -0.53975, 255, false,
      ROV_TL_PIN_PWM, ROV_TL_PIN_FREEWHEEL, ROV_TL_PIN_RSTOP, ROV_TL_PIN_DIR));
  drivetrain.push_back(MotorDriver(1,0.4218, -0, 255, false,
      ROV_ML_PIN_PWM, ROV_ML_PIN_FREEWHEEL, ROV_ML_PIN_RSTOP, ROV_ML_PIN_DIR));
  drivetrain.push_back(MotorDriver(2,0.4080, -0.381, 255, false,
      ROV_BL_PIN_PWM, ROV_BL_PIN_FREEWHEEL, ROV_BL_PIN_RSTOP, ROV_BL_PIN_DIR));
  drivetrain.push_back(MotorDriver(3,0.4080, 0.381, 255, true,
      ROV_BR_PIN_PWM, ROV_BR_PIN_FREEWHEEL, ROV_BR_PIN_RSTOP, ROV_BR_PIN_DIR));
  drivetrain.push_back(MotorDriver(4,0.4218, 0, 255, true,
      ROV_MR_PIN_PWM, ROV_MR_PIN_FREEWHEEL, ROV_MR_PIN_RSTOP, ROV_MR_PIN_DIR));
  drivetrain.push_back(MotorDriver(5,0.4218, 0.53975, 255, true,
      ROV_TR_PIN_PWM, ROV_TR_PIN_FREEWHEEL, ROV_TR_PIN_RSTOP, ROV_TR_PIN_DIR));

  return ROV_OK;
}

int ROV::setDuties(std::vector<int> duties) {
  for (int i = 0; i < drivetrain.size(); i++) {
    drivetrain[i].setDuty(duties.at(i));
  }
}

/**
 * @brief Update all wheels
 * @return Error code defined in common.h
 */
int ROV::setSpeeds(float turningRadius, float turningSpeed, float forwardVel) {

  // UPDATE DIRECTION
  float maxTurningSpeed = 0;

  for (int i=0; i < drivetrain.size(); i++) {
    if (maxTurningSpeed < drivetrain.at(i).getTurningSpeed(turningRadius, turningSpeed)){
      maxTurningSpeed = drivetrain.at(i).getTurningSpeed(turningRadius, turningSpeed);
    }
  }
  maxTurningSpeed = (maxTurningSpeed != 0) ? maxTurningSpeed : 1;

  for (int i=0; i < drivetrain.size(); i++) {
    drivetrain[i].setSpeed(turningRadius, turningSpeed, maxTurningSpeed, forwardVel);
  }
  /*

  if(turningRadius == 0){
    for (int i=0; i < drivetrain.size(); i++) {
      drivetrain[i].setDuty(forwardVel);
    }
  } else{
    int sign = (turningRadius > 0)?1:-1;
    for (int i=0; i < drivetrain.size(); i++) {
      if(i < 3){
        drivetrain[i].setDuty(200*sign);
      }else{
        drivetrain[i].setDuty(200*sign*-1);
      }
    }
  }
  */
  return ROV_OK;
}
