
//============================================================================================================================================================================================>>
// USER SETTINGS
//============================================================================================================================================================================================>>

// THIS IS WHERE YOU SET UP THE DEVICE TO YOUR LIKING. 


    // RECOIL SERVO
    // --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------->>    
    
    #define RECOIL_MS               200                     // << --- SET ME - Actual time your servo takes to recoil at full speed (check servo manufacturer specs)
    #define RETURN_MS               1000                    // << --- SET ME - Time in milliseconds you want the recoil servo to take to return
    #define REVERSE_RECOIL          false                   // << --- SET ME - Use to reverse the direction of the recoil servo
    #define RECOIL_SERVO_EP_MIN     1000                    // << --- SET ME - You can adjust the recoil servo's end-points, 1000-2000 are normal values. 
    #define RECOIL_SERVO_EP_MAX     2000                    // << --- SET ME - Don't exceed anything sane (~800 - ~2200)


    // NOTIFICATION LED
    // --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------->>
    // 
    #define CANNON_RELOAD_NOTIFY    false                   // << --- SET ME - set to True to blink the IR apple notification LEDs when canon reload time has transpired (CUSTOM_CANNON_RELOAD below)
    
    
    
    // IR TYPE - FIGHT OR REPAIR
    // --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------->>

    #define REPAIR_TANK             false                   // << --- SET ME - false means the cannon IR will be sent, true means repair IR will be sent
    #define REPAIR_ON_HIT           true                    // << --- SET ME - If REPAIR_TANK = true, setting this to true will cause a repair signal to be fired each time an incoming hit is received.
                                                            //                 Useful for static repair emplacements, the damaged tank simply approaches, fires a hit at the emplacement, and the emplacement
                                                            //                 responds with the repair signal
 
    // IR PROTOCOL - OUTGOING (what protocol do we fire)
    // --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------->>
    
    #define IR_FIRE_PROTOCOL    IR_TAMIYA                   // << --- SET ME
                                
                                // Options are: 
                                //      IR_TAMIYA           // Standard Tamiya protocol
                                //      IR_TAMIYA_2SHOT     // Tamiya 2-shot kill code
                                //      IR_TAMIYA_35        // Tamiya 1/35 scale IR
                                //      IR_HENGLONG         // Heng Long
                                //      IR_TAIGEN_V1        // Original Taigen V1 motherboards
                                //      IR_TAIGEN           // For Taigen V2 and V3 motherboards
                                //      IR_FOV              // Forces of Valor 1/24 scale tanks, no longer being sold. Taigen is going to re-release them but we still don't know if they will use the same IR or not. 
                                //      IR_VSTANK           // VsTank 1/24 scale protocol
                                //      IR_OPENPANZER       // Not yet implemented. For future custom IR codes.


    // IR PROTOCOL - INCOMING (alternate)
    // --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------->>
    // We will always take hits from the same IR protocol specified as the outgoing protocol (above). 
    // But we can also enable hits from a second alternate protocol as well. Options are same as above. Use IR_DISABLED if you don't want to use a secondary protocol
    
    #define IR_HIT_PROTOCOL_ALT IR_HENGLONG                 // << --- SET ME



    // IR PROTOCOL - REPAIR
    // --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------->>

    #define IR_REPAIR_PROTOCOL  IR_RPR_CLARK                // << --- SET ME
    
                                // Options are:
                                //      IR_RPR_CLARK        // Repair signal: Clark TK-20, 22, and 60 repair protocol
                                //      IR_RPR_IBU          // Repair signal: Italian Battle Unit
                                //      IR_RPR_RCTA         // Repair signal: RC Tanks Australia. Theoretically this is the same as IBU, but untested. 



    // IR PROTOCOL - MACHINE GUN
    // --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------->>

    #define IR_MG_PROTOCOL      IR_MG_CLARK                 // << --- SET ME
    #define MG_DAMAGE           false                       // << --- SET ME - Whether or not this device will accept machine gun damage
    
                                // Options are:
                                //      IR_MG_CLARK         // Machine gun: Clark protocol (Sony)
                                //      IR_MG_RCTA          // Machine gun: RCTA protocol


    
    // TEAM SELECTION
    // --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------->>
    // For now teams only apply if you select the FOV protocol above. Otherwise set this to IR_TEAM_NONE
    #define IR_TEAM             IR_TEAM_NONE                // << --- SET ME
    
                                // Options are:
                                //      IR_TEAM_NONE        // No team
                                //      IR_TEAM_FOV_1       // FOV Team 1 we treat as TEAM_NONE. We can use IR_TEAM_FOV_1 in code but it will equal IR_TEAM_NONE
                                //      IR_TEAM_FOV_2              
                                //      IR_TEAM_FOV_3          
                                //      IR_TEAM_FOV_4          



    // WEIGHT CLASS
    // --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------->>
    #define WEIGHT_CLASS        WC_MEDIUM                   // << --- SET ME
    
                                // Options are:     
                                //      WC_LIGHT            // Tamiya lightweight spec
                                //      WC_MEDIUM           // Tamiya medium spec
                                //      WC_HEAVY            // Tamiya heavy spec
                                //      WC_CUSTOM           // Custom specification (see below)

    
    // If you intend to use a custom weight class (setting WC_CUSTOM above), you further need to define the four settings of it here
    
    #define CUSTOM_CANNON_RELOAD    1500                    // << --- SET ME - Cannon reload time in mS
    #define CUSTOM_RECOVERY_TIME    8000                    // << --- SET ME - How long does recovery mode last (invulnerability time when tank is regenerating after being destroyed). 
    #define CUSTOM_CANNON_HITS         1                    // << --- SET ME - How many cannon hits can the tank sustain before being destroyed. 
    #define CUSTOM_MG_HITS            20                    // << --- SET ME - How many machine gun hits can the tank sustain before being destroyed. 

        

    // DAMAGE PROFILE
    // --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------->>

    #define DAMAGE_PROFILE      TAMIYA_DAMAGE               // << --- SET ME
    
                                // Options are:     
                                //      TAMIYA_DAMAGE       // Stock Tamiya damage speed reduction profile
                                //      OPENPANZER_DAMAGE   // Open Panzer damage profile (experimental)



    // TANK ID
    // --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------->>
    // These features are not implemented yet. Leave them to false and 0 for now
    #define SEND_ID             false                       // << --- SET ME
    #define TANK_ID             0                           // << --- SET ME




    
