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

void event_ir_right_changed();
void event_ir_left_detected();

void initialize_rover_sensors(){
    // set up qrd sensors to use change notifications
    // (see page 166 of packet I pretty much copied it word for word)
    
    // Initialize color sensor (left)
    _ANSB15 = DIGITAL;
    _TRISB15 = INPUT; 
    _CN11IE = ENABLED; // enable interrupt on pin 18 (Change notification 11)
    _CN11PUE = DISABLED; // disable pull up resistor
    _CN11PDE = DISABLED; // disable pull down resistor
    
    _ANSB14 = DIGITAL;
    _TRISB14 = INPUT; 
    _CN12IE = ENABLED; // enable interrupt on pin 17 (Change notification 12)
    _CN12PUE = DISABLED; // disable pull up resistor
    _CN12PDE = DISABLED; // disable pull down resistor
    
    // Enable change notification interrupts
    _CNIE = ENABLED;
    _CNIP = 6; // set priority
    _CNIF = 0; // clear interrupt flag
    
    // Initialize IR Sensor (left)
    // Initialize IR Sensor (right)
    // etc

    sensor_distance_left = 0;
    sensor_distance_right = 0;

    // initialize pin change interrupts
}

void event_ir_left_detected() {
    sensor_distance_left = SENSOR_IR_L;
}

void event_ir_right_changed() {
    sensor_distance_right = SENSOR_IR_R;
}


unsigned short is_on_line() {
    return sensor_line_center || sensor_line_left || sensor_line_right;
}

// INTERRUPTS -------------------------------------------------

// see pages 165 and 166 of packet
INTERRUPT CHANGE_NOTIFICATION {
    _CNIF = 0; // clear interrupt flag
    // QRD sensors
    // invert because high voltage means dark space
    sensor_line_left = _RB15 ? FALSE : TRUE; 
    sensor_line_right = _RB14 ? FALSE : TRUE;
}

#endif