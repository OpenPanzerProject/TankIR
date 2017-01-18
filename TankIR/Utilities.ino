// mapf - Float version of the "map" function
float mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}



void PrintDebugLine()
{
    for (uint8_t i=0; i<45; i++) { Serial.print(F("-")); }
    Serial.println(); 
    Serial.flush();   // This causes a pause until the serial transmission is complete
}

void PrintSpaceDash()
{
    Serial.print(F(" - "));
}

void PrintSpaceBar()
{
    Serial.print(F(" | "));
}

void PrintSpace()
{
    Serial.print(F(" "));
}    

void PrintSpaces(uint8_t num)
{
    if (num == 0) return;
    for (uint8_t i=0; i<num; i++) { PrintSpace(); }
}

void PrintLine()
{
    Serial.println();
}

void PrintLines(uint8_t num)
{    
    if (num == 0) return;
    for (uint8_t i=0; i<num; i++) { PrintLine(); }
}

void PrintTrueFalse(boolean boolVal)
{
    if (boolVal == true) { Serial.print(F("TRUE")); } else { Serial.print(F("FALSE")); }
}

void PrintLnTrueFalse(boolean boolVal)
{
    PrintTrueFalse(boolVal);
    Serial.println();
}

void PrintYesNo(boolean boolVal)
{
    if (boolVal == true) { Serial.print(F("Yes")); } else { Serial.print(F("No")); }
}

void PrintLnYesNo(boolean boolVal)
{
    PrintYesNo(boolVal);
    Serial.println();
}

void PrintHighLow(boolean boolVal)
{
    if (boolVal == true) { Serial.println(F("HIGH")); } else { Serial.println(F("LOW")); }
}

void PrintPct(uint8_t pct)
{
    Serial.print(pct);
    Serial.print(F("%"));
}
void PrintLnPct(uint8_t pct)
{
    PrintPct(pct);
    Serial.println();
}

float Convert_mS_to_Sec(int mS)
{
    return float(mS) / 1000.0;
}

void DumpBattleInfo()
{
    Serial.println();
    PrintDebugLine();
    Serial.println(F("BATTLE INFO"));
    PrintDebugLine();
    if (Tank.BattleSettings.IR_FireProtocol != IR_UNKNOWN)
    {
    Serial.print(F("Is Repair Tank?   ")); PrintLnYesNo(Tank.isRepairTank());
    if (Tank.isRepairTank()) { Serial.print(F("Fire Protocol:    ")); Serial.println(ptrIRName(Tank.BattleSettings.IR_RepairProtocol)); }
    else
    {
        Serial.print(F("Fire Protocol:    ")); Serial.print(ptrIRName(Tank.BattleSettings.IR_FireProtocol));
        if (Tank.BattleSettings.IR_Team != IR_TEAM_NONE) { Serial.print(F(" (Team ")); Serial.print(ptrIRTeam(Tank.BattleSettings.IR_Team)); Serial.print(F(")")); } Serial.println();
    }
    Serial.print(F("Hit Protocol 2:   ")); 
    if (Tank.BattleSettings.IR_HitProtocol_2 != IR_UNKNOWN ) { Serial.println(ptrIRName(Tank.BattleSettings.IR_HitProtocol_2)); } else { Serial.println(F("N/A")); }
    Serial.print(F("Repaired by:      ")); 
    if (Tank.BattleSettings.IR_RepairProtocol != IR_UNKNOWN) { Serial.println(ptrIRName(Tank.BattleSettings.IR_RepairProtocol)); } else { Serial.println(F("N/A")); }
    Serial.print(F("Send MG IR Code:  ")); 
    if (Tank.BattleSettings.Use_MG_Protocol) { Serial.print(F("Yes (")); Serial.print(ptrIRName(Tank.BattleSettings.IR_MGProtocol)); Serial.println(")"); }
    else PrintLnYesNo(false);
    Serial.print(F("Accept MG Damage: ")); 
    if (Tank.BattleSettings.Accept_MG_Damage) { Serial.print(F("Yes (")); Serial.print(ptrIRName(Tank.BattleSettings.IR_MGProtocol)); Serial.println(")"); }
    else PrintLnYesNo(false);
    
    Serial.print(F("Damage Profile:   ")); Serial.println(ptrDamageProfile(Tank.BattleSettings.DamageProfile));
    Serial.print(F("Weight Class:     ")); Serial.println(ptrWeightClassName(Tank.BattleSettings.WeightClass)); 
    Serial.print(F("(")); Serial.print(Tank.BattleSettings.ClassSettings.maxHits); Serial.print(F(" cannon hits, ")); if (Tank.BattleSettings.WeightClass == WC_CUSTOM) { Serial.print(Tank.BattleSettings.ClassSettings.maxMGHits); Serial.print(F(" MG hits, ")); } Serial.print(Convert_mS_to_Sec(Tank.BattleSettings.ClassSettings.reloadTime),1); Serial.print(F(" sec reload, ")); Serial.print(Convert_mS_to_Sec(Tank.BattleSettings.ClassSettings.recoveryTime),1); Serial.println(F(" sec recovery)"));    
    }
    else
    {
    Serial.println(F("IR & Tank Battling Disabled"));
    }

    Serial.println();
    Serial.println();
    Serial.println();
}




