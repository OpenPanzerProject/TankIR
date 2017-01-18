/* OP_SimpleTimer.h     Open Panzer Simple Timer library - for handling timed events without using delays
 * Source:              openpanzer.org              
 * Authors:             Marcello Romani, Luke Middleton
 * 
 * This library is a modification of the Simple Timer timer library written by Marcello Romani.
 * Timer events now return a unique ID. Even though timer "slots" are constantly being
 * reused, timer IDs are always unique. This prevents routines inadvertently deleting timers associated
 * with other routines, which was a problem with the orignal code. 
 * 
 * The library has also been re-named to OP_SimpleTimer to avoid conflicts with other libraries. 
 *
 * The rest of the library remains as written by Marcello Romani. 
 * For the Arduino page on his original version, see: http://playground.arduino.cc/Code/SimpleTimer
 * 
 */ 


#ifndef OP_SIMPLETIMER_H
#define OP_SIMPLETIMER_H

#include <Arduino.h>
#include "Settings.h"

typedef void (*timer_callback)(void);

class OP_SimpleTimer {

public:
    // maximum number of timers
    const static int MAX_TIMERS = MAX_SIMPLETIMER_SLOTS;    // See Settings.h under the SIMPER TIMER heading for the definition of MAX_SIMPLETIMER_SLOTS and how it was calculated. 

    // setTimer() constants
    const static int RUN_FOREVER = 0;
    const static int RUN_ONCE = 1;

    // constructor
    OP_SimpleTimer();

    // this function must be called inside loop()
    void run();

    // call function f every d milliseconds
    int setInterval(long d, timer_callback f);

    // call function f once after d milliseconds
    int setTimeout(long d, timer_callback f);

    // call function f every d milliseconds for n times
    int setTimer(long d, timer_callback f, int n);

    // destroy the specified timer
    void deleteTimer(int ID);

    // restart the specified timer
    void restartTimer(int ID);

    // returns true if the specified timer is enabled
    boolean isEnabled(int ID);

    // enables the specified timer
    void enable(int ID);

    // disables the specified timer
    void disable(int ID);

    // enables the specified timer if it's currently disabled,
    // and vice-versa
    void toggle(int ID);

    // returns the number of used timers
    int getNumTimers();

    // returns the number of available timers
    int getNumAvailableTimers() { return MAX_TIMERS - numTimers; };
    
    // Gets the timer number (0-MAX_TIMERS) by ID
    int getTimerNum(int ID);

private:
    // deferred call constants
    const static int DEFCALL_DONTRUN = 0;       // don't call the callback function
    const static int DEFCALL_RUNONLY = 1;       // call the callback function but don't delete the timer
    const static int DEFCALL_RUNANDDEL = 2;     // call the callback function and delete the timer

    // find the first available slot
    int findFirstFreeSlot();

    // value returned by the millis() function
    // in the previous run() call
    unsigned long prev_millis[MAX_TIMERS];

    // pointers to the callback functions
    timer_callback callbacks[MAX_TIMERS];

    // delay values
    long delays[MAX_TIMERS];

    // number of runs to be executed for each timer
    int maxNumRuns[MAX_TIMERS];

    // number of executed runs for each timer
    int numRuns[MAX_TIMERS];

    // which timers are enabled
    boolean enabled[MAX_TIMERS];

    // deferred function call (sort of) - N.B.: this array is only used in run()
    int toBeCalled[MAX_TIMERS];

    // IDs for each timer (not equal to the timer number)
    int timerID[MAX_TIMERS];
    int NextID; 

    // actual number of timers in use
    int numTimers;
};

#endif
