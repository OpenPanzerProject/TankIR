/* OP_Tank.cpp      Open Panzer Tank - functions primarily related to battle
 * Source:          openpanzer.org              
 * Authors:         Luke Middleton
 *   
 */  


#include "Tank.h"

// Static variables must be declared outside the class
battle_settings OP_Tank::BattleSettings;
OP_SimpleTimer * OP_Tank::TankTimer;    
boolean         OP_Tank::IR_Enabled;
IRsend          OP_Tank::IR_Tx;
IRrecvPCI     * OP_Tank::IR_Rx;
IRdecode        OP_Tank::IR_Decoder;
boolean         OP_Tank::CannonReloadComplete;
boolean         OP_Tank::isInvulnerable;            
boolean         OP_Tank::isDestroyed;           
Servo_RECOIL  * OP_Tank::_RecoilServo;
uint8_t         OP_Tank::CannonHitsTaken;
uint8_t         OP_Tank::MGHitsTaken;
float           OP_Tank::DamagePct;
float           OP_Tank::DamagePctPerCannonHit;
float           OP_Tank::DamagePctPerMGHit;
boolean         OP_Tank::RepairOngoing;
int             OP_Tank::RepairTimerID;
IRTYPES         OP_Tank::_lastHit;
IRTEAMS         OP_Tank::_lastTeam;

// Hit notification LED effect variables
boolean         OP_Tank::HitLEDsOn;
int16_t         OP_Tank::FadeStep;
int16_t         OP_Tank::FadeTarget;
int16_t         OP_Tank::CurrentFadeLevel;  
boolean         OP_Tank::FadeOut;
int             OP_Tank::FadeStep_TimerID;
int             OP_Tank::HitLED_TimerID;



// Return a character string of the name of the weight class, used for printing
const __FlashStringHelper *ptrWeightClassName(WEIGHTCLASS wClass) {
  if(wClass>LAST_WEIGHT_CLASS) wClass=LAST_WEIGHT_CLASS+1;
  const __FlashStringHelper *Names[LAST_WEIGHT_CLASS+2]={F("Custom"), F("Light"), F("Medium"), F("Heavy"), F("Unknown")};
  return Names[wClass];
};


// Return a character string of the name of the damage profile, used for printing
const __FlashStringHelper *ptrDamageProfile(DAMAGEPROFILES dProfile) {
  if(dProfile>LAST_DAMAGE_PROFILE) dProfile = LAST_DAMAGE_PROFILE+1;
  const __FlashStringHelper *Names[LAST_DAMAGE_PROFILE+2]={F("Tamiya Spec"), F("Open Panzer"), F("Unknown")};
  return Names[dProfile];
};



// Constructor
OP_Tank::OP_Tank() 
{
    // Initialize
    CannonReloadComplete = true;
    IR_Enabled = true;
    RepairOngoing = false;
    HitLEDsOn = false;
    isDestroyed = false;        
    CannonHitsTaken = 0;        
    MGHitsTaken = 0;
    DamagePct = 0;   
    if (CANNON_RELOAD_NOTIFY) HitLEDs_ReloadNotify();   // If enabled, briefly blink the apple notification LEDs to signify reload is complete. 
    DisableHitReception();                      // We start by ignoring hits
    IR_Rx = new IRrecvPCI(IR_RECEIVE_INT_NUM);  // Pass the external interrupt number to the IRrecvPCI class (Arduino Interrupt 0 on the TCB - see OP_Tank.h)
    IR_Rx->setBlinkingOnReceive(true);        // For testing only. This will cause the board LED to flash on any IR reception, whether the IR can be decoded or not.
    
}


void OP_Tank::begin(battle_settings BS, Servo_RECOIL * sr,  OP_SimpleTimer * t)
{
    // Save settings
    
    BattleSettings = BS;        // Battle Settings    
    _RecoilServo = sr;
    TankTimer = t;              // Sketch's SimpleTimer

    // Do a quick sanity check on the IR_Team value
    if (BattleSettings.IR_Team != IR_TEAM_NONE)
    {   // If we aren't one of these protocols, change IR_TEAM back to NONE
        if (BattleSettings.IR_FireProtocol != IR_FOV)
            BattleSettings.IR_Team == IR_TEAM_NONE;
    }


    // Enable IR
    IR_Enabled = true;
    IR_Rx->enableIRIn();

    // If we aren't using a custom weight class, setup the specified Tamiya weight class
    SetupTamiyaWeightClass(BattleSettings.WeightClass);

    // Setup damage settings
    if (BattleSettings.IR_FireProtocol != IR_DISABLED && BattleSettings.IR_MGProtocol != IR_DISABLED && BattleSettings.Accept_MG_Damage)
    {
        // The vehicle will take damage from both cannon fire and machine gun fire. 
        DamagePctPerCannonHit = 100.0 / (float)BattleSettings.ClassSettings.maxHits;
        DamagePctPerMGHit =     100.0 / (float)BattleSettings.ClassSettings.maxMGHits;
    }
    else if (BattleSettings.IR_FireProtocol != IR_DISABLED)
    {
        // The vehicle will take damage from cannon fire only
        DamagePctPerCannonHit = 100.0 / (float)BattleSettings.ClassSettings.maxHits;
        DamagePctPerMGHit = 0.0;
    }
    else
    {
        // The vhicle will only take damage from machine gun fire (unlikely you would want this scenario)
        DamagePctPerCannonHit = 0.0;
        DamagePctPerMGHit = 100.0 / (float)BattleSettings.ClassSettings.maxMGHits;      
    }
    //Serial.print(F("Damage per Cannon Hit: "));   Serial.println(DamagePctPerCannonHit,2);
    //Serial.print(F("Damage per MG Hit: "));       Serial.println(DamagePctPerMGHit,2);

    // Start
    EnableHitReception();      // Accept incoming hits
    
}

