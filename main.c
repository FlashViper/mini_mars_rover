#pragma config OSCIOFNC = OFF // turn off the clock output so we can use pin 8 as a digital io
// #pragma config FNOSC=FRC // 8 MHz oscilator
// GOOD JOB JOEYYYYYYYYY - Elliot
#pragma config FWDTEN = OFF
#pragma config WINDIS = OFF

#include "rover_sensors.h"
#include "rover_logic.h"
#include "rover_timing.h"
#include "rover_motors.h"
#include "debug.h"
#include "laser.h"
#include "rover_milestones.h"
#include "servo.h"

#define DEBUG 1
#define NORMAL 0

int main(){
    initialize_rover_timing();
    initialize_rover_sensors();
    initialize_rover_motors();
    initialize_laser();
    initialize_servo();
    initialize_debug();

    // float angle = 0.0f;
    // int counting_up = TRUE;
    // set_servo_angle(90);
    // enable_debug_led();
    
    while(1){
        if(DEBUG_BUTTON_PRESSED) enable_laser();
        else disable_laser();
    }
    return 0;
    unsigned short mode = NORMAL;
    if(DEBUG_BUTTON_PRESSED){
        mode = DEBUG;
        // debug_proximity_sensors();
        set_servo_angle(85);
        debug_proximity_sensors_individually();
        debug_qrd_sensors();
        set_servo_angle(10);
        // wait_for_input();
        // rover_turn_degrees(90, 0.25f, 0, CLOCKWISE);
        // rover_turn_degrees(90, 0.25f, 0, COUNTERCLOCKWISE);
    }
    
    initialize_rover_logic();
    enable_debug_led();
    wait_for_input();
    disable_debug_led();
    if(mode == DEBUG){
        state = state_initial;
    }
    while(1){   
        rover_logic_run();
    }
    
    return 0;
}