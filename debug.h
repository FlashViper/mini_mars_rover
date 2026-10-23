#ifndef ROVER_DEBUG
#define ROVER_DEBUG

#include "xc.h"
#include "constants.h"

#define DEBUG_LED _LATB14
#define DEBUG_BUTTON_PRESSED PORTAbits.RA2
unsigned int debug_led_enabled = FALSE;

void initialize_debug(){
    _ANSB14 = DIGITAL;
    _TRISB14 = OUTPUT;

    _ANSA2 = DIGITAL;
    _TRISA2 = INPUT;
}

void enable_debug_led(){
    debug_led_enabled = TRUE;
    DEBUG_LED = debug_led_enabled;
}

void disable_debug_led(){
    debug_led_enabled = FALSE;
    DEBUG_LED = debug_led_enabled;
}

#endif