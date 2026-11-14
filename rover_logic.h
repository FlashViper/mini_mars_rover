#ifndef ROVER_LOGIC
#define ROVER_LOGIC

#include "rover_sensors.h"
#include "rover_motors.h"
#include "line_following.h"
#include "constants.h"
#include "laser.h"
#include "debug.h"


#define COMPLEX 1
#define SIMPLE 0

// prigress flags (set to true when we complete part of the course)
unsigned short flag_collected_sample = FALSE;
unsigned short flag_deposited_sample = FALSE;
unsigned short flag_canyon_navigated = FALSE;

void state_initial();
void state_follow_line();
void state_sample_collect();
void state_sample_deposit();
void state_canyon_simple();
void state_canyon_complex();
void state_canyon_back_up();
void state_look_for_lander();
void state_enter_lander();
void state_antenna_seek();

void state_find_line();
void state_panic();
void state_final();
void switch_to_canyon_mode();

// the current state :)
// function is a handy type that was defined in constants.c
// we're using it in place of an enum because it's easier to read
static function state = state_initial;

// the state it will fall back to in an emergency
static function last_signifigant_state = state_follow_line;

void initialize_rover_logic(){
    flag_collected_sample = FALSE;
    flag_deposited_sample = FALSE;
    flag_canyon_navigated = FALSE;
    // state = state_initial;
    state = state_follow_line;
}

void wait_for_input(){
    while(!DEBUG_BUTTON_PRESSED){
        
    }
}

void rover_logic_run(){
    state();
}

// robot's starting state
// follow the starting line until it disappears
// then turn 90 degrees to the left and start line following mode
void state_initial(){
    // follow line until you come to a crossroads
    // then turn left :)
    enable_debug_led();
    static int flag_found_line = FALSE;
    static long countdown = 2;

    last_signifigant_state = state_initial;
    
    if(!flag_found_line){
        rover_set_fwd_speed(16.0f);
        if(is_on_line()) 
            flag_found_line = TRUE;

        return;
    } else if (countdown > 0) {
        // follow_line_with_pid(&line_follower_slow_accurate); // change to reckless later ?
        follow_line_with_pid(&line_follower_fast_start);
        if(!is_on_line()){
            countdown -= 1;
        }

        return;
    }

    disable_debug_led();
    // rover_turn_degrees(100, 0.24f, 0.0f, COUNTERCLOCKWISE);
    while(!sensor_line_center){
        rover_set_constant_speed(-14.0f, 14.0);
    }
    wait_for_input();
    state = state_follow_line;
    // }
}

// the BIG BEAFY state
// this is what's running when none of the other tasks are
// see line_following.h for more details on how that works
// it dynamically switches between slower and faster PID parameters based on
// how curvy the line is (although this still has to be tuned)
void state_follow_line(){
    last_signifigant_state = state_follow_line;
    enable_debug_led();

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

    const unsigned short reckless_mode_allowed = DISABLED; // hard-coded so we can switch it off in software
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

        if(!sensor_line_center && absolute_value_float(avg_speed_variance) > 0.5 * line_follower_fast_reckless.base_speed) {
            reckless_mode = DISABLED;
        }
    } else {
        enable_debug_led();
        speed_variance = follow_line_with_pid(&line_follower_slow_accurate);

        if (sensor_line_center && absolute_value_float(avg_speed_variance) < 0.5 * line_follower_slow_accurate.base_speed) {
            reckless_mode = ENABLED;
        }
    }
    
    // might have to move this to a timer-based interrupt, like _T5Interrupt or something
    avg_speed_variance = weight * avg_speed_variance + (1 - weight) * speed_variance;
    
    // EXIT CONDITIONS:
    // (1) line disappears + see wall ahead or to side --> state_canyon
    // (2) IR sensor detects something --> state_sample_collect
    // (3) see walls to side + still sees line --> state_sample_deposit
    // (4) all other missions accomplished --> state_look_for_lander


    float wall_distance_left = get_wall_distance_left();
    float wall_distance_right = get_wall_distance_right();
    float wall_distance_center = get_wall_distance_center();
    const float wall_detection_distance = 7.5f;

    if(wall_distance_center < 3.5f){
        rover_move_distance(5.0f, 0.3f, MOTOR_BWD);
        if(!is_on_line() && (wall_distance_left < wall_detection_distance || wall_distance_right < wall_detection_distance)) switch_to_canyon_mode();
    }

    // Condition (1)
    if(!flag_canyon_navigated && !is_on_line() && wall_distance_left < wall_detection_distance && wall_distance_right < wall_detection_distance && wall_distance_center < 22.0f){        
        switch_to_canyon_mode();
        return;
    }
    
    // Condition (2)
    // if(sensor_distance_right && !sensor_distance_left){
    //     state = state_sample_collect;
    //     return;
    // }

    // Condition (3)
    // if((flag_collected_sample && !flag_deposited_sample) && sensor_distance_left && sensor_distance_right && (is_on_line())){
    //     state = state_sample_deposit;
    //     return;
    // }

    // Condition (4)
    if(flag_canyon_navigated && flag_collected_sample && flag_deposited_sample){
        state = state_look_for_lander;
        return;
    }

    // Finally, if none of the above conditions triggered and we still can't find the line,
    // switch states to state_find_line
    // if(!is_on_line() && absolute_value_float(speed_variance) < 2.0f){
    //     return;
    // }
}

