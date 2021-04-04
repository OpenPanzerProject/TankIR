
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------>>
// CANNON FIRE
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------>>
// Pin change interrupt service routine for pins A0 - A5
// We use this to detect a positive voltage on pin_VoltageTrigger (A0) and if we do, fire the cannon. 
ISR (PCINT1_vect) 
{
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis(); 
    
    // Check if the pin is high, and if it has been more than some minimum length of time since the last interrupt
    if (digitalRead(pin_VoltageTrigger) == HIGH &&  (interrupt_time - last_interrupt_time > 250))   // 250 mS = 1/4 second
    {  
        // Pin went high - fire the cannon
        FireCannon();
    }
    last_interrupt_time = interrupt_time;
}  



// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------>>
// CANNON FIRE
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------>>
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
                TriggerRepairSound();
            }
        }
        else
        {
            // This is a fighting tank. But we can't fire the cannon if we're in the midst of being repaired by another tank.
            if (!Tank.isRepairOngoing())
            {
                Serial.println(F("Fire Cannon"));  
                Tank.Fire(); // See OP_Tank library. This starts the servo recoil, triggers the high intensity flash unit, and it sends the IR signal
                TriggerCannonSound();
            }
        }
    }
}



