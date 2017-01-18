/* Settings.h       Open Panzer Settings file for the Standalone IR circuit
 * Source:          openpanzer.org              
 * Authors:         Luke Middleton
 *
 * These values should not need to be modified by general users. 
 *   
 */ 


#ifndef SETTINGS_H
#define SETTINGS_H


// ------------------------------------------------------------------------------------------------------------------------------------------------------->>
// SERIAL PORT
// ------------------------------------------------------------------------------------------------------------------------------------------------------->>
    #define USB_BAUD_RATE               115200


// ------------------------------------------------------------------------------------------------------------------------------------------------------->>
// TIMER 1 (16 bit)
// ------------------------------------------------------------------------------------------------------------------------------------------------------->>
    // Timer 1 is used by several classes:
    // [] PPMDecode - checks the value of TCNT1 everytime the PPM input pin changes (the pin is using an external interrupt). By checking the time since the
    //    last signal, it knows how long the pulse-width is
    // [] OP_Servos - uses Timer 1's Output Compare A to set a timed interrupt to generate servo pulse widths
    // [] IRsendBase - uses Timer 1's Output Compare B to set a timed interrupt to generate infra-red pulses. IRsend also uses Timer 2 for the actual PWM.

    // We set up Timer 1 in Normal Mode: count starts from BOTTOM (0), goes to TOP (0xFFFF / 65,535), then rolls over. 
    // We set prescaler to 8. With a 16MHz clock that gives us 1 clock tick every 0.5 uS (0.0000005 seconds).
    // The rollover will occur roughly every 32.7 mS (0.0327 seconds). These settings are dictated by TCCR1A and TCCR1B.
    // We also clear all interrupt flags to start (write 1 to respective bits of TIFR1). 
    // And we start off with all interrupts disabled (write 0 to all bits in TIMSK1). 
    #define SetupTimer1() ({ \  
        TCCR1A = 0x00;       \
        TCCR1B = 0x02;       \
        TIFR1 =  0x2F;       \
        TIMSK1 = 0x00;       \
        TCNT1 = 0; })
    
    // We also need to let these libraries know what the conversion is between ticks and uS
    #define IR_uS_TO_TICKS(s)       (s*2)       // For IR sending
    #define SERVO_uS_TO_TICKS(s)    (s*2)       // For converting servo pulse-widths to tick counts
    #define SERVO_TICKS_TO_uS(s)    (s/2)       // For converting servo tick counts to pulse widths
    
    // Each of these libraries still have many hardcoded references to Timer 1, so if you ever do decide to change the timer you will have to do more than
    // just modifing the above...
    

// ------------------------------------------------------------------------------------------------------------------------------------------------------->>
// TIMER 2
// ------------------------------------------------------------------------------------------------------------------------------------------------------->>
    // Timer 2 is used to generate the PWM for IR sending. It is used in phase-correct PWM mode, with OCR2A controlling the frequency and 
    // OCR2B controlling the duty cycle. There is no prescaling, so the output frequency is 16MHz / (2 * OCR2A)
    // To turn the output on and off, the PWM running is left running, but we connect and disconnect the output pin. 
    // This pin is hardcoded below to Port D3 / Arduino pin 3 (OC2B).
    // As Ken Shirriff said, a few hours staring at the ATmega documentation and this will all make sense.
    // For more info, see his website "Secrets of Arduino PWM" at http://www.righto.com/2009/07/secrets-of-arduino-pwm.html
    #define IR_SEND_PWM_PIN         3                               // Arduino pin 3
    #define IR_SEND_PWM_START       (TCCR2A |= _BV(COM2B1))         // Macro to connect OC2B to PWM pin
    #define IR_SEND_PWM_STOP        (TCCR2A &= ~(_BV(COM2B1)))      // Macro to disconnect OC2B from PWM pin
    // This sets up the modulation frequency in kilohertz
    #define IR_SEND_CONFIG_KHZ(val) ({ \
                                    const uint8_t pwmval = SYSCLOCK / 2000 / (val); \
                                    TCCR2A = _BV(WGM20);  TCCR2B = _BV(WGM22) | _BV(CS20); \
                                    OCR2A = pwmval; OCR2B = pwmval / 3; })
    


// ------------------------------------------------------------------------------------------------------------------------------------------------------->>
// SIMPLE TIMER
// ------------------------------------------------------------------------------------------------------------------------------------------------------->>
    // We use the OP_SimpleTimer class for convenient timing functions throughout the project, it is a modified and improved version 
    // of SimpleTimer: http://playground.arduino.cc/Code/SimpleTimer
    // The class needs to know how many simultaneous timers may be active at any one time. We don't want this number too low or operation will be eratic, 
    // but setting it too high will waste RAM. Each additional slot costs 19 bytes of global RAM. 

    // Our best estimate as of 9/22/2016 (version 00.91.06) is (on the TCB board, this is likely to be more than we need here but we're too lazy to recount): 
    // Main Sketch:     7       At least 14 slots but shouldn't be more than 7 active at any one time
    // OP_Tank:         11      At least 15 slots but shouldn't be possible for more than 11 to be active at one time
    //-----------------------
    // TOTAL:           18   

    #define MAX_SIMPLETIMER_SLOTS       20          // Based on the calculations above, this gives us a few extra slots in case we miscalculated or if we need to add more
                                                    // But any time you add more you should re-visit this list. Sometimes extra timer slots can be used that would only 
                                                    // operate at times when other timers must be inactive, so not all new timers require the creation of new slots. 


// ------------------------------------------------------------------------------------------------------------------------------------------------------->>
// PINS! 
// ------------------------------------------------------------------------------------------------------------------------------------------------------->>
    
    // Pushbutton
        #define pin_Button               4          // Input    - Input pushbutton 

    // Board LEDs
        #define pin_BoardLED             13         // Output   - Green LED on Arduino boards

    // Transistorized Light outputs
        #define pin_HitNotifyLEDs        5          // Output   - Hit notification LEDs if using the Tamiya apple. PWM capable
        #define pin_MuzzleFlash          6          // Output   - Trigger output for Taigen High Intensity muzzle flash unit

    // OTHER PINS
    // The remaining pins are defined in their respective libraries: 
        // - 4 Servos (top of OP_Servo.cpp - first 4 of PORTB (Arduino 8-11)
        // - IR sending (Arduino pin 3) and receiving (Arduino pin 2) - see OP_IRLib library


// ------------------------------------------------------------------------------------------------------------------------------------------------------->>
// MOTOR DRIVERS - GENERAL
// ------------------------------------------------------------------------------------------------------------------------------------------------------->>
    // THESE ARE THE EXTERNAL SPEEDS THAT WILL BE PASSED FROM THE SKETCH TO THE CONTROLLER CLASSES
    // The internal speed ranges (meaning, the ranges used internally by the different controllers) can vary depending on 
    // the type of the controller, and are set in their respective begin() functions
    // We use the same numbers for drive, turret, and smoker. What this number basically sets, is the maximum motor output resolution. 
    // YOU SHOULDN'T NEED TO MODIFY THIS EVER!
    #define MOTOR_MAX_FWDSPEED           255
    #define MOTOR_MAX_REVSPEED          -255
    #define MOTOR_MAX_FWDSPEED_DBL       255.0
    #define MOTOR_MAX_REVSPEED_DBL      -255.0


#endif