boolean OP_Tank::isRepairTank()
{   
    return REPAIR_TANK; // Define set in A_Setup.h

    // Or you could use a physical switch  - 
    
    // Repair tank setting is set by the position of a physical switch on the TCB board. 
    // If LOW (held to ground), tank is fighter. If HIGH (through input pullup), tank is repair. 
//    return digitalRead(pin_RepairTank);
}

void OP_Tank::SetupTamiyaWeightClass(char weight_class)
{
    // This routine assigns settings according to the given Tamiya weight class
    // Settings are defined by the Tamiya standard, see the insert to Tamiya #53447 "Hop Up Options: Battle System"
    // There are three Tamiya classes: LIGHT, MEDIUM, and HEAVY.
    // Tamiya classes do not accept hits from machine gun fire.

    // Of course the user also has the option of creating a custom weight class, in which case this routine is skipped and the custom settings are used instead. 

    BattleSettings.WeightClass = weight_class;
    
    switch (BattleSettings.WeightClass)
    {
        //case CUSTOM:                                              // CUSTOM
        // The custom case is handled in the begin() function above
        
        case WC_LIGHT:                                              // LIGHT
            BattleSettings.ClassSettings.reloadTime = 3000;         // 3 second reload
            BattleSettings.ClassSettings.maxHits = 3;               // 3 hits before destruction
            BattleSettings.ClassSettings.recoveryTime = 15000;      // 15 second recovery (invulnerability) time
            break;

        case WC_HEAVY:                                              // HEAVY
            BattleSettings.ClassSettings.reloadTime = 9000;         // 9 second reload! 
            BattleSettings.ClassSettings.maxHits = 9;               // 9 hits before destruction
            BattleSettings.ClassSettings.recoveryTime = 10000;      // 10 second recovery (invulnerability) time
            break;

        case WC_MEDIUM:                                             // MEDIUM
        default:                                                    // Anything unknown, default to MEDIUM
            BattleSettings.ClassSettings.reloadTime = 5000;         // 5 second reload
            BattleSettings.ClassSettings.maxHits = 6;               // 6 hits before destruction
            BattleSettings.ClassSettings.recoveryTime = 12000;      // 12 second recovery (invulnerability) time
    }
}



//------------------------------------------------------------------------------------------------------------------------>>
// CANNON FIRE
//------------------------------------------------------------------------------------------------------------------------>>

void OP_Tank::Fire(void)
{
    // There is a lot going on when we fire the cannon, and the order of things can be different between airsoft and mechanical recoil, 
    // or whether the tank is a Repair tank or not. So we break it down into small parts and call them one after the other. 
    
    // But first of all, if the tank is the middle of being repaired (or in the middle of repairing another tank if this is a bergepanzer), 
    // then we do not fire anything at all - firing is disabled
    if (!RepairOngoing) // Don't do anything if repair is ongoing
    {
        // Is this a repair tank? 
        if (isRepairTank())
        {
            // This is a repair tank. We skip mechanical/servo recoil and airsoft. We do have a repair sound, and we also do a
            // special light effect on the hit notification LEDs (in the apple). And of course we also send the repair IR code. 
            RepairOngoing = true;   // Set the repair flag. It is the same flag if we are being repaired as it is if we are repairing someone else. 
            Repair_BlinkHandler();      // Do the special repair light effect (start blinking slow and gradually increase faster and faster)
            // Start the repair timer. During this time we can not fire the repair signal again, nor can we move (the move disabling is handled by the sketch)
            TankTimer->setTimeout(REPAIR_TIME_mS, RepairOver);   // REPAIR_TIME_mS is set in OP_BattleTimes.h
            Cannon_StartReload();   // Start the reload timer - but we actually still won't be able to fire again until after the repair is over, which takes longer than reloading.
            Cannon_SendIR();        // Send the IR code
        }
        // Or is this a fighting tank? 
        else 
        {   
            Cannon_SendIR();            // Send IR
            _RecoilServo->Recoil();     // Trigger recoil servo
            Cannon_Flash();             // Flash the high intensity flash unit
            Cannon_StartReload();       // Now start the reload timer

        }
    }
}
void OP_Tank::Cannon_Flash(void)
{
    TriggerMuzzleFlash();               // High intensity flash unit
}
void OP_Tank::Cannon_SendIR(void)
{
    // The user can choose to skip IR completely, so check first if it's enabled
    if (BattleSettings.IR_FireProtocol != IR_DISABLED)
    {
        // We don't want to hit ourselves. So while we are sending, we disable reception
        DisableHitReception();      
        if (isRepairTank())
        {   // We are a repair tank, send the repair signal if one is selected
            if (BattleSettings.IR_RepairProtocol != IR_DISABLED) IR_Tx.send(BattleSettings.IR_RepairProtocol);
        }
        else
        {   // We are a battle tank, send the battle signal. 
            // But also check if we need to send a team-specific signal. 
            if (BattleSettings.IR_Team == IR_TEAM_NONE) { IR_Tx.send(BattleSettings.IR_FireProtocol); }
            else
            {   // This is a team-specific protocol
            
                // FOV TEAMS
                if (BattleSettings.IR_FireProtocol == IR_FOV)
                {
                    switch (BattleSettings.IR_Team)
                    {   // Team 1 is free-for-all and would have been sent above because we set it to IR_TEAM_NONE
                        case IR_TEAM_FOV_2: IR_Tx.send(IR_FOV, FOV_TEAM_2_VALUE); break;
                        case IR_TEAM_FOV_3: IR_Tx.send(IR_FOV, FOV_TEAM_3_VALUE); break;
                        case IR_TEAM_FOV_4: IR_Tx.send(IR_FOV, FOV_TEAM_4_VALUE); break;
                    }
                }
                // CHECK SOME OTHER PROTOCOLS WITH TEAMS HERE 
                //else if (BattleSettings.IR_FireProtocol == SOME_OTHER_PROTOCOL)
                //{
                //} 
            }
        }
        // Done sending, re-enable reception
        EnableHitReception();
    }
}
void OP_Tank::Cannon_StartReload(void)
{
    // Start the reload timer. Further canon fire will not be possible until the timer completes. 
    CannonReloadComplete = false;
    TankTimer->setTimeout(BattleSettings.ClassSettings.reloadTime, ReloadComplete);    // Will call function "ReloadComplete" after the correct amount of time has passed for this weight class. 
}
void OP_Tank::ReloadComplete(void)
{
    CannonReloadComplete = true;
}
boolean OP_Tank::CannonReloaded(void)
{
    return CannonReloadComplete; 
}




