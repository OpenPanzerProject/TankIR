/* OP_Tank.h        Open Panzer Tank - functions primarily related to battle
 * Source:          openpanzer.org              
 * Authors:         Luke Middleton
 *   
 */  

#ifndef OP_Tank_h
#define OP_Tank_h

#include <Arduino.h>
#include "Settings.h"
#include "IRLib.h"
#include "SimpleTimer.h"
#include "Motors.h"
#include "A_Setup.h"

// Repairs take 15 seconds
#define REPAIR_TIME_mS                  15000   // How long does a repair operation take. During this time the tank can still receive hits, but 
                                                // it can't move. 

// Tanks is dead for 15 seconds 
#define DESTROYED_INOPERATIVE_TIME_mS   15000   // How long is the vehicle immobilized after being destroyed. 15 seconds is the Tamiya spec. After this,
                                                // the vehicle will automatically re-generate with full health restored. 
                                            

#define HIT_FILTER_mS               1100    // After taking a hit, we ignore any further hits for this length of time in milliseconds. This prevents a single
                                            // shot from being recorded as multiple hits. Should be at least 1000 mS because stock Tamiya fires the hit signal
                                            // repeatedly for 1 full second

#define MUZZLE_FLASH_TRIGGER_mS     50      // Trigger signal length for Asiatam/Taigen high-intensity flash unit, or for user-supplied LED

// The IR receiver class needs to know which external interrupt to use. 
#define IR_RECEIVE_INT_NUM          0       // On Arduino UNO/Nano we use external interrupt 0 (which maps to pin 2)

// These variables are used to create a flickering effect on the hit notification LEDs, similar to the way Tamiya does
#define MAX_BRIGHT                  255     // Maximum LED brightness during the flicker effect (should be 255)
#define MIN_BRIGHT                  10      // Minimum LED brightness during the flicker effect
#define BRIGHT_FADE_BREAK           150     // We will always ramp up to some value above this. Must be less than MAX_BRIGHT.
#define DIM_FADE_BREAK              130     // We will always ramp down to some value below this. Must by higher than MIN_BRIGHT.
#define MAX_FADE_STEP               50      // Maximum amount we will increase/decrease the brightness per step
#define MIN_FADE_STEP               10      // Minimum amount we will increase/decrease the brightness per step
#define FADE_UPDATE_mS              20      // How often we will increment/decrement the brightness
#define FLICKER_EFFECT_LENGTH_mS    3000    // How long to flicker the lights using the random fade up/down effect
// This defines how long a single machine-gun hit blink will last
#define MG_HIT_BLINK_TIME           100     // in milliSeconds

// There are four possible weight classes - the three standard Tamiya classes, 
// and one custom class defined by the user. 
typedef unsigned char WEIGHTCLASS; 
#define WC_CUSTOM       0
#define WC_LIGHT        1
#define WC_MEDIUM       2
#define WC_HEAVY        3
#define LAST_WEIGHT_CLASS   WC_HEAVY
const __FlashStringHelper *ptrWeightClassName(WEIGHTCLASS wClass); //Returns a character string that is name of tank class (see OP_Tank.cpp)


// See the Damage function in OP_Tank.cpp for definitions
typedef unsigned char DAMAGEPROFILES;
#define TAMIYA_DAMAGE       0       // Stock Tamiya damage profile
#define OPENPANZER_DAMAGE   1       // Open Panzer damage profile
//#define ADDITIONAL (number)
#define LAST_DAMAGE_PROFILE OPENPANZER_DAMAGE
const __FlashStringHelper *ptrDamageProfile(DAMAGEPROFILES dProfile); //Returns a character string that is name of the damage profile 


// All the types of IR receptions possible
typedef char HIT_TYPE;
#define HIT_TYPE_NONE       0       // No hit, signal couldn't be decoded, or it didn't apply to us
#define HIT_TYPE_CANNON     1
#define HIT_TYPE_MG         2
#define HIT_TYPE_REPAIR     3

// A collection of settings for the tank
struct weightClassSettings{
    uint16_t reloadTime;        // How long (in mS) does it take to reload the cannon. Depends on weight class
    uint16_t recoveryTime;      // How long does recovery mode last (invulnerability time when tank is regenerating after being destroyed). Class-dependent. 
    uint8_t  maxHits;           // How many hits can the tank sustain before being destroyed. Depends on weight class
    uint8_t  maxMGHits;         // How many hits can the tank sustain from machine gun fire before being destroyed. Only applies to custom weight classes, 
                                // and only if Accept_MG_Damage = TRUE
};
struct battle_settings{
    char     WeightClass;       // What is the tank's current weight class
    weightClassSettings ClassSettings;  // What are the settings for the weight class (max hits, reload time, recovery time)
    IRTYPES  IR_FireProtocol;   // Which battle protocol are we *sending* by cannon fire
    IRTEAMS  IR_Team;           // Does this tank belong to a team - only applies to a few protocols
    IRTYPES  IR_HitProtocol_2;  // We can accept hits from up to 2 protocols
    IRTYPES  IR_RepairProtocol; // Which repair protocol are we using
    IRTYPES  IR_MGProtocol;     // Which machine gun protocol are we using
    boolean  Use_MG_Protocol;   // If true, the Machine Gun IR code will be sent when firing the machine gun, otherwise, it will be skipped. 
    boolean  Accept_MG_Damage;  // If true, the vehicle will be susceptible to MG fire. 
    char     DamageProfile;     // Which Damage Profile are we using
    boolean  SendTankID;        // Do we include the Tank ID in the cannon IR transmission
    uint16_t TankID;            // What is this tank's ID number
};


