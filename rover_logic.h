#ifndef ROVER_LOGIC
#define ROVER_LOGIC

#include "rover_sensors.h"
#include "rover_motors.h"
#include "line_following.h"
#include "constants.h"
#include "debug.h"

#define SPEED_CANYON 

unsigned short flag_collected_sample = FALSE;
unsigned short flag_deposited_sample = FALSE;
unsigned short flag_canyon_navigated = FALSE;

void state_initial();
void state_follow_line(); // might need a state_find_line for when we finish with other tasks
void state_sample_collect();
void state_sample_deposit();
void state_canyon();
void state_antenna_seek();
void state_find_line();
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
    follow_line_with_pid(&line_follower_fast_reckless);
    
        
    // EXIT CONDITIONS:
    // (1) line disappears + see wall ahead or to side --> state_canyon
    // (2) IR sensor detects something --> state_sample_collect
    // (3) see walls to side + still sees line --> state_sample_deposit
    // (4) reach leftward fork in road --> state_antenna_seek

    // Condition (1)
    if(!flag_canyon_navigated && !is_on_line() && sensor_distance_left && sensor_distance_right){
        state = state_canyon;
        return;
    }
    
    // Condition (2)
    if(sensor_distance_right && !sensor_distance_left){
        state = state_sample_collect;
        return;
    }

    // Condition (3)
    if((flag_collected_sample && !flag_deposited_sample) && sensor_distance_left && sensor_distance_right && (is_on_line())){
        state = state_sample_deposit;
        return;
    }

    // Condition (4)
    if(flag_canyon_navigated && flag_collected_sample && flag_deposited_sample){
        state = state_antenna_seek;
        return;
    }
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