//------------------------------------------------------------------------------------------------------------------------>>
// MUZZLE FLASH
//------------------------------------------------------------------------------------------------------------------------>>
void OP_Tank::TriggerMuzzleFlash(void)
{
    // This one is a PNP transistor, so logic HIGH = OFF, LOW = ON
    digitalWrite(pin_MuzzleFlash, LOW);
    TankTimer->setTimeout(MUZZLE_FLASH_TRIGGER_mS, ClearMuzzleFlash);
}
void OP_Tank::ClearMuzzleFlash(void)
{
    // Muzzle flash is over. Remember, for PNP HIGH = off
    digitalWrite(pin_MuzzleFlash, HIGH);
}




//------------------------------------------------------------------------------------------------------------------------>>
// RECEIVING HITS AND TAKING DAMAGE
//------------------------------------------------------------------------------------------------------------------------>>

// Returns the HIT_TYPE if the tank was hit
HIT_TYPE OP_Tank::WasHit(void)
{
// Initialize to false
boolean hit = false; 
boolean TwoShotHit = false;

    if (isInvulnerable || isDestroyed || IR_Enabled == false)
    {
        // The tank can't be hit if it is invulnerable, so don't even bother checking.
        // Same goes if the tank is already destroyed. 
        // Also if IR is disabled none of this matters. 
        return HIT_TYPE_NONE;
    }
    else
    {
        if (IR_Rx->GetResults(&IR_Decoder)) // If true, some IR signal was received 
        {   // Now we have to decode the signal, and see if it applies to us
            
            // For testing
                //IR_Decoder.decode(BattleSettings.IR_FireProtocol);
                //Serial.print(F("Decoded: ")); Serial.print(ptrIRName(IR_Decoder.decode_type)); Serial.print(F(" Value: ")); Serial.println(IR_Decoder.value);
                //IR_Decoder.DumpResults();
    
            // There are multiple types of IR signals that can be received: Cannon, Machine Gun, Repair
            // Clear these to start, they will get set as we proceed to whatever protocol/team hit us
            _lastHit = IR_UNKNOWN;
            _lastTeam = IR_TEAM_NONE;

            // CANNON 
            // The user can specify up to 2 protocols. Here we check the first one - which is the same protocol they use to fire with
            if (BattleSettings.IR_FireProtocol != IR_DISABLED)
            {
                // Were we hit with the primary IR protocol? 
                hit = IR_Decoder.decode(BattleSettings.IR_FireProtocol);
                // If so, save it to the _lastHit variable
                if (hit) _lastHit = BattleSettings.IR_FireProtocol;
                
                // If the FireProtocol is set to Tamiya 2-Shot and we were hit, we want to save that because damage will be different 
                if (hit && BattleSettings.IR_FireProtocol == IR_TAMIYA_2SHOT) { TwoShotHit = true; }
                // But even if we weren't hit, because they set it to the 2-shot protocol, we automatically check for regular 1/16 Tamiya code as well
                if (!hit && BattleSettings.IR_FireProtocol == IR_TAMIYA_2SHOT) { hit = IR_Decoder.decode(IR_TAMIYA); if (hit) { _lastHit = IR_TAMIYA; } }
                // Likewise, if the FireProtocol is set to Tamiya, automatically check for Tamiya 2-Shot kill code as well
                if (!hit && BattleSettings.IR_FireProtocol == IR_TAMIYA) { hit = TwoShotHit = IR_Decoder.decode(IR_TAMIYA_2SHOT); if (hit) { _lastHit = IR_TAMIYA_2SHOT; } }
            }
            // Now we also check the second one, but only if the first one didn't already return a hit, and if the second one is not set to null or the same as the first
            if (!hit && BattleSettings.IR_HitProtocol_2 != IR_DISABLED && BattleSettings.IR_HitProtocol_2 != BattleSettings.IR_FireProtocol)
            {
                // Were we hit with the secondary IR protocol?
                hit = IR_Decoder.decode(BattleSettings.IR_HitProtocol_2);
                // If so, save it to the _lastHit variable
                if (hit) _lastHit = BattleSettings.IR_HitProtocol_2;
                
                // Same deal here, if the user wants to check one Tamiya code, we automatically also check the other. 
                // If the HitProtocol_2 is set to Tamiya 2-Shot and we were hit, we want to save that because damage will be different 
                if (hit && BattleSettings.IR_HitProtocol_2 == IR_TAMIYA_2SHOT) { TwoShotHit = true; }
                // But even if we weren't hit, because they set it to the 2-shot protocol, we automatically check for regular 1/16 Tamiya code as well
                if (!hit && BattleSettings.IR_HitProtocol_2 == IR_TAMIYA_2SHOT) { hit = IR_Decoder.decode(IR_TAMIYA); if (hit) { _lastHit = IR_TAMIYA; } }
                // Likewise, if the HitProtocol_2 is set to Tamiya, automatically check for Tamiya 2-Shot kill code as well
                if (!hit && BattleSettings.IR_HitProtocol_2 == IR_TAMIYA) { hit = TwoShotHit = IR_Decoder.decode(IR_TAMIYA_2SHOT); if (hit) { _lastHit = IR_TAMIYA_2SHOT; } }
            }
            
            // If hit is true, we were hit with cannon fire. But some protocols implement teams and if we were hit with one of those we want to record
            // which team hit us. If we are on the same team ourselves, we will ignore the hit and take no damage. 
            if (hit)
            {   
                // FOV TEAMS
                if (_lastHit == IR_FOV)
                {   
                    // Save the team to _lastTeam variable. If the team that hit us is the same team that we're on, set hit = false
                    switch (IR_Decoder.value) 
                    {
                        case FOV_TEAM_1_VALUE: _lastTeam = IR_TEAM_NONE; break; // FOV Team 1 is considered "No team" and all teams take hits from it
                        case FOV_TEAM_2_VALUE: _lastTeam = IR_TEAM_FOV_2; if (BattleSettings.IR_Team == IR_TEAM_FOV_2) { hit = false; } break;
                        case FOV_TEAM_3_VALUE: _lastTeam = IR_TEAM_FOV_3; if (BattleSettings.IR_Team == IR_TEAM_FOV_3) { hit = false; } break;
                        case FOV_TEAM_4_VALUE: _lastTeam = IR_TEAM_FOV_4; if (BattleSettings.IR_Team == IR_TEAM_FOV_4) { hit = false; } break;
                    }
                }
            }

            
            // Ok, now if hit is still true we really were hit by cannon fire
            if (hit)
            {   
                DisableHitReception();      // Temporary invulnerability after being hit (mostly so we only proces the hit a single time)
                
                // What about if we were in the middle of being repaired? We need to cancel the repair and turn off the repair lights
                // to make way for the cannon hit lights. 
                if (RepairOngoing) CancelRepair(); 
                
                CannonHitsTaken += 1;       // Increment number of cannon hits taken
                
                // Increment our overall damage percent 
                if (TwoShotHit)
                {
                    DamagePct += 50;    // Two-shot hits increase damage by 50 percent each time
                }
                else
                {
                    DamagePct += DamagePctPerCannonHit; // Regular hits increase by the amount-per-cannon-hit
                }
                
                if (DamagePct >= 100.0)
                {
                    // Don't let damage go above 100%
                    DamagePct = 100.0;
                                        
                    // After destruction, the tank becomes inoperative for some period of time (15 seconds is the Tamiya spec - NOT the same as recovery/invulnerability time!)
                    // After that time it will automatically recover itself. During invulnerability time, the tank can fire but is impervious to enemy fire. 
                    // Invulnerabilty time is dependent on the weight class. 
                    isDestroyed = true;
                    TankTimer->setTimeout(DESTROYED_INOPERATIVE_TIME_mS, ResetBattle);   // DESTROYED_INOPERATIVE_TIME_mS is defined in OP_BattleTimes.h
                    // Start the destroyed light effect
                    HitLEDs_CannonHit();    // After the cannon hit effect, because isDestroyed is true, the subsequent HitLEDs_Destroyed effect will start automatically
                }
                else
                {
                    // Flash the hit notification LEDs
                    HitLEDs_CannonHit();
                    // Start a brief invulnerability timer. Each IR signal is sent multiple times, but we only want to count 
                    // one hit per shot. For the next second after being hit, we ignore further hits
                    TankTimer->setTimeout(HIT_FILTER_mS, EnableHitReception);
                }
                return HIT_TYPE_CANNON; // Return cannon hit type 
            }
            // If that didn't match, we may still have been hit, but by machine gun fire.
            // Check that, but only if the user has specified MG damange and an MG protocol
            else if (BattleSettings.Accept_MG_Damage && BattleSettings.IR_MGProtocol != IR_DISABLED && IR_Decoder.decode(BattleSettings.IR_MGProtocol))
            {
                // We were hit with a machine gun
                
                // Save the protocol to the _lastHit variable
                _lastHit = BattleSettings.IR_MGProtocol;

                // What about if we were in the middle of being repaired? We need to cancel the repair and turn off the repair lights
                // to make way for the MG hit lights. 
                if (RepairOngoing) CancelRepair(); 
                
                // Unlike cannon fire, we don't disable IR reception, because we allow multiple MG hits to occur in quick succession
                MGHitsTaken += 1;               // Increment number of machine gun hits taken
                DamagePct += DamagePctPerMGHit; // Increment our overall damage percent
                
                if (DamagePct >= 100.0)
                {
                    // Don't let damage go above 100%
                    DamagePct = 100.0;
                    
                    // After destruction, the tank becomes inoperative for some period of time (15 seconds is the Tamiya spec - NOT the same as recovery/invulnerability time!)
                    // After that time it will automatically recover itself. During invulnerability time, the tank can fire but is impervious to enemy fire. 
                    // Invulnerability time is dependent on the weight class. 
                    isDestroyed = true;
                    TankTimer->setTimeout(DESTROYED_INOPERATIVE_TIME_mS, ResetBattle);   // DESTROYED_INOPERATIVE_TIME_mS is defined in OP_BattleTimes.h
                    // Start the destroyed light effect directly
                    HitLEDs_Destroyed();    
                }
                else
                {
                    // Flash the hit notification LEDs, but this is a different pattern from when being hit by a cannon
                    HitLEDs_MGHit();
                    // Reenable hit reception immediately, we can take MG hits as fast as someone can send them. Even though we didn't call DisableHitReception()
                    // we still have to call EnableHitReception because the IR_Receiver automatically stops after being decoded. 
                    EnableHitReception();
                }
                return HIT_TYPE_MG; // Return MG hit type
            }
            // If that didn't match, we may still have been hit, but by a repair tank. 
            // Check but only if we haven't sustained any damage yet (otherwise there is no repair needed)
            // And also ignore it if we are already in the process of being repaired
            else if (DamagePct > 0.0 && !RepairOngoing && IR_Decoder.decode(BattleSettings.IR_RepairProtocol))
            {
                _lastHit = BattleSettings.IR_RepairProtocol;// Save the protocol to the _lastHit variable
                RepairOngoing = true;       // Set the repair flag
                Repair_BlinkHandler();      // Do the special repair light effect (start blinking slow and gradually increase faster and faster)
                // Reenable hit reception immediately. The point is that while being repaired, the tank is vulnerable. 
                EnableHitReception();
                // Start the repair timer. During this time we can not be repaired again, nor can we move (the move disabling is handled by the sketch)
                RepairTimerID = TankTimer->setTimeout(REPAIR_TIME_mS, RepairOver);   // REPAIR_TIME_mS is set in OP_BattleTimes.h
                // Return "hit" type
                return HIT_TYPE_REPAIR;
                // Note - we don't decrease the damage just yet. That only happens at the end of the repair operation, if the vehicle makes it that long
                // without being hit by the enemy. 
            }
            else
            {
                // We weren't hit. Re-enable reception
                EnableHitReception();
            }
        }
    }
    return HIT_TYPE_NONE;   // If we make it to here, we weren't hit
}

