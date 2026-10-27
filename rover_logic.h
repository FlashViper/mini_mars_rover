#ifndef ROVER_LOGIC
#define ROVER_LOGIC

#include "rover_sensors.h"
#include "rover_motors.h"
#include "line_following.h"
#include "constants.h"
#include "debug.h"


// prigress flags (set to true when we complete part of the course)
unsigned short flag_collected_sample = FALSE;
unsigned short flag_deposited_sample = FALSE;
unsigned short flag_canyon_navigated = FALSE;

void state_initial();
void state_follow_line();
void state_sample_collect();
void state_sample_deposit();
void state_canyon();
void state_antenna_seek();
void state_find_line();
void state_panic();
void state_final();

// function is a handy type that was defined in constants.c
static function state = state_initial;
static function last_signifigant_state = state_follow_line; // the state it will fall back to in an emergency

void initialize_rover_logic(){
    flag_collected_sample = FALSE;
    flag_deposited_sample = FALSE;
    flag_canyon_navigated = FALSE;
    state = state_initial;
}

void wait_for_input(){
    while(!DEBUG_BUTTON_PRESSED){
        
    }
}

void rover_logic_run(){
    state();
}

void state_initial(){
    // follow line until you come to a crossroads
    // then turn left :)
    enable_debug_led();
    static int flag_found_line = FALSE;

    last_signifigant_state = state_initial;
    rover_set_constant_speed(14.0f, 14.0f);

    if(!flag_found_line){
        if(is_on_line()) 
            flag_found_line = TRUE;

        return;
    } else if (is_on_line()) {
        follow_line_with_pid(&line_follower_slow_accurate); // change to reckless later ?
        // follow_line_with_pid(&line_follower_fast_reckless);
        return;
    }

    disable_debug_led();
    // rover_move_distance(4, 0.3, MOTOR_BWD);
    rover_turn_degrees(95, 0.24f, 0.0f, COUNTERCLOCKWISE);
    state = state_follow_line;
}

void state_follow_line(){
    last_signifigant_state = state_follow_line;

    // we want this to be FAST. But faster controllers are more reckless and miss the line often
    // therefore, we need to switch dynamically between severeal pid values based on how curvy the path is getting

    // Here's the plan: we have a 'reckless mode' switch that we can turn off whenever we're worried about 
    // the state of our robot's soul. Turn it off, and it'll follow the line carefully. Turn it on, and the robot
    // will follow the line as fast as robot-ly possible. But if the robot gets too far away from the line, we
    // panic and flip the switch back to the careful mode. Once the robot is going in a straight enough direction for
    // long enough, we can 

    // Other ideas:
    // robot moves in sprints: each tile, move ~6 inches carefully, 
    // then if the tile looks straight, switch to the fast pid until you reach the next tile, otherwise move carefully
    // ^^^ this idea is kinda fire not gonna lie. I might try coding that and see which one turns out better

    const unsigned short reckless_mode_allowed = ENABLED; // hard-coded so we can switch it off in software
    unsigned short reckless_mode = DISABLED; // disabled at the start

    // How do we measure if the robot is going off course? The pid function returns the speed variance, which
    // is half of the difference between the speed (in inches per second) of each wheel
    // if we average that over a very long time window, we can see how different the speeds of the wheels have been
    // on average for the last while, which is a pretty good indicator of if we've had to turn a lot
    static float avg_speed_variance = 9999; // set to a large number for now
    const float speed_threshold = 1.0f; // probably needs to be bigger
    const float weight = 0.98f; // might need to be ~0.999f because of the oscilator speed
    float speed_variance = 0.0f;
    
    if(reckless_mode_allowed && reckless_mode){
        speed_variance = follow_line_with_pid(&line_follower_fast_reckless);
        disable_debug_led();

        if(!sensor_line_center || absolute_value_float(avg_speed_variance) > speed_threshold) {
            reckless_mode = DISABLED;
        }
    } else {
        enable_debug_led();
        speed_variance = follow_line_with_pid(&line_follower_slow_accurate);

        if (sensor_line_center && absolute_value_float(avg_speed_variance) < speed_threshold) {
            reckless_mode = ENABLED;
        }
    }
    
    // might have to move this to a timer-based interrupt, like _T5Interrupt or something
    avg_speed_variance = weight * avg_speed_variance + (1 - weight) * speed_variance;
    
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

    // Finally, if none of the above conditions triggered and we still can't find the line,
    // switch states to state_find_line
    if(!is_on_line()){
        state = state_find_line;
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
    // navegate the canyonx
    const float canyon_speed = 20.0f;
    rover_set_constant_speed(canyon_speed, canyon_speed);
    float wall_direction = (float)sensor_distance_right - (float)sensor_distance_left;

    // about to hit a wall so let's make a decision QUICK
    if(sensor_distance_forward){ 
        rover_set_constant_speed(0, 0);
        rover_turn_degrees(90, 0.25f, 0.0f, (wall_direction > 0) ? COUNTERCLOCKWISE : CLOCKWISE);
    } 

    if(is_on_line()){
        rover_turn_degrees(90, 0.25f, 0.0f, (wall_direction > 0) ? COUNTERCLOCKWISE : CLOCKWISE);
        state = state_follow_line;
        return;
    }
    // else {
    //     if(wall_direction > 0.01){
            
    //     } else if (wall_direction < -0.01){

    //     }
    // }
}

void state_antenna_seek(){
    // navegate to the lander
    // then point antenna up at the thing
    // can't be that hard right?
}

void state_find_line(){
    // int not_on_line_cycles = 0;

    // while(not_on_line_cycles < 3) {
    //     rover_move_distance(4.0, 0.3f, MOTOR_BWD);
    //     not_on_line_cycles++;

    //     if(is_on_line()){
    //         state = last_signifigant_state;
    //     }
    // }
    
    rover_set_constant_speed(-6, -6);
    initialize_timer_1(1);
    while(!flag_timer_1_elapsed){
        if(is_on_line()){
            state = last_signifigant_state;
            return;
        }
    }
    state = state_panic;
}

void state_panic(){
    rover_set_constant_speed(0.0f, 0.0f); // hit the breaks!!!
    wait_for_input(); // wait for us to move it back to the right spot
    state = last_signifigant_state;
}

void state_final(){
    // do nothing
    // or maybe do a little victory dance
    // either way, you're done!!!
}

#endif