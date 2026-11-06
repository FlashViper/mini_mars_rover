#ifndef ROVER_LASER
#define ROVER_LASER

#include "xc.h"
#include "constants.h"

void initialize_laser();
void enable_laser();
void disable_laser();


void initialize_laser(){
    // configure laser on pin 6 (B2)
    _ANSB2 = DIGITAL;
    _TRISB2 = OUTPUT;

    // disable the laser by default
    disable_laser();
}

void enable_laser(){
    _LATB2 = ENABLED;
}

void disable_laser(){
    _LATB2 = DISABLED;
}

#endif