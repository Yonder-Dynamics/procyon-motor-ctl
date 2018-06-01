#include "ActuatedJoint.h"

ActuatedJoint::ActuatedJoint(int ID,LAD* lad,mount_ptr mount):
    mountInfo(mount),actuator(lad),JointDriver(ID){
    this->calcMountInfo();
    this->angle = this->getAngle();
}

float ActuatedJoint::getAngle(){
    // do some trig
    float left_a  = this->mountInfo->left_angle;
    float right_a = this->mountInfo->right_angle;

    float left_hyp  = this->mountInfo->left_hyp;
    float right_hyp = this->mountInfo->right_hyp;

    float ext = ((float)this->actuator->getExtension())/PINGER_SCALE;




    float angle = left_a + right_a + cosines(ext,left_hyp,right_hyp);
    if(this->mountInfo->flipped){
        angle = M_PI - angle;
    }
    // Serial.print("Angle2: ");
    // // Serial.print((long)(cosines(ext,left_hyp,right_hyp)*100));
    // printFloat(angle*180/M_PI);
    // Serial.println();
    return angle;
}

float ActuatedJoint::calcGoalExt(float goal_angle){
    float spanned_angle = 
        goal_angle - this->mountInfo->left_angle - this->mountInfo->right_angle;
    return inv_cosines(spanned_angle,
                        this->mountInfo->left_hyp,
                        this->mountInfo->right_hyp);
}

void ActuatedJoint::setGoal(float goal){
    if(this->mountInfo->flipped){
        goal = M_PI - goal;
    }
    this->goal = goal;
    this->actuator->setGoal(this->calcGoalExt(this->goal));
    // this->update();
}

float ActuatedJoint::getGoal(){
    return this->goal;
}

void ActuatedJoint::move(float movement){
    int dir = 0;
    if(movement > 0){
        dir = 1;
    }else if(movement < 0){
        dir = -1;
    }
    this->actuator->move(dir,(int)abs(movement));
}

void ActuatedJoint::calcMountInfo(){
    float left_a = this->mountInfo->left_mount_aligned;
    float left_p = this->mountInfo->left_mount_perp;

    float right_a = this->mountInfo->right_mount_aligned;
    float right_p = this->mountInfo->right_mount_perp;

    float left_hyp  = sqrt(left_a*left_a + left_p*left_p);
    float right_hyp = sqrt(right_a*right_a + right_p*right_p);


    this->mountInfo->left_hyp = left_hyp;
    this->mountInfo->right_hyp = right_hyp;

    float left_ang = cosines(left_p,left_a,left_hyp);
    float right_ang = cosines(right_p,right_a,right_hyp);

    if(left_p < 0){
        left_ang = -left_ang;
    }
    if(right_p < 0){
        right_ang = -right_ang;
    }

    // Serial.print("Left Hyp: ");
    // printFloat(left_ang*180/M_PI);
    // Serial.print("\nRight Hyp: ");
    // printFloat(right_ang*180/M_PI);
    // Serial.println();

    this->mountInfo->left_angle = left_ang;
    this->mountInfo->right_angle = right_ang;
}

char ActuatedJoint::update(){
    return this->actuator->update();
}

void ActuatedJoint::setPinMode(void (*fn)(int,int)){
    JointDriver::setPinMode(fn);
    this->actuator->setPinMode(fn);
}

void ActuatedJoint::setDigitalOut(void (*fn)(int,int)){
    JointDriver::setDigitalOut(fn);
    this->actuator->setDigitalOut(fn);
}

void ActuatedJoint::setAnalogOut(void (*fn)(int,int)){
    JointDriver::setAnalogOut(fn);
    this->actuator->setAnalogOut(fn);
}

//law of cosines
float cosines(float a,float b,float c){
    float cosA = (b*b + c*c - a*a)/(2*b*c);
    float angle = acosf(cosA);
    return angle;
}

float inv_cosines(float A,float b,float c){
    return sqrt(b*b + c*c - 2*b*c*cosf(A));
}