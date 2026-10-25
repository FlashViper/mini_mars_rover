#ifndef ROVER_MILESTONES
#define ROVER_MILESTONES

#include "debug.h"
#include "rover_motors.h"
#include "rover_sensors.h"
#include "rover_logic.h"

void one_tile(){
    rover_move_distance(24.0f, 1.0f, MOTOR_FWD);
}

void stall_1s(){
    // move backward really slowly to give user time to take their hand off the button
    rover_move_distance(1.0f, 1.0f, MOTOR_BWD);
}

void wait_for_input(){
    while(!DEBUG_BUTTON_PRESSED){
        
    }
}

void test_qrd_sensor(){
    while(1){
        static float speed = 20.0f;
        rover_set_constant_speed(sensor_line_left * speed, sensor_line_right * speed);
    }
}

void test_line_follow(){
    stall_1s();
    while(1){
        state_follow_line();
    }
}

void run_milestone_5(float distance_fwd, float time_fwd, float time_turn){
    
    enable_debug_led();
    
    stall_1s();

    // forward
    rover_move_distance(distance_fwd, time_fwd, MOTOR_FWD);
    
    enable_debug_led();

    // left 90 degrees
    rover_turn_degrees(90, time_turn, 0.0f, COUNTERCLOCKWISE);
    
    // forward
    rover_move_distance(distance_fwd, time_fwd * 2, MOTOR_FWD);
    
    // right 180 degrees (duration * 2 to keep turning time consistant)
    rover_turn_degrees(180, time_turn * 2, 0.0f, COUNTERCLOCKWISE);
    
    // move back the same amount
    rover_move_distance(distance_fwd, time_fwd * 1.5, MOTOR_FWD);
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