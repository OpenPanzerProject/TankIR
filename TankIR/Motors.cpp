/* OP_Motors.cpp    Open Panzer Motors - a library of motor functions
 * Source:          openpanzer.org              
 * Authors:         Luke Middleton
 *   
 */ 
 

#include "Motors.h"


// ------------------------------------------------------------------------------------------------------------------>>
// GENERIC MOTOR CONTROL FUNCTIONS
// ------------------------------------------------------------------------------------------------------------------>>
// This function will cut motor speed by whatever percent is passed. It can be used to temporarily modify the maximum
// speed a motor can go (useful for battle damage). We can always restore the motor to whatever its full speed is by 
// calling restore_Speed();
// In this case, we take the cut and apply it equally to min and max (forward and backward). 
void Motor::cut_SpeedPct(uint8_t cut_pct)
{ 
    if (cut_pct == 0) return;                           // If cut is 0, then there is nothing to cut. 

    // We make sure cut is a "percent" between 1 - 100
    // The higher the number, the greater the speed will be cut
    cut_pct = constrain(cut_pct, 1, 100);
    unsigned long diff;                                 // Use a long because numbers could get big here for a moment
    diff = (this->di_maxspeed - this->di_minspeed);     // First we calculate the entire difference between min and max speed
    diff = (unsigned long)cut_pct * diff;               // Next we multiply this difference by our "percent" 1-100
    diff = diff / 200;                                  // Dividing by 200 gives us half the percentage value of our speed range
    this->i_minspeed = this->di_minspeed + (int)diff;   // We apply half to the minimum speed
    this->i_maxspeed = this->di_maxspeed - (int)diff;   // and the other half to the maximum speed
}
// Just a different way of doing the above. Instead of passing the percent to cut, 
// we pass the max speed possible, and it cuts the rest. 
void Motor::set_MaxSpeedPct(uint8_t max_pct)
{
    if (max_pct > 100) return;                          // Max speed can't exceed 100
    max_pct = constrain(max_pct, 0, 100);
    this->cut_SpeedPct(100-max_pct);
}


// ------------------------------------------------------------------------------------------------------------------>>
// RECOIL SERVO CONTROL
// ------------------------------------------------------------------------------------------------------------------>>
void Servo_RECOIL::begin(void)
{
    this->attach(ESC_Position);
    
    this->setupRecoil_mS(ESC_Position, _RecoilmS, _ReturnmS, _Reversed);
    
    // This servo also needs to be initialized to its end position. 
    // Because it looks cool we will ramp it to battery, although this will also block code. 
    uint16_t p; 
    if (_Reversed)
    {   // Use this instead to go straight to the end position
        //this->writeMicroseconds(ESC_Position, this->getMinPulseWidth(this->ESC_Position));              
        
        p = this->getMinPulseWidth(this->ESC_Position);
        this->setRampSpeed_mS(ESC_Position, _ReturnmS, true);
        do {delay(1);}
        while (this->getPulseWidth(ESC_Position) > p);
        this->stopRamping(ESC_Position);
        this->writeMicroseconds(ESC_Position, p);
    }
    else
    {   // Use this instead to go straight to the end position
        //this->writeMicroseconds(ESC_Position, this->getMaxPulseWidth(this->ESC_Position));

        p = this->getMaxPulseWidth(this->ESC_Position);
        this->setRampSpeed_mS(ESC_Position, _ReturnmS, false);
        do {delay(1);}
        while (this->getPulseWidth(ESC_Position) < p);
        this->stopRamping(ESC_Position);    
        this->writeMicroseconds(ESC_Position, p);
    }
   
    // We don't need to set anything else, unless the user wants to modify the endpoints
}

// We also don't need a "Recoil" method because that is exposed by being a member of the OP_Servos class. Just call
// Servo_RECOIL(object).Recoil();

void Servo_RECOIL::setSpeed(int s)
{
    // This function does nothing
    return;
}

void Servo_RECOIL::stop(void)
{
    // This function does nothing
    return;
}

// This sets limits on the servo object, rather than the motor's internal range. 
void Servo_RECOIL::setLimits(uint16_t min, uint16_t max)
{
    this->setMinPulseWidth(ESC_Position, min);
    this->setMaxPulseWidth(ESC_Position, max);
}