// state for picking up the ping pong ball
// should back into the sample collector
void state_sample_collect(){
    // find the sample container using IR and center yourself
    // then push the button
}

// state for dropping off the ping pong ball
// should check what color the ball is and go from there
void state_sample_deposit(){
    // determine which box it needs to go into
    // then navegate to the box and put it in :)
    if(sensor_ball_color > 0){ // white
        
    } else {
        
    }
}

// state for navegating the canyon
void state_canyon_simple(){
    const float canyon_speed = 28.0f;
    rover_set_constant_speed(canyon_speed, canyon_speed);
    
    float wall_direction = (clamp(get_wall_distance_right(), 0, 18) - clamp(get_wall_distance_left(), 0, 18)) / 18;
    // about to hit a wall so let's make a decision QUICK
    if(get_wall_distance_center() < 12.0f){ 
        rover_set_constant_speed(0, 0);
        if(get_wall_distance_center() < 5.0f) rover_move_distance(5, 0.2, MOTOR_BWD);
        
        unsigned int turn_left = wall_direction < 0;
        set_debug_led_state(turn_left);
        rover_turn_degrees(90, 0.2f, 0.0f, (turn_left) ? COUNTERCLOCKWISE : CLOCKWISE);
        disable_debug_led();
    } 

    if(is_on_line()){
        while(is_on_line()){
            rover_set_fwd_speed(canyon_speed);
        }
        rover_turn_degrees(90, 0.25f, 0.0f, (wall_direction < 0) ? COUNTERCLOCKWISE : CLOCKWISE);
        if(get_wall_distance_center() < 7.0f){
            rover_move_distance(5.0, 0.2f, MOTOR_BWD);
            rover_turn_degrees(180, 0.35, 0, CLOCKWISE);
        }
        state = state_follow_line;
        return;
    }
}

void state_canyon_complex(){
    // disable_debug_led();
    const PID pid_canyon = {
        28.0f, 45.0f, 4.5f, 0.0, 1.0, 0.8f
    };

    float error = follow_wall_with_pid(&pid_canyon, 3.0f, 10, 24);
    set_debug_led_state(get_wall_distance_center() < 12.0f);

    if(get_wall_distance_center() < 5){
        rover_move_distance(7.0f, 0.5, MOTOR_BWD);

        float wall_direction = get_wall_distance_right() - get_wall_distance_left();
        unsigned short turn_direction = wall_direction > 0 ? CLOCKWISE : COUNTERCLOCKWISE;
        while(get_wall_distance_center() < 12){
            short multiplier = turn_direction == CLOCKWISE ? -1 : 1;
            rover_set_constant_speed(-pid_canyon.base_speed * multiplier, pid_canyon.base_speed * multiplier);
        }
        // if(absolute_value_float(wall_direction) > 3){
        //     rover_turn_degrees(90, 0.4, 0, wall_direction > 0 ? CLOCKWISE : COUNTERCLOCKWISE);
        // }
    }
    if(is_on_line()){
        state = state_follow_line;
    }
}

void state_canyon_back_up(){
    // rover_set_constant_speed(0, 0);
    rover_move_distance(7.0f, 0.5, MOTOR_BWD);
    state = state_canyon_complex;
}

// once all tasks have been completed, the robot knows the lander is coming right up
// we switch to a special version of the line following algorithm designed only to use the right two qrd sensors
// then we use the left to detect the fork in the road
// in the future we might want to track the distance we've traveled on average and use that to count the number of tiles
// that we've probably traveled, then use that as another metric for switching to this state
void state_look_for_lander() {
    follow_line_with_right_sensor_only(&line_follower_slow_accurate);
    if(sensor_line_left && sensor_line_right){
        state = state_enter_lander;
    }
}

void state_enter_lander(){
    rover_turn_degrees(90, 0.1f, 0.0f, CLOCKWISE);

    static PID line_follower_in_reverse = line_follower_slow_accurate;
    line_follower_in_reverse.base_speed *= -1; // make sure we're following it in reverse since we're backing in :)

    while(is_on_line()){
        follow_line_with_pid(&line_follower_in_reverse);
    }

    // move backwards
    rover_set_constant_speed(0.0f, 0.0f);
    state = state_antenna_seek;
}

void state_antenna_seek(){
    // point antenna up at the thing
    // can't be that hard right?
}

void state_find_line(){
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

void switch_to_canyon_mode(){
    const unsigned short canyon_mode = COMPLEX;
    if(canyon_mode == COMPLEX){
        state = state_canyon_complex;
    } else {
        rover_move_distance(7.0f, 0.25, MOTOR_FWD);
        state = state_canyon_simple;
    }

}

#endif