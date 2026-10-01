#ifndef ROVER_SENSORS
#define ROVER_SENSORS

// #include "xc.h"

// #define SENSOR_IR_L
//...

static unsigned short sensor_ir_left = 0;
static unsigned short sensor_ir_right = 0;

void initialize_rover_sensors(){
    // Initialize IR Sensor (left)
    // Initialize IR Sensor (right)
    // etc

    sensor_ir_left = 0;
    sensor_ir_right = 0;
}

void rover_poll_sensors(){

}

#endif