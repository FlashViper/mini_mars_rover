#ifndef LINE_FOLLOWING
#define LINE_FOLLOWING

#include "rover_motors.h"
#include "rover_sensors.h"
#include "rover_math.h"

// to learn about pid controllers, see pages 353-356 of the packet
// k_i is currently not functional
struct PID {
    float base_speed;
    float k_p;
    float k_d;
    float k_i;
};

typedef struct {
    float base_speed;
    float k_p;
    float k_d;
    float k_i;
} PID;

// This one is so far 100% accurate on all segments of the course
// *It's really slow though* It takes around 40 seconds to navigate a fairly complex line
// that only leaves 20 seconds to play with, which is not enough. Ideally line following
// should take the least time possible, with the exception of maybe the canyon navigation
// ---------------------------------------------------------------------
// k_p tuned pretty well already. k_d needs to be tuned
// k_i likely not needed
const PID line_follower_slow_accurate = {
    14.0f, 9.0f, 0.05f, 0
};

// 
const PID line_follower_fast_reckless = {
    40.0f, 25.0f, 2.5f, 0
};


float calculate_line_sensor_error() {
    // we have to convert to floats because if not the robot will turn into a BANSHEE from the hit book series harry potter
    // I think it's because they're unsigned and subtracting them causes an integer overflow
    // then it converts that to an error
    // so without the (float)s it tries to set error to like 16,000 whenever the left sensor is triggered

    // 1: right sensor on line, left sensor off line
    // 0: both sensors on line (or off)
    // -1: right sensor off line, left sensor on line
    float left_right_difference = (float)sensor_line_right - (float)sensor_line_left;

    // if only one of the three diodes is off the line, it's not a huge deal. 
    // but if two of them are off? that's not good at all!!!
    // this will make the robot take tighter turns when it's about to leave the line
    float center_adjustment = (float)(1 - sensor_line_center) * (left_right_difference);
    
    /* Example error calculations.
    TABLE OF OUTPUTS:
    Code  | Error calculation                    | Error | Action     
    000 --> stop robot                               0    
    100 --> error = (-1 + (1 - 0) * (-1)) / 2       -1            HARD RIGHT
    001 --> error = (1 + (1 - 0) * (1)) / 2          1            HARD LEFT
    110 --> error = (-1 + (1 - 1) * (-1)) / 2      -0.5           slight right
    011 --> error = (1 + (1 - 1) * (1)) / 2         0.5           slight left
    101 (should never happen)                    ||||||||||||||||||||||||||||||||||||||
    111 --> error = (0 + (1 - 1) * 0) / 2            0     STRAIT AND NARROW PATH BABY

    Also, I know this table looks like it was written by a llm. I promise it wasn't. It was written by me.
    You can check the git commit messages. Why did I type this all out? Two reasons:
    (1) to double check my math
    (2) to explain my bonkers duct-tape math to my teammates

    In reality, we might want to weight the center adjustment (multiply by two, for example, then divide by three)
    This would make it even worse of a deal when the robot is wandering off track
    */

    // fun fact, it's faster for a computer to multiply by a reciprocal than to divide
    // that's why it's (... * 0.5) instead of (... / 2)
    return 0.5f * (center_adjustment + left_right_difference);
}


void follow_line_with_pid(const PID *pid_parameters){
    static float previous_error = 0.0f;

    float error = calculate_line_sensor_error();
    float speed_adjustment = (pid_parameters->k_p) * error + (error - previous_error) * pid_parameters->k_d;
    
    rover_set_constant_speed(
        pid_parameters->base_speed + speed_adjustment,
        pid_parameters->base_speed - speed_adjustment
    );

    previous_error = error;
}

#endif