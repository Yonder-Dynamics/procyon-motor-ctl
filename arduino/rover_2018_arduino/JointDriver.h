#pragma once

#include <Arduino.h>

class JointDriver{
    public:
        virtual JointDriver(int ID);
        virtual float getGoal();
        virtual void setGoal(float goal);
        virtual float getAngle();
        virtual char update();
        virtual void tare();
        int getID(){ return this->ID; };
        void setID(int ID){ this-> ID = ID; };
        virtual void setDigitalOut(void (*fn)(int,int));
        virtual void setAnalogOut(void (*fn)(int,int));
        virtual void setPinMode(void (*fn)(int,int));
        virtual void activate(){};
        virtual void kill();
        virtual void unkill();
    private:
        int ID;
    protected:
        void (*digitalWrite)(int,int);
        void (*analogWrite)(int,int);
        void (*pinMode)(int,int);
};
