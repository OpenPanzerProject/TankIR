/* OP_Motors.h      Open Panzer Motors - a library of motor functions
 * Source:          openpanzer.org              
 * Authors:         Luke Middleton
 *   
 */ 


/* This is a stripped down version of the OP_Motors class used on the Open Panzer TCB
 * This version only controls a recoil servo
 */
 
#ifndef OP_Motors_h
#define OP_Motors_h

#include "Settings.h"
#include "Servo.h"

// ESC Position type. Motor controllers we will be using (including the onboard L298) are all dual, so we have SIDEA and SIDEB.
// We can also use servo outputs to control speed controllers, there are 8 possible positions for those. 
typedef enum ESC_POS_t
{   
    SERVO_1 = 0,
};

class Motor {
  protected:
    ESC_POS_t ESC_Position;
    int e_minspeed, e_maxspeed, e_middlespeed;      // We have external speed range, and internal. External is the range of numbers that will be passed
                                                    // to the motor object from our main sketch. These are likely to be -255 to 255
    int i_minspeed, i_maxspeed, i_middlespeed;      // The internal is specific to each ESC, the external range will be mapped to the internal range
                                                    // For example, the internal range for a servo motor is 1000-2000. For Pololu Serial controllers it is -127 to 127
    int di_minspeed, di_maxspeed, di_middlespeed;   // We also have "backup" or "default" values of internal speed, so we can modify internal speed
                                                    // temporarily and then easily revert back to the default. 
    int curspeed;                                   // Current speed
    boolean reversed;                               // Motor reversed

  public:
    // Constructor, set member ESC_Position, external speed range, and reversed status
    Motor (ESC_POS_t pos, int min, int max, int middle, boolean rev=false) : ESC_Position(pos), e_minspeed(min), e_maxspeed(max), e_middlespeed(middle), reversed(rev) {}
    
    // The external range is the range of values our motor object should expect to be passed for control.
    void set_ExternalRange (int min, int max, int middle) 
        { this->e_minspeed = min; this->e_maxspeed = max; this->e_maxspeed = middle; }

    // This is the internal range of values that is specific to each motor driver.
    void set_InternalRange (int min, int max, int middle)
        { this->i_minspeed = min; this->i_maxspeed = max; this->i_middlespeed = middle; }

    // This is a copy of the internal range. In case we want to temporarily modify the range of possible output values,
    // we can use cut_Speed and then revert back to defaults by using restore_Speed
    void set_DefaultInternalRange (int min, int max, int middle)
        { this->di_minspeed = min; this->di_maxspeed = max; this->di_middlespeed = middle; }

    // Restores both positive and negative speed ranges to internal defaults
    void restore_Speed(void)        
        { this->i_minspeed = this->di_minspeed; this->i_maxspeed = this->di_maxspeed; this->i_middlespeed = this->di_middlespeed; }

    // Functions to set/get the reversed status
    void set_Reversed (boolean rev) 
        { this->reversed = rev; }
    boolean isReversed(void)
        { return this->reversed; }

    // Other common functions
    void cut_SpeedPct(uint8_t);         // Cut the total speed range by some percent
    void set_MaxSpeedPct(uint8_t);      // Alternate way of writing cut_SpeedPct

    // This maps the external speed range to the internal one
    int map_Range(int s)
    {   if (s == this->e_middlespeed) {return this->i_middlespeed;}
        else 
        {   if (s > this->e_middlespeed)
            {
                if (this->reversed) return map(s, this->e_middlespeed, this->e_maxspeed, this->i_middlespeed, this->i_minspeed); 
                else                return map(s, this->e_middlespeed, this->e_maxspeed, this->i_middlespeed, this->i_maxspeed);
            }
            else
            {
                if (this->reversed) return map(s, this->e_middlespeed, this->e_minspeed, this->i_middlespeed, this->i_maxspeed); 
                else                return map(s, this->e_middlespeed, this->e_minspeed, this->i_middlespeed, this->i_minspeed); 
            }
        }
    }   
    
    int getSpeed(void) { return this->curspeed; }
    virtual void setSpeed(int) =0;                  // Purely virtual
    virtual void begin(void) =0;
    virtual void stop(void) =0;
    virtual void update(void) =0;
};

class Servo_RECOIL: public Motor, public OP_Servos {
  public:
    Servo_RECOIL(ESC_POS_t pos, int min, int max, int middle, uint16_t mS_Recoil, uint16_t mS_Return, uint8_t Reversed) : Motor(pos,min,max,middle), _RecoilmS(mS_Recoil), _ReturnmS(mS_Return), _Reversed(Reversed) {}
    void setSpeed(int); // This doesn't do anything for this particular derived class
    void setLimits(uint16_t, uint16_t); // Set end-point limits on the servo object, not the motor
    void begin(void);
    void stop(void);    // This doesn't do anything for this particular derived class
    void Recoil(void)
        { this->StartRecoil(this->ESC_Position); }
    void update(void) { return; }   // Do nothing
  private:
    const uint16_t _RecoilmS;
    const uint16_t _ReturnmS;
    const uint8_t  _Reversed;   
};


#endif //OP_Motors_h





