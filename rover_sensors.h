#ifndef ROVER_SENSORS
#define ROVER_SENSORS

#include "xc.h"
#include "constants.h"

#define CHANGE_NOTIFICATION _CNInterrupt(void)
#define SENSOR_IR_L _TRISA0 // TO DO: UPDATE WITH ACTUAL PIN
#define SENSOR_IR_R _TRISA1 // TO DO: DITTO
//...

static unsigned short sensor_distance_left = 0;
static unsigned short sensor_distance_forward = 0;
static unsigned short sensor_distance_right = 0;

static unsigned short sensor_line_left = 0;
static unsigned short sensor_line_center = 0;
static unsigned short sensor_line_right = 0;

void update_line_sensors();

void initialize_rover_sensors(){
    // set up qrd sensors to fire the change notification interrupt whenever
    // one of them flips from high to low
    // (see page 166 of packet I pretty much copied it word for word)
    
    // Initialize color sensor (left)
    _ANSB15 = DIGITAL;
    _TRISB15 = INPUT; 
    _CN11IE = ENABLED; // enable change interrupt on pin 18 (Change notification 11)
    _CN11PUE = DISABLED; // disable pull up resistor
    _CN11PDE = DISABLED; // disable pull down resistor
    
    _ANSB14 = DIGITAL;
    _TRISB14 = INPUT; 
    _CN12IE = ENABLED; // enable change interrupt on pin 17 (Change notification 12)
    _CN12PUE = DISABLED; // disable pull up resistor
    _CN12PDE = DISABLED; // disable pull down resistor (we already have one)

    _ANSB13 = DIGITAL;
    _TRISB13 = INPUT;
    _CN13IE = ENABLED;
    _CN13PUE = DISABLED;
    _CN13PDE = DISABLED;
    
    // Enable change notification interrupts
    _CNIE = ENABLED;
    _CNIP = 6; // set priority
    _CNIF = 0; // clear interrupt flag
    
    // Initialize IR Sensor (left)
    // Initialize IR Sensor (right)
    // etc

    update_line_sensors();

    // initialize pin change interrupts
}

void update_line_sensors(){
    // QRD sensors
    // invert because high voltage means it's over a dark surface
    // (condition) ? <true value> : <false value> is a shortcut for using if statements
    sensor_line_left = _RB15 ? FALSE : TRUE; 
    sensor_line_right = _RB14 ? FALSE : TRUE;
    sensor_line_center = _RB13 ? FALSE : TRUE;
    // sensor_line_center = _RB13 ? FALSE : TRUE;
}

unsigned short is_on_line() {
    return sensor_line_center || sensor_line_left || sensor_line_right;
}

// INTERRUPTS -------------------------------------------------

// see pages 165 and 166 of packet
INTERRUPT CHANGE_NOTIFICATION {
    _CNIF = 0; // clear interrupt flag
    update_line_sensors();
}

#endif