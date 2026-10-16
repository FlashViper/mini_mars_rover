#ifndef ROVER_SENSORS
#define ROVER_SENSORS

#include "xc.h"

#define SENSOR_IR_L _TRISA0 // TO DO: UPDATE WITH ACTUAL PIN
#define SENSOR_IR_R _TRISA1 // TO DO: DITTO
//...

static unsigned short sensor_ir_left = 0;
static unsigned short sensor_ir_right = 0;

void event_ir_right_changed();
void event_ir_left_detected();

void initialize_rover_sensors(){
    // Initialize IR Sensor (left)
    // Initialize IR Sensor (right)
    // etc

    sensor_ir_left = 0;
    sensor_ir_right = 0;

    // initialize pin change interrupts
}

void event_ir_left_detected() {
    sensor_ir_left = SENSOR_IR_L;
}

void event_ir_right_changed() {
    sensor_ir_right = SENSOR_IR_R;
}


// INTERRUPTS -------------------------------------------------


#endif