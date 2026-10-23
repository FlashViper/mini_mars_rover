#ifndef ROVER_SENSORS
#define ROVER_SENSORS

#include "xc.h"

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


#endif