IRTYPES OP_Tank::LastHitProtocol(void)
{
    return _lastHit;    // What protocol was the last successful hit
}

IRTEAMS OP_Tank::LastHitTeam(void)
{
    return _lastTeam;   // Which team last hit us (if applicable)
}

void OP_Tank::CancelRepair(void)
{
    // We cancel a repair if we were hit by enemy fire in the middle of one. 
    // Health is not increased. 
    // We can just use our StopRepair function here. 
    StopRepair();
}

void OP_Tank::RepairOver(void)
{
    // Repair is over, we successfully made it through the whole 15 seconds. 
    RepairOngoing = false;

    // Now we do the opposite of taking damage.
    DamagePct -= DamagePctPerCannonHit;     // Subtract a cannon hit
    if (DamagePct < 0.0) DamagePct = 0.0;   // But don't go below zero      

    // Call the repair blink hander, it will turn off the lights. 
    Repair_BlinkHandler();
}

void OP_Tank::StopRepair(void)
{
    // Stop this timer from running
    if (TankTimer->isEnabled(RepairTimerID)) { TankTimer->deleteTimer(RepairTimerID); }
    
    if (RepairOngoing) 
    { 
        RepairOngoing = false;
        
        // Call the repair blink hander, it will turn off the lights. 
        Repair_BlinkHandler();
    }
}

