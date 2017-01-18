
// ONBOARD LED LIGHTS (Green and Red)
// -------------------------------------------------------------------------------------------------------------------------------------------------->   
void LED(int WhatLed, byte Status)
{
    digitalWrite(WhatLed, Status);
}

void RedLedOn()
{
    digitalWrite(pin_RedLED, HIGH);
}
void RedLedOff()
{
    digitalWrite(pin_RedLED, LOW);
}
void GreenLedOn()
{
    digitalWrite(pin_GreenLED, HIGH);
}
void GreenLedOff()
{
    digitalWrite(pin_GreenLED, LOW);
}

void GreenBlink()
{
    static boolean GreenState; 
    GreenState ? GreenLedOn() : GreenLedOff();
    GreenState = !GreenState;
}

void RedBlink()
{
    static boolean RedState; 
    RedState ? RedLedOn() : RedLedOff();
    RedState = !RedState;
}

void GreenBlinkSlow(int HowMany)
{   // This uses Delays and blocks all code!
    for (int i=1; i<=HowMany; i++)
    {
        GreenLedOn();
        delay(750);
        GreenLedOff();
        if (i < HowMany)
        { delay(500);  }
    }
}

void RedBlinkSlow(int HowMany)
{   // This uses Delays and blocks all code!
    for (int i=1; i<=HowMany; i++)
    {
        RedLedOn();
        delay(750);
        RedLedOff();
        if (i < HowMany)
        { delay(500);  }
    }
}

void BothBlinkSlow(int HowMany)
{   // This uses Delays and blocks all code!
    for (int i=1; i<=HowMany; i++)
    {
        GreenLedOn();
        RedLedOn();
        delay(750);
        GreenLedOff();
        RedLedOff();
        if (i < HowMany)
        { delay(500);  }
    }
}

void RedBlinkFast(int HowMany)
{   // This uses Delays and blocks all code!
    for (int i=1; i<=HowMany; i++)
    {
        RedLedOn();
        delay(100);
        RedLedOff();
        if (i < HowMany)
        { delay(90); }
    }
}

void GreenBlinkFast(int HowMany)
{   // This uses Delays and blocks all code!
    for (int i=1; i<=HowMany; i++)
    {
        GreenLedOn();
        delay(100);
        GreenLedOff();
        if (i < HowMany)
        { delay(90); }
    }
}

void GreenBlinkOne()
{ GreenBlinkFast(1); }

void GreenBlinkTwo()
{ GreenBlinkFast(2); }

void GreenBlinkThree()
{ GreenBlinkFast(3); }

void RedBlinkOne()
{ RedBlinkFast(1); }

void RedBlinkTwo()
{ RedBlinkFast(2); }

void RedBlinkThree()
{ RedBlinkFast(3); }