class OP_Tank
{   public:
        OP_Tank(void);                              // Constructor
        static void     begin(battle_settings, Servo_RECOIL *, OP_SimpleTimer *);     
        // battle_settings, pointers to recoil servo, and sketch's SimpleTimer
        
        // Functions - Cannon Fire
        static void     Fire(void);                 // Fires the correct IR signal based on the IR protocol 
        static boolean  CannonReloaded(void);       // Has the cannon finished reloading?
        
        // Direct control over portions of the typical cannon fire event
        static void     TriggerMuzzleFlash(void);
        
        // Functions - IR receiving (ie, getting hit!)
        static HIT_TYPE WasHit(void);               // Have we been hit
        static IRTYPES  LastHitProtocol(void);      // What were we hit with
        static IRTEAMS  LastHitTeam(void);          // Which team hit us (if applicable)
        static uint8_t  PctDamaged(void);           // Returns a number from 0-100 of the percent damage taken
        static uint8_t  PctHealthRemaining(void);   // Returns a number from 0-100 of the percent of health remaining
        static boolean  isRepairOngoing(void);      // Returns the status of a repair operation
//        static void     Damage();                   // NOTE: The Standalone IR board does not have a speed to be reduced, therefore we have no "damage" function
        static battle_settings BattleSettings;      // Battle settings struct
        static boolean  isInvulnerable;             // Is the tank presently invulnerable to incoming fire
        static boolean  isDestroyed;                // Is the tank destroyed
        static uint8_t  CannonHitsTaken;            // How many cannon hits have we sustained
        static uint8_t  MGHitsTaken;                // How many machine gun hits have we sustained
                
        // Misc
        static boolean  isRepairTank(void);         // Returns status of fight/repair switch on the TCB.
        static void     StopRepair(void);           // The only time the sketch might need to call this is a failsafe, LVC or other dire situation occured, and we want to
                                                    // shut everything down. Some functions check for a repair before they actually do anything (ie EngineOn/EngineOff in the sketch).
                                                    // If we force a repair to be over first then those other functions will be sure to run as expected.
                                                    // This function is not used to actually stop a repair in normal practice, that is taken care of automatically. 
        
    private:
        // Setup
        static void     SetupTamiyaWeightClass(char);   // Initializes the weight class settings for a standard Tamiya weight class
    
        // IR objects
        static IRsend     IR_Tx;
        static IRrecvPCI *IR_Rx;    
        static IRdecode   IR_Decoder;

        // Cannon Firing
        static void     Cannon_Flash(void);
        static void     Cannon_SendIR(void);
        static void     Cannon_RepairLights(void);
        static void     Cannon_StartReload();
        static void     ReloadComplete(void);
        static boolean  CannonReloadComplete;
    
        // High Intensity Flash 
        static void     ClearMuzzleFlash(void);

        // Incoming hits
        static void     EnableHitReception(void);
        static void     DisableHitReception(void);
        static void     ResetBattle(void);
        static IRTYPES  _lastHit;
        static IRTEAMS  _lastTeam;
        
        // Hit notification LEDs
        static boolean  HitLEDsOn;                  // True if currently ON or DIM, False if OFF
        static void     HitLEDs_On(void);
        static void     HitLEDs_Off(void);
        static void     HitLEDs_Toggle(void);
        static void     HitLEDs_SetDim(uint8_t level);
        static void     HitLEDs_Blink(long blinkTime);
        static void     HitLEDs_CannonHit(void);    // Cannon-hit damage light effect
        static void     CannonHitLEDs_Update(void);
        static void     CannonHitLEDs_Stop(void);
        static void     HitLEDs_MGHit(void);        // Machine gun-hit damage light effect
        static void     HitLEDs_Destroyed(void);    // Destroyed light effect
        static void     Repair_BlinkHandler(void);  // Repair light effect
        static void     HitLEDs_Repair(void);       // Repair light effect
        static int16_t      FadeStep;
        static int16_t      FadeTarget;
        static int16_t      CurrentFadeLevel;
        static boolean  FadeOut;
        static int      FadeStep_TimerID;
        static int      HitLED_TimerID;
        static void     HitLEDs_ReloadNotify(void); // Blink on cannon reload, if enabled in A_Setup.h   

        // Damage/Repair
        static uint8_t  HitsTaken_Cannon;           // How many hits have we sustained
        static uint8_t  HitsTaken_MG;               // How many machine gun hits have we sustained      
        static float    DamagePct;                  // Damage the vehicle has sustained, in percent
        static float    DamagePctPerCannonHit;      // How many damage does a single cannon hit inflict
        static float    DamagePctPerMGHit;          // How many damage does a single round of machine gun fire inflict
        static boolean  RepairOngoing;              // Flag gets set if tank receives a repair code, and remains set until the operation is completed (see REPAIR_TIME_mS)
        static void     CancelRepair(void);         // If the model receive an enemy hit in the middle of a repair operation, we cancel the repair operation, do not increase the
                                                    // the health level, and apply damage as usual. 
        static void     RepairOver(void);           // This gets called if the repair is completed successfully. This is where the health is increased. 
        static int      RepairTimerID;              // Timer ID for the repair operation

        // Misc
        static boolean  IR_Enabled;                 // True if either cannon or MG enabled, false if both disabled
        static OP_SimpleTimer * TankTimer;
        static Servo_RECOIL * _RecoilServo;
        
};


#endif //OP_Tank_h
