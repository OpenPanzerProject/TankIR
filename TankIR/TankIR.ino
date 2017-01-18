/* OpenPanzer IR    A standalone IR tank IR device for use on the Arduino UNO or Nano (or any Arduino compatible with an ATmega328 processor)
 * Source:          openpanzer.org              
 * Authors:         Luke Middleton
 * 
 * Copyright 2017 Open Panzer
 *   
 * For more information, see the Open Panzer Wiki:
 * http://openpanzer.org/wiki/
 * 
 * TCB GitHub Repository:
 * https://github.com/OpenPanzerProject/TCB
 *    
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */ 

#include "A_Setup.h"
#include "Settings.h"
#include "SimpleTimer.h"
#include "IRLib.h"
#include "IRLibMatch.h"
#include "Button.h"
#include "Tank.h"


// GLOBAL VARIABLES
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------>>

// SIMPLE TIMER 
    OP_SimpleTimer timer;                        // SimpleTimer named "timer"
    boolean TimeUp = true;

// DEBUG FLAG
    boolean DEBUG = true;                        // Whether or no to send informational messages out the serial port during routine operation

// TANK OBJECTS
    OP_Servos TankServos;
    OP_Tank Tank;                              

// MOTOR OBJECTS
    // We always have a recoil servo
        Servo_RECOIL * RecoilServo;
// REPAIR
    #define REPAIR_NONE     0                     // No repair operation ongoing
    #define REPAIR_SELF     1                     // We are being repaired by another tank
    #define REPAIR_OTHER    2                     // We are repairing another tank
    uint8_t RepairOngoing = REPAIR_NONE;          // Init 

// INPUT BUTTON
    OP_Button InputButton = OP_Button(pin_Button, true, true, 25);   // Initialize a button object. Set pin, internal pullup = true, inverted = true, debounce time = 25 mS
    

void setup()
{   // Here we get everything started. Begin with the most important things, and keep going in descending order

    // INIT SERIALS & COMMS
    // -------------------------------------------------------------------------------------------------------------------------------------------------->
        Serial.begin(USB_BAUD_RATE);                               // Hardware Serial 0 - Connected to FTDI/USB connector

    // PINS 
    // -------------------------------------------------------------------------------------------------------------------------------------------------->
        // These pins are defined in Settings.h

        // Pushbutton - held to ground when pushed
            pinMode(pin_Button, INPUT_PULLUP);      // Input    - Pushbutton input
    
        // LEDs and Lights
            pinMode(pin_RedLED, OUTPUT);            // Output   - Red   LED (optional)
            RedLedOff();
            pinMode(pin_GreenLED, OUTPUT);          // Output   - Green LED (LED on Arduino boards)
            GreenLedOff();
            
            pinMode(pin_HitNotifyLEDs, OUTPUT);     // Output   - Hit notification LEDs if using the Tamiya apple. Tank class will initialize to off. 
            pinMode(pin_MuzzleFlash, OUTPUT);       // Output   - Use to trigger a Taigen high-intensity Flash Unit
         
    // TIMERS
    // -------------------------------------------------------------------------------------------------------------------------------------------------->
    // These are macros, see Settings.h for definitions
        SetupTimer1();
        
    // MOTOR OBJECTS
    // -------------------------------------------------------------------------------------------------------------------------------------------------->    
        TankServos.begin();             

    // RECOIL SERVO DEFINITION
    // -------------------------------------------------------------------------------------------------------------------------------------->>
    // We still pass an external min/max speed although it won't be used for this object. 
    // What will be used are recoil/return times, along with a reverse setting if the servo needs to be reversed. These can be modified
    // later but will be initialized to sensible defaults.
        #define SERVONUM_RECOIL     0   // Recoil servo is servo #0 (Port B0)
        RecoilServo = new Servo_RECOIL (SERVONUM_RECOIL,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0,RECOIL_MS,RETURN_MS,REVERSE_RECOIL);  
        // Recoil servos also have custom end-points. Because RecoilServo is a motor of class Servo, we can call setMin/MaxPulseWidth from the servo class directly, rather than from TankServos
        RecoilServo->setMinPulseWidth(SERVONUM_RECOIL, RECOIL_SERVO_EP_MIN);
        RecoilServo->setMaxPulseWidth(SERVONUM_RECOIL, RECOIL_SERVO_EP_MAX);
        // The reversed setting needs to be applied both to the motor class (flag) as well as to the servo class (actual recoil movement settings). 
        RecoilServo->set_Reversed(REVERSE_RECOIL);                       // motor class method
        RecoilServo->setRecoilReversed(SERVONUM_RECOIL, REVERSE_RECOIL); // servo class method
        // The begin function will make sure the recoil servo is initialized to its "battery" position
        RecoilServo->begin();

    // OTHER OBJECTS - BEGIN
    // -------------------------------------------------------------------------------------------------------------------------------------------------->    
        weightClassSettings CustomClassSettings = { CUSTOM_CANNON_RELOAD, CUSTOM_RECOVERY_TIME, CUSTOM_CANNON_HITS, CUSTOM_MG_HITS };
        battle_settings BattleSettings;
        BattleSettings.WeightClass = WEIGHT_CLASS;
        BattleSettings.ClassSettings = CustomClassSettings;
        BattleSettings.IR_FireProtocol = IR_FIRE_PROTOCOL;
        BattleSettings.IR_Team = IR_TEAM;
        BattleSettings.IR_HitProtocol_2 = IR_HIT_PROTOCOL_ALT;
        BattleSettings.IR_RepairProtocol = IR_REPAIR_PROTOCOL;
        BattleSettings.IR_MGProtocol = IR_MG_PROTOCOL;
        BattleSettings.Use_MG_Protocol = false;     // We are not firing a machine gun
        BattleSettings.Accept_MG_Damage = MG_DAMAGE;
        BattleSettings.DamageProfile = TAMIYA_DAMAGE;
        BattleSettings.SendTankID = SEND_ID;                                      
        BattleSettings.TankID = TANK_ID;                                              
        // Now pass the battle settings and other settings to the Tank object
        Tank.begin(BattleSettings, RecoilServo, &timer);
  

    // RANDOM SEED
    // -------------------------------------------------------------------------------------------------------------------------------------------------->    
        randomSeed(analogRead(A0));

    // DUMP INFO
    // -------------------------------------------------------------------------------------------------------------------------------------------------->        
        DumpBattleInfo();

    // LEDS OFF
    // -------------------------------------------------------------------------------------------------------------------------------------------------->        
        RedLedOff();
        GreenLedOff();
}