boolean OP_Tank::isRepairOngoing()
{
    return RepairOngoing;
}

uint8_t OP_Tank::PctDamaged(void)
{
    return round(DamagePct);
}

uint8_t OP_Tank::PctHealthRemaining(void)
{
    return (100 - constrain(round(DamagePct),0,100));
}

void OP_Tank::DisableHitReception(void)
{
    isInvulnerable = true;      // The tank will now ignore hits
}
    
void OP_Tank::EnableHitReception(void)
{
    if (IR_Tx.isSendingDone()) 
    {
        IR_Decoder.Reset();         // Clear the decoder of anything that may have come in
        IR_Rx->resume();            // Resume IR reception
        isInvulnerable = false;     // We are now vulnerable to hits
    }
    else
    {
        // We want to enable reception, but we are also still in the middle of sending a signal out. 
        // Start a repeating timer that will keep checking back, and auto enable hit reception
        // when the transmission is done. 
        TankTimer->setTimeout(5, EnableHitReception);
    }
}


// Cut speed to motors (ie, "damage"). NOTE: Since this is the Standalone IR project, we are by definition not a vehicle with speed that can be reduced. 
//                                     Therefore, this damage function does nothing. 
/*
void OP_Tank::Damage()
{
  
int cut_Pct;

    switch (BattleSettings.DamageProfile)
    {
        case TAMIYA_DAMAGE:
            // For reference, see the package insert to Tamiya #53447 "Hop Up Options: Battle System".
            // When a Tamiya tank is hit, the drive motor speed is reduced. Tamiya follows a consistent formula based on the number of hits
            // taken compared to the max number of hits the given weight class can take before being destroyed. The formula is as follows: 
            // 1. Subtract 1 from the number of max hits (because the last hit destroys the tank, it doesn't damage it)
            // 2. Divide the remaining number of hits by 2. If an odd number, round up to the nearest integer. We will call this Halfway.
            // 3. If the number of hits taken so far is less than or equal to Halfway, reduce the speed of the drive motors to 50%. 
            // 4. If the number of hits taken is greater than Halfway, reduce the speed of the drive motors to 25%. 
            // 5. If the number of hits taken equals the max number of hits allowed, the tank is destroyed. 
            // Because this formula only needs to know the max number of hits allowed, we can easily apply it to any custom weight class the
            // user may create, as well as the standard Tamiya classes: 
            
            // MaxHits = BattleSettings.ClassSettings.maxHits;     // The max number of hits for this weight class
            // Halfway = round(float(MaxHits-1)/2.0);              // The Tamiya formula to calculate Halfway
            // if      (hitsTaken <= Halfway) { cut_Pct = 50;  }   // Reduce speed to 50%
            // else if (hitsTaken <  MaxHits) { cut_Pct = 75;  }   // Reduce speed to 25% (cut by 75%)
            // else                           { cut_Pct = 100; }   // Destroyed (cut by 100%)

            // EDIT: Well, that's the way Tamiya does it, but if you want to include machine gun damage, we need a slightly different approach. 
            // This should give us the same result as the Tamiya approach, but also allow damage due to machine gun fire. 
            if      (DamagePct <= 0.0)                          cut_Pct = 0;
            else if (DamagePct >  0.0 && DamagePct <= 50.0 )    cut_Pct = 50;
            else if (DamagePct > 50.0 && DamagePct <  100.0)    cut_Pct = 75;
            else                                                cut_Pct = 100;
                    
            // Serial.print(F("Speed cut to "));
            // Serial.print(100-cut_Pct);
            // Serial.println(F("%"));
            break;
          
        default:
            cut_Pct = 0; 
            break;
          
    }

}
*/

