#ifndef ROVER_DEBUG
#define ROVER_DEBUG

#include "xc.h"
#include "constants.h"
#include "rover_motors.h"

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


void one_tile(){
    rover_move_distance(24.0f, 1.0f, MOTOR_FWD);
}

void wait_for_input(){
    while(!DEBUG_BUTTON_PRESSED){
        
    }
}

void run_milestone_5(float distance_fwd, float time_fwd, float time_turn){
    
    enable_debug_led();
    
    // move backward really slowly to give user time to take their hand off
    rover_move_distance(1.0f, 2.0f, MOTOR_BWD);

    // forward
    rover_move_distance(distance_fwd, time_fwd, MOTOR_FWD);

    // left 90 degrees
    rover_turn_degrees(90, time_turn, 0.0f, COUNTERCLOCKWISE);
    
    // forward
    rover_move_distance(distance_fwd * 2, time_fwd * 2, MOTOR_FWD);
    
    // right 180 degrees (duration * 2 to keep turning time consistant)
    rover_turn_degrees(180, time_turn * 2, 0.0f, CLOCKWISE);
    
    // move back the same amount
    rover_move_distance(distance_fwd * 2, time_fwd * 1.5, MOTOR_FWD);
}

void run_course_blind(){
    // move backward really slowly to give user time to take their hand off
    rover_move_distance(1.0f, 2.0f, MOTOR_BWD);
    
    // navigate to sample
    one_tile();
    one_tile();
    one_tile();
    
    // "collect" sample
    rover_move_distance(15.0f, 0.35f, MOTOR_BWD);
    rover_turn_degrees(45, 0.25f, 0, CLOCKWISE);
    rover_move_distance(15, 0.5f, MOTOR_FWD);
    rover_move_distance(1, 1, MOTOR_BWD);
    rover_move_distance(9, 0.5f, MOTOR_BWD);
    rover_turn_degrees(45, 0.25f, 0, COUNTERCLOCKWISE);
    
    wait_for_input();
    one_tile();
    one_tile();
    rover_turn_degrees(90.0f, 0.35f, 0.0f, CLOCKWISE);
    one_tile();
    one_tile();
    one_tile();
    rover_turn_degrees(90.0f, 0.35f, 0.0f, CLOCKWISE);
    
    one_tile();
    one_tile();
    rover_turn_degrees(90, 0.75, 0, COUNTERCLOCKWISE);
    one_tile();
    rover_turn_degrees(90, 0.75, 0, CLOCKWISE);
    one_tile();
    rover_turn_degrees(90, 0.75, 0, CLOCKWISE);
    one_tile();
    rover_turn_degrees(90, 0.75, 0, COUNTERCLOCKWISE);
}

#endif