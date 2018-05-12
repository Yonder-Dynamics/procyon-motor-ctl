#include "ActuatedJoint.h"

ActuatedJoint::ActuatedJoint(LAD* lad,mount_ptr mount):
    mountInfo(mount),actuator(lad){
    
    this->calcMountInfo();
    this->angle = this->getAngle();
}

float ActuatedJoint::getAngle(){
    // do some trig
    float left_a  = this->mountInfo->left_angle;
    float right_a = this->mountInfo->right_angle;

    float left_hyp  = this->mountInfo->left_hyp;
    float right_hyp = this->mountInfo->right_hyp;

    float ext = (float)this->actuator->getExtension();

    return left_a + right_a + cosines(ext,left_hyp,right_hyp);
}

float ActuatedJoint::calcGoalExt(float goal_angle){
    float spanned_angle = 
        goal_angle - this->mountInfo->left_angle - this->mountInfo->right_angle;
    return inv_cosines(spanned_angle,
                        this->mountInfo->left_hyp,
                        this->mountInfo->right_hyp);
}

void ActuatedJoint::setGoal(float goal){
    this->goal = goal;
    this->actuator->setGoal(this->calcGoalExt(this->goal));
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

    this->mountInfo->left_angle = cosines(left_p,left_a,left_hyp);
    this->mountInfo->right_angle = cosines(right_p,right_a,right_hyp);
}

void ActuatedJoint::update(){
    this->actuator->update();
}

//law of cosines
float cosines(float a,float b,float c){
    float cosA = (b*b + c*c - a*a)/(2*b*c);
    return acosf(cosA);
}

float inv_cosines(float A,float b,float c){
    return sqrt(b*b + c*c - 2*b*c*cosf(A));
}