void OP_Tank::ResetBattle(void)
{
    // This function is called when the tank is "regenerating" or "recovering" after being destroyed (or when the TCB has just rebooted). 
    // During invulnerability time the tank is invulnerable to enemy fire for a length of time dependent on its class. 
    isDestroyed = false;        // We are no longer destroyed
    CannonHitsTaken = 0;        // Reset the hit counter
    MGHitsTaken = 0;
    DamagePct = 0;
    DisableHitReception();      // Ignore enemy fire
    TankTimer->setTimeout(BattleSettings.ClassSettings.recoveryTime, EnableHitReception);    // Enable hits after recovery (invulnerability) time has passed
}




//------------------------------------------------------------------------------------------------------------------------>>
// HIT NOTIFICATION LEDs 
//------------------------------------------------------------------------------------------------------------------------>>
void OP_Tank::HitLEDs_On(void)
{
    digitalWrite(pin_HitNotifyLEDs, HIGH);
    HitLEDsOn = true;
}
void OP_Tank::HitLEDs_Off(void)
{
    digitalWrite(pin_HitNotifyLEDs, LOW);
    HitLEDsOn = false;
}
void OP_Tank::HitLEDs_Toggle(void)
{
    HitLEDsOn ? HitLEDs_Off() : HitLEDs_On();
    HitLEDsOn = !HitLEDsOn;
}
void OP_Tank::HitLEDs_SetDim(uint8_t level)
{
    analogWrite(pin_HitNotifyLEDs, level);
    level > 0 ? HitLEDsOn = true : HitLEDsOn = false;
}
void OP_Tank::HitLEDs_Blink(long blinkTime)
{
    digitalWrite(pin_HitNotifyLEDs, HIGH);
    TankTimer->setTimeout(blinkTime, HitLEDs_Off);    
}

//------------------------------------------------------------------------------------------------------------------------>>
// HIT NOTIFICATION LEDs - CANNON HIT
//------------------------------------------------------------------------------------------------------------------------>>
// These flicker the LEDs that are typically installed in the IR "apple" to indicate damage received or tank destroyed. 
void OP_Tank::HitLEDs_CannonHit(void)
{   // The Cannon Hit effect randomly flickers the lights the same way Tamiya does. 

    // If we are still in the middle of running a flickering effect, just 
    // extend the time it runs. 
    if (TankTimer->isEnabled(HitLED_TimerID)) 
    { 
        // Delete the currently running timeout timer
        TankTimer->deleteTimer(HitLED_TimerID); 
        // Start a new one
        HitLED_TimerID = TankTimer->setTimeout(FLICKER_EFFECT_LENGTH_mS, CannonHitLEDs_Stop);
        // In the meanwhile, the flickering effect will continue to run
    }
    
    // In this case the previous effect is over, so start a new one
    if (!TankTimer->isEnabled(FadeStep_TimerID))
    {
        // We will set FadeOut = true later, when we are done with the effect and want to fade out
        FadeOut = false;

        // We start off at max brightness, 
        CurrentFadeLevel = 255;
        HitLEDs_On();
        
        // Pick some random brightness to fade down to
        FadeTarget = random(MIN_BRIGHT, DIM_FADE_BREAK);
        // Pick some random fade speed to get there
        FadeStep = -random(MIN_FADE_STEP, MAX_FADE_STEP);   // negative step value      
        // Start the fade timer, it will decrement the brightness by FadeStep every FADE_UPDATE_mS milliseconds until it reaches FadeTarget
        FadeStep_TimerID = TankTimer->setInterval(FADE_UPDATE_mS, CannonHitLEDs_Update);
        // Now start another one-shot timer to cancel the overall effect after FLICKER_EFFECT_LENGTH_mS milliseconds
        HitLED_TimerID = TankTimer->setTimeout(FLICKER_EFFECT_LENGTH_mS, CannonHitLEDs_Stop);
    }
}
void OP_Tank::CannonHitLEDs_Update(void)
{
static boolean StopNextTime = false; 

    // Increase or decrease the brightness level by FadeStep amount
    CurrentFadeLevel += FadeStep;
    // Set the LEDs to this brightness
    HitLEDs_SetDim(CurrentFadeLevel);
    
    // That's usually it, unless we've reached our target brightness. If so, we pick some
    // new random numbers and fade in the opposite direction
    if (FadeStep > 0 && CurrentFadeLevel >= FadeTarget)
    {
        // We reached the brightness target - now swap to some dim value
        if (FadeOut)
        {
            // This will be the last fade, and we will go slowly
            FadeTarget = 0;
            FadeStep = -4;  // Negative, because we are fading out. Small number so the fade takes time.  
            StopNextTime = true;        // This lets us know after we're done fading, that's it. 
        }
        else
        {   // This is a random fade
            FadeTarget = random(MIN_BRIGHT, DIM_FADE_BREAK);
            FadeStep = -random(MIN_FADE_STEP, MAX_FADE_STEP);   // negative step value
        }
    }
    else if (FadeStep < 0 && CurrentFadeLevel <= FadeTarget)
    {
        if (StopNextTime)
        {
            // We've reached the end of the last fade. Stop the timer and turn off the lights
            // (but they should already be off)
            if (TankTimer->isEnabled(FadeStep_TimerID))
            {
                TankTimer->deleteTimer(FadeStep_TimerID);
                HitLEDs_Off();
                CurrentFadeLevel = 0;
                // Reset
                StopNextTime = false;
                
                // BUT! If the tank is destroyed, we now start the HitLEDs_Destroyed effect
                if (isDestroyed) HitLEDs_Destroyed();
            }
        }
        else
        {
            // We reached our random dim target - now swap to some random bright value
            FadeTarget = random(BRIGHT_FADE_BREAK, MAX_BRIGHT);
            FadeStep = random(MIN_FADE_STEP, MAX_FADE_STEP);   // positive value
        }
    }
}
void OP_Tank::CannonHitLEDs_Stop(void)
{
    if (TankTimer->isEnabled(FadeStep_TimerID))
    {
        FadeOut = true;
    }
}

//------------------------------------------------------------------------------------------------------------------------>>
// HIT NOTIFICATION LEDs - MACHINE GUN HIT
//------------------------------------------------------------------------------------------------------------------------>>
void OP_Tank::HitLEDs_MGHit(void)
{
    // If we're hit by machine gun fire, we do a simple effect - two short blinks
    // But because we can, we make a nice little routine that lets us adjust precisely the
    // two blinks and the space between them. In fact with this approach you could create any 
    // sequence of blinks and spaces of any length of time.
    
    static uint8_t curStep = 0;
    const uint8_t steps = 3;
    const uint8_t time[steps] = {100,60,40};
  
    if (curStep < steps)
    {
        // Odd numbers get turned off
        if (curStep & 1)    
        {
            HitLEDs_Off();
            // Come back to this routine after the specified time for this step is up
            TankTimer->setTimeout(time[curStep], HitLEDs_MGHit);
        } 
        // Even numbers get turned on
        else        
        {
            HitLEDs_On();
            // Come back to this routine after the specified time for this step is up
            TankTimer->setTimeout(time[curStep], HitLEDs_MGHit);
        }
        curStep += 1;
    }
    else
    {
        // We're done, turn off the lights and don't come back
        HitLEDs_Off();
        curStep = 0;    // reset
    }
    
}


