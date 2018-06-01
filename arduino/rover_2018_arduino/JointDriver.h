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
        void setDigitalOut(void (*fn)(int,int));
        void setAnalogOut(void (*fn)(int,int));
        void setPinMode(void (*fn)(int,int));
    private:
        int ID;
    protected:
        void (*digitalWrite)(int,int);
        void (*analogWrite)(int,int);
        void (*pinMode)(int,int);
};