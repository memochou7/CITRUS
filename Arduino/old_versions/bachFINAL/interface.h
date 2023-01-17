#ifndef __INTERFACE_H__
#define __INTERFACE_H__
#include "rgb_lcd.h"
// Not sure what to do with below functions
// Is the formula going to be converted into CPP?
// Is his data getting streamed into excel?
// Or is his data returned from a function
// And how is the user interfacing w it?
extern bool cncNum;
extern bool warning; //=true when limits are reached
class interface : public rgb_lcd {
    char column = 66; // is "B" or "C" depending on select, in reference to excel column
    bool enabled = false; //off till on
    bool update; //can be true thru timedupdate and external edit
  public:
    
    void obtainConfig(unsigned long limitConfig[6], bool overrideTrigger);
    void updateRuntimes(unsigned long water, unsigned long coolant);
    void updateCurrents(int concentration, int coolant);

    void startDisplay(int lcdpower, bool enableDisplay);
    void clearDisplay();
    void statusDisplay(unsigned long limitConfig[6]);
    void runTimeDisplay(int minutesLeft, int secondsLeft);

    void store(int address, int val);
    int  retrieve(int address);

    void timedUpdateChecks(bool force, unsigned int duration);
    void updateNow();


};
#endif