//------------------------------------------------------------------------------------------------------------------------>>
// HIT NOTIFICATION LEDs - DESTROYED
//------------------------------------------------------------------------------------------------------------------------>>
void OP_Tank::HitLEDs_Destroyed()
{
    static int DestroyedBlinkerID = 0;
    static boolean started = false;
    static int16_t fadeLevel = 255; // This needs to be a signed two-byte integer
    #define SLOW_FADE_OUT_STEP 2
    
    if (!started && isDestroyed)
    {   // In this case, we are just starting at the beginning of being destroyed.
    
        // Shoudn't need to, but delete the timer if it already exists
        if (TankTimer->isEnabled(DestroyedBlinkerID)) { TankTimer->deleteTimer(DestroyedBlinkerID); } 
        
        // Turn the lights on
        HitLEDs_On();   
        
        // Now set a timer to keep coming back here after a short interval so we can blink the lights
        DestroyedBlinkerID = TankTimer->setInterval(450, HitLEDs_Destroyed);     // This is a slow blink, about half a second
        
        // The effect has been started
        started = true;
    }
    else if (started && isDestroyed)
    {   // In this case, we already started the effect and we are coming back here just to blink the lights.  
        HitLEDs_Toggle();
    }
    else if (started && !isDestroyed)
    {   // Ok, the tank is done being destroyed. What we do now depends on whether the light is currently on or off
        if (HitLEDsOn)
        {
            // In this case, we can start fading out slowly
            TankTimer->deleteTimer(DestroyedBlinkerID);
            // Start a new timer that will call us much more frequently
            DestroyedBlinkerID = TankTimer->setInterval(FADE_UPDATE_mS, HitLEDs_Destroyed);
            started = false; // Change this to false
        }
        else
        {
            // We need to wait for the light to be on before we can fade off, so do one more toggle
            HitLEDs_Toggle();
        }
    }
    else if (!started && !isDestroyed)
    {
        // Now we are in the final fade-out phase
        fadeLevel -= SLOW_FADE_OUT_STEP;
        if (fadeLevel > 0)
        {   
            HitLEDs_SetDim(fadeLevel); // Dim the light
        }
        else
        {
            // We're completely done
            TankTimer->deleteTimer(DestroyedBlinkerID);
            HitLEDs_Off();
            fadeLevel = 255;    // reset for next time
        }
    }
}


//------------------------------------------------------------------------------------------------------------------------>>
// HIT NOTIFICATION LEDs REPAIR EFFECT - used both when firing repair code, and receiving repair code
//------------------------------------------------------------------------------------------------------------------------>>
void OP_Tank::Repair_BlinkHandler(void)
{   // Only start/continue the blinking effect if we are in the midst of being repaired
    if (RepairOngoing) 
    {
        HitLEDs_Repair(); 
    }
    else 
    {   // Stop the repair blinking effect
        if (TankTimer->isEnabled(HitLED_TimerID)) TankTimer->deleteTimer(HitLED_TimerID);
        HitLEDs_Off();
    }
}

void OP_Tank::HitLEDs_Repair(void)
{
    static boolean LightState = false;
    
    static boolean effectStarted = false;
    const int StartInterval = 500;   
    const int InitialSubtract = 43; 
    const int Knee1 = 36;
    const int Knee2 = 18;
    const int Knee3 = 1;
    const uint8_t SC_1 = 4;
    const uint8_t SC_2 = 2;
    const uint8_t SC_3 = 1;
    static int Interval;
    static int Subtract;

    if (!effectStarted) 
    {
        Interval = StartInterval;
        Subtract = InitialSubtract;
        effectStarted = true;
    }
    else
    {
        if      (Subtract > Knee1) Subtract -= SC_1;
        else if (Subtract > Knee2) Subtract -= SC_2;
        else if (Subtract > Knee3) Subtract -= SC_3;
        if      (Subtract < 0)     Subtract = 0;
    }

    // Toggle the light state
    LightState ? HitLEDs_Off() : HitLEDs_On();    
    LightState = !LightState;      // Flop states

    if (Interval <= 0)
    {
        effectStarted = false;
        HitLEDs_Off();
        // Effect is done, go back to Repair_BlinkHandler. It will restart the blinking effect if appropriate. 
        HitLED_TimerID = TankTimer->setTimeout(250,Repair_BlinkHandler);
    }
    else
    {
        // Now set a timer to come back here and toggle it after the correct length of time has passed.
        HitLED_TimerID = TankTimer->setTimeout(Interval, HitLEDs_Repair);
    }

    // For next time
    Interval -= Subtract;    
}


//------------------------------------------------------------------------------------------------------------------------>>
// HIT NOTIFICATION LEDs CANON RELOADED - Short blink to notify  user the canon reload time has completed
//                                        Can be enabled/disabled with setting CANNON_RELOAD_NOTIFY on the A_Setup.h tab
//------------------------------------------------------------------------------------------------------------------------>>
void OP_Tank::HitLEDs_ReloadNotify(void)
{   
    HitLEDs_Blink(250);
}
