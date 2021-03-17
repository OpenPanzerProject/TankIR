/* OP_Servo.h       Open Panzer Servo - a library for writing servo pulses to 8 pins on a single port
 * Source:          openpanzer.org              
 * Authors:         Michael Margolis, DuaneB, Luke Middleton
 *
 * This library is a modification of DuaneB's RCArduionFastLib: http://rcarduino.blogspot.com/
 * ...which itself was a modification of the standard Arduino Servo library written by Michael Margolis (Arduino Forum user "MEM")
 *
 * This library is hardcoded for use on an Arduino UNO/Nano with four servos on pins 8, 9, 10, 11
 *
 */ 
 
#ifndef OP_Servo_H
#define OP_Servo_H
#include <Arduino.h>
#include "Settings.h"

// This library is stripped down and makes several assumptions, so don't change anything unless you know exactly what you are doing! 
// Assumption 1 - we only have 4 servos
// Assumption 2 - they occupy the first 4 pins of a single Atmega port (the port can be changed in OP_Servos.cpp, but should be set to Port B for this circuit)
// Assumption 3 - Servos are numbered 0-3. They correspond to Pins 0-3 of the selected Port register

// If anything about those assumptions changes, this library will not work correctly, or at all! 

// The frame space is treated as an extra "servo" so this number needs to be 1 greater than the total number of actual outputs
// This library is hard-coded to 4 servos, so this number should always be 5! 
#define SERVO_OUT_COUNT         5

// Default minimum and maximum servo pulse widths. They can be modified on a per-servo basis 
// later if the user needs. 
#define SERVO_OUT_MINPULSE      750
#define SERVO_OUT_MAXPULSE      2250
#define SERVO_OUT_CENTERPULSE   1500
#define SERVO_MAXRAMP_TICKSTEP  50

class OP_Servos
{
    // We are using static for everything because we only want one instance of this class. 
public:
    OP_Servos();
//  static void setup();

    // configures timer1
    static void begin();

    // Called by the timer interrupt service routine, see the cpp file for details.
    // Don't really want it public, but it has to be for the ISR to see it
    static void OCR1A_ISR();

    // called to set the pulse width for a specific channel, pulse widths are in microseconds 
    static void attach(uint8_t);
    static void detach(uint8_t);
    static boolean isAttached(uint8_t);
    static void writeMicroseconds(uint8_t, uint16_t);
    static void setFrameSpace(uint8_t, uint16_t);
    static void setMinPulseWidth(uint8_t, uint16_t);
    static void setMaxPulseWidth(uint8_t, uint16_t);
    static uint16_t getMinPulseWidth(uint8_t);
    static uint16_t getMaxPulseWidth(uint8_t);
    static uint16_t getPulseWidth(uint8_t);
    static void setRampSpeed_mS(uint8_t, uint16_t, uint8_t);
    static void setRampStepPerFrame(uint8_t, int16_t);
    static void stopRamping(uint8_t);
    static void setupRecoil_mS(uint8_t, uint16_t, uint16_t, boolean);       // Setup recoil parameters, pass ramping speed in mS
    static void StartRecoil(uint8_t);   // Kick off a recoil event
    static void setRecoilReversed(uint8_t, boolean);
    
protected:
    class PortPin
    {   public:
            uint8_t  PinMask;
            uint16_t NumTicks;          // Current pulse width in timer ticks
            uint16_t MaxTicks;          // Maximum pulse width in ticks
            uint16_t MinTicks;          // Minimum pulse width in ticks
            boolean  Enabled;           // Is this servo enabled (attached)
            int16_t  TickStep;          // Used for slowly ramping a servo from one position to another
            uint8_t  RecoilState;       // Special flag for recoil effect
            uint32_t RecoilStartTime;   // Time when recoil action starts
            uint16_t RecoilTime_mS;     // How long to wait for the recoil action to complete
            uint16_t RecoiledNumTicks;  // The full-back position of the recoiled servo, in ticks. Will equal either MaxTicks or MinTicks depending on if the servo is reversed.
            int16_t  RecoilTickStep_Return; // Specific tick step for slowly returning the barrel to starting position after recoil kick
    };

    static boolean initialized; 
    static void setPinHigh(uint8_t) __attribute__((always_inline));
    static void setPinLow(uint8_t) __attribute__((always_inline));    
    static void setPulseWidthTimer(uint8_t) __attribute__((always_inline));
    static void setPulseWidthTimer_Ramp(uint8_t);
    
    // Information about each channel
    static volatile PortPin Channel[SERVO_OUT_COUNT]; 
    
    // current output channel
    static volatile uint8_t CurrentChannel;    

    
private:    
    // Remember, static variables must be initialized outside the class
    static uint16_t _GlobalMaxTicks;
    static uint16_t _GlobalMinTicks;
};



#endif
