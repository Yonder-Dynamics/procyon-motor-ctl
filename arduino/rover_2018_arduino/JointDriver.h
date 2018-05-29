#pragma once

class JointDriver{
    public:
        virtual float getGoal();
        virtual void setGoal(float goal);
        virtual float getAngle();
        virtual char update();
};