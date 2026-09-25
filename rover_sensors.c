#ifndef ROVER_SENSORS
#define ROVER_SENSORS

#include "xc.h"

// #define SENSOR_IR_L
//...

static unsigned short sensor_ir_left = 0;
static unsigned short sensor_ir_right = 0;

void initialize_rover_sensors(){
    // IR Sensor (left)
    _ANSA0 = 0;
    _TRISA0 = 1;
    // etc

    sensor_ir_left = 0;
    sensor_ir_right = 0;
}

void rover_poll_sensors(){

}

#endif