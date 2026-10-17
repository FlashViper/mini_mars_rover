#ifndef ROVER_LOGIC
#define ROVER_LOGIC

#include "rover_sensors.h"
#include "rover_motors.h"
#include "constants.h"

#define SPEED_CANYON 

void state_initial();
void state_follow_line(); // might need a state_find_line for when we finish with other tasks
void state_sample_collect();
void state_sample_deposit();
void state_canyon();
void state_antenna_seek();
void state_final();

// function is a handy type that was defined in constants.c
static function state = state_initial;

void initialize_rover_logic(){
    state = state_initial;
}

void rover_logic_run(){
    state();
}

void state_initial(){
    // follow line until you come to a crossroads
    // then turn left :)
    rover_turn_degrees(90, 1.5, 0.0f, CLOCKWISE);
}

void state_follow_line(){
    if (sensor_ir_left){
        // do something
    }
    
    // follow the line...duh

    // EXIT CONDITIONS:
    // line disappears + see wall ahead or to side --> state_canyon
    // IR sensor detects something --> state_sample_collect
    // see walls to side + still sees line --> state_sample_deposit
    // reach leftward fork in road --> state_antenna_seek
}

void state_sample_collect(){
    // find the sample container using IR and center yourself
    // then push the button
}

void state_sample_deposit(){
    // determine which box it needs to go into
    // then navegate to the box and put it in :)
}

void state_canyon(){
    // navegate the canyon
    // set_motor_speed(MOTOR_BOTH, 0.0f);
}

void state_antenna_seek(){
    // navegate to the lander
    // then point antenna up at the thing
    // can't be that hard right?
}

void state_final(){
    // do nothing
    // or maybe do a little victory dance
    // either way, you're done!!!
}

#endif