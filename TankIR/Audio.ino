
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------>>
// Adafruit Audio FX board triggers
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------>>
// Adafruit offers several Audio FX boards with different amounts of on-board memory and outputs. Unless you are using an external amplifier, you probably want the ones
// with a built-in amp
// Adafruit Audio FX Sound Board + 2x2W Amp - 16MB capacity: https://www.adafruit.com/product/2217
// Adafruit Audio FX Sound Board + 2x2W Amp - 2MB  capacity: https://www.adafruit.com/product/2210
// According to documentation pin must be held to ground for approximately 125 mS
#define LENGTH_ADAFRUIT_FX_HELD_TO_GROUND   200                                 // 200 mS should be plenty of time
// Cannon fire sound
void TriggerCannonSound(void)
{
    digitalWrite(pin_FIRE_CANNON_TRIGGER, LOW);                                 // Set pin to ground
    timer.setTimeout(LENGTH_ADAFRUIT_FX_HELD_TO_GROUND, ClearCannonSoundPin);   // Keep it to ground briefly
}
void ClearCannonSoundPin(void)
{
    digitalWrite(pin_FIRE_CANNON_TRIGGER, HIGH);                                 // Trigger is done, set pin back to high
}
// Hit received sound
void TriggerHitReceivedSound(void)
{
    digitalWrite(pin_RECEIVE_HIT_TRIGGER, LOW);                                 // Set pin to ground
    timer.setTimeout(LENGTH_ADAFRUIT_FX_HELD_TO_GROUND, ClearHitReceivedSoundPin);   // Keep it to ground briefly
}
void ClearHitReceivedSoundPin(void)
{
    digitalWrite(pin_RECEIVE_HIT_TRIGGER, HIGH);                                 // Trigger is done, set pin back to high
}
// Vehicle destroyed sound
void TriggerDesroyedSound(void)
{
    digitalWrite(pin_VEHICLE_DESTROYED_TRIGGER, LOW);                           // Set pin to ground
    timer.setTimeout(LENGTH_ADAFRUIT_FX_HELD_TO_GROUND, ClearDestroyedSoundPin); // Keep it to ground briefly
}
void ClearDestroyedSoundPin(void)
{
    digitalWrite(pin_VEHICLE_DESTROYED_TRIGGER, HIGH);                          // Trigger is done, set pin back to high
}
// Repair sound
void TriggerRepairSound(void)
{
    digitalWrite(pin_VEHICLE_REPAIR_TRIGGER, LOW);                              // Set pin to ground
    timer.setTimeout(LENGTH_ADAFRUIT_FX_HELD_TO_GROUND, ClearRepairSoundPin);   // Keep it to ground briefly
}
void ClearRepairSoundPin(void)
{
    digitalWrite(pin_VEHICLE_REPAIR_TRIGGER, HIGH);                             // Trigger is done, set pin back to high
}