void loop()
{
// MAIN LOOP VARIABLES
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------>>
// Battle Variables
    static boolean Alive = true;                                      // Has the tank been destroyed? If so, Alive = false
    static int DestroyedBlinkerID = 0;                                // Timer ID for blinking lights when tank is destroyed
    HIT_TYPE HitType;                                                 // If we were hit, what kind of hit was it
// Blinkers
    static int GreenBlinker = 0;
    static int RedBlinker = 0;
// Time Variables
    static unsigned long currentMillis; 
// Timing stuff
    currentMillis = millis(); 
// Button stuff
    enum {BUTTON_WAIT, BUTTON_TO_WAIT};       
    static uint8_t ButtonState;                                       //The current button state machine state


    // PER-LOOP UPDATES
    // -------------------------------------------------------------------------------------------------------------------------------------------------->
        PerLoopUpdates();       // Reads the input button, and updates all timers


    // CHECK THE BUTTON
    // -------------------------------------------------------------------------------------------------------------------------------------------------->
        switch (ButtonState) 
        {
            // This state watches for short and long presses, dumps debug info with a short press, 
            // or enters a special menu with a long press
            case BUTTON_WAIT:                
                if (InputButton.wasReleased())
                {   // A single press (short) of the button will fire the cannon
                    FireCannon();
                }
                else if (InputButton.pressedFor(2000)) 
                {
                    // User has held down the input button for two seconds (long press)
                    // Wait for them to release the button before proceeding
                    do { delay(10); InputButton.read(); } while (!InputButton.wasReleased());                     
                    ButtonState = BUTTON_WAIT;

                    // Now - you could take some other action here to occur on long button press
                    
                }
                break;

            //This is a transition state where we just wait for the user to release the button
            //before moving back to the WAIT state.
            case BUTTON_TO_WAIT:
                if (InputButton.wasReleased()) ButtonState = BUTTON_WAIT;
                break;
        }


    // BATTLE 
    // ------------------------------------------------------------------------------------------------------------------------------------------------>  
    // Were we hit? 
    if (Alive) HitType = Tank.WasHit();
    else       HitType = HIT_TYPE_NONE;
    
    if (HitType != HIT_TYPE_NONE)
    {
        // We flash the onboard LED when IR signals are detected. Turn it off now they are done.
        GreenLedOff();
        
        // We were hit. But was it a damaging hit, or a repair hit? 
        switch (HitType)
        {
            case HIT_TYPE_CANNON: 
                Serial.print(F("CANNON HIT! (")); 
                Serial.print(ptrIRName(Tank.LastHitProtocol()));
                if (Tank.LastHitTeam() != IR_TEAM_NONE) Serial.print(ptrIRTeam(Tank.LastHitTeam()));
                Serial.println(F(")"));
                
                if (Tank.isRepairTank() && REPAIR_ON_HIT)
                {
                    // We want to respond to hits with a repair signal
                    FireCannon();
                    // Cancel the hit, it doesn't affect us
                    HitType = HIT_TYPE_NONE;
                }
                else
                {
                    // Were we in the middle of a repair?
                    if (RepairOngoing) { Serial.println(F("REPAIR OPERATION CANCELLED")); }
                    if (RepairOngoing) { RepairOngoing = REPAIR_NONE; }   // End repair if we were in the middle of one
                }
                break;
            case HIT_TYPE_MG:
                Serial.print(F("MACHINE GUN HIT! (")); 
                Serial.print(ptrIRName(Tank.LastHitProtocol()));
                Serial.println(F(")"));                    
                // Were we in the middle of a repair?
                if (RepairOngoing) { Serial.println(F("REPAIR OPERATION CANCELLED")); }
                if (RepairOngoing) { RepairOngoing = REPAIR_NONE; }   // End repair if we were in the middle of one
                break;
            case HIT_TYPE_REPAIR:
                if (!RepairOngoing && Tank.isRepairOngoing()) 
                {                   
                    // This marks the start of a repair operation
                    RepairOngoing = REPAIR_SELF;    // Repair self, meaning, we are the one being repaired
                    Serial.print(F("VEHICLE REPAIR STARTED (")); 
                    Serial.print(ptrIRName(Tank.LastHitProtocol()));
                    Serial.println(F(")"));                    
                }
                break;                
        }

        // Now apply "damage", it doesn't matter if we were "damaged" or "repaired". The Damage function will take into account
        // the current amount of damage. If we were repaired, the amount of damage will be less than before, so the Damage function will 
        // calculate a new, lesser damage. Of course if we were hit by cannon or machine gun fire, then Damage will calculate a new, more severe damage. 
        if (HitType != HIT_TYPE_NONE) { Tank.Damage(); }

        // Now show the remaining health level if this was a damaging hit (not a repair hit)
        if (HitType != HIT_TYPE_REPAIR && HitType != HIT_TYPE_NONE) { Serial.print(F("Health Level: ")); Serial.print(Tank.PctHealthRemaining()); Serial.println(F("%")); }

        if (Tank.isDestroyed && Alive)
        {
            Serial.println(F("TANK DESTROYED")); 
            Alive = false;
        }
    }

    // Were we in a repair operation, and now is it complete? 
    if (RepairOngoing && !Tank.isRepairOngoing()) 
    {                   
        if (REPAIR_SELF) // Only show our health level if we were the one being repaired (as opposed to repairing someone else)
        { 
            Serial.println(F("VEHICLE REPAIR COMPLETE")); 
            Serial.print(F("Health Level: ")); Serial.print(Tank.PctHealthRemaining()); Serial.println(F("%")); 
        }  
        // Tank repair is over. 
        RepairOngoing = REPAIR_NONE;        
    }

    
    // Were we destroyed and now are recovered? 
    if (!Alive && !Tank.isDestroyed)
    {   // We're now alive
            Alive = true;
            Serial.println(F("TANK RESTORED")); 
    }
}


void FireCannon()
{
    if (Tank.CannonReloaded())          // Only fire if reloading is complete
    {   
        if (Tank.isRepairTank()) 
        {   
            if (!Tank.isRepairOngoing() && !RepairOngoing)
            {
                // If we are a repair tank, we immobilze the tank when firing the repair signal. 
                // This is very similar to what we do if we *receive* a repair signal
                RepairOngoing = REPAIR_OTHER;   // This marks the start of a repair operation - we are repairing an other vehicle
                Serial.println(F("Fire Repair Signal"));
               
                // Now fire the repair signal. 
                Tank.Fire(); 
            }
        }
        else
        {
            // This is a fighting tank. But we can't fire the cannon if we're in the midst of being repaired by another tank.
            if (!Tank.isRepairOngoing())
            {
                Serial.println(F("Fire Cannon"));  
                Tank.Fire(); // See OP_Tank library. This triggers the mechanical and servo recoils, the high intensity flash unit, the cannon sound, and it sends the IR signal
            }
        }
    }
}


