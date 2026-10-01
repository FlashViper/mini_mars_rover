#ifndef ROVER_MOTORS
#define ROVER_MOTORS

#include "rover_timing.c"
#include "rover_math.c"


#define MOTOR_LEFT 0b01
#define MOTOR_RIGHT 0b10
#define MOTOR_TIMER_LEFT (TIMER_1)
#define MOTOR_TIMER_RIGHT (TIMER_2)
#define MOTOR_BOTH (MOTOR_LEFT & MOTOR_RIGHT)

#define DEGREES_PER_STEP 1.8f

#define WHEEL_DIAMETER 3.2f // [TO MEASURE] inches
#define WHEEL_RADIUS (WHEEL_DIAMETER / 2)


// external (used by other programs)
void initialize_rover_motors();
void turn_degrees(unsigned short motor_ids, float degrees);
void set_motor_speed(unsigned short motor_id, float speed); // speed is in inches per second


// internal (used by external functions to control the motor)
void turn_motor_steps(unsigned short motor_id, int steps);
void set_motor_direction(unsigned short motor_id, int direction);
void disable_motor(unsigned short motor_id);


void turn_degrees(unsigned short motor_ids, float degrees){
    int steps = (int)(degrees / DEGREES_PER_STEP);
    if (motor_ids & MOTOR_LEFT){
        turn_motor_steps(MOTOR_LEFT, steps);
    }

    if (motor_ids & MOTOR_RIGHT){
        turn_motor_steps(MOTOR_RIGHT, steps);
    }
}

void set_motor_speed(unsigned short motor_id, float speed){
    float radians_per_second = speed / WHEEL_RADIUS;
    float degrees_per_second = radians_per_second * 180.0f / 3.1416f;
    int steps_per_second = (int)(degrees_per_second / DEGREES_PER_STEP);

    if(motor_id & MOTOR_LEFT){
        set_timer_ticks_per_second(MOTOR_TIMER_LEFT, steps_per_second);
        set_motor_direction(MOTOR_LEFT, (int)sign_float(speed));
    }
    
    if(motor_id & MOTOR_RIGHT){
        set_timer_ticks_per_second(MOTOR_TIMER_LEFT, steps_per_second);
        set_motor_direction(MOTOR_LEFT, (int)sign_float(speed));
    }
}

#endif