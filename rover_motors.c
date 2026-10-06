#ifndef ROVER_MOTORS
#define ROVER_MOTORS

#include "xc.h"
#include "constants.c"
#include "rover_timing.c"
#include "rover_math.c"

#define MOTOR_LEFT_TIMER _LATB0
#define MOTOR_LEFT_DIRECTION_FORWARD _LATB1
#define MOTOR_LEFT_DIRECTION_BACKWARD _LATB2

#define MOTOR_RIGHT_TIMER _LATA0
#define MOTOR_RIGHT_DIRECTION_FORWARD _LATA1
#define MOTOR_RIGHT_DIRECTION_BACKWARD _LATA2

#define MOTOR_LEFT 0b01
#define MOTOR_RIGHT 0b10
#define MOTOR_TIMER_LEFT (TIMER_1)
#define MOTOR_TIMER_RIGHT (TIMER_2)
#define MOTOR_BOTH (MOTOR_LEFT & MOTOR_RIGHT)

#define DEGREES_PER_STEP 1.8f

#define WHEEL_DIAMETER 3.2f // [TO MEASURE] inches
#define WHEEL_RADIUS (WHEEL_DIAMETER / 2)


static int motor_left_steps = 0;
static int motor_right_steps = 0;
static int motor_left_target = 0;
static int motor_right_target = 0;


// external (used by other programs)
void initialize_rover_motors();
void turn_degrees(unsigned short motor_ids, float degrees);
void set_motor_speed(unsigned short motor_id, float speed); // speed is in inches per second
void move_forward(float distance);


// internal (used by external functions to control the motor)
void turn_motor_steps(unsigned short motor_id, int steps);
void enable_motor(unsigned short motor_id, int direction);
void disable_motor(unsigned short motor_id);

// hardware (low level motor control)
void motor_left_tick();
void motor_right_tick();


void initialize_rover_motors(){
    // set motor pins as a digital output
    
    // motor one (left):
    // B0 (pin 4): timer
    // B1 (pin 5): motor direction 1
    // B2 (pin 6): motor direction 2
    _ANSB0 = DIGITAL;
    _ANSB1 = DIGITAL;
    _ANSB2 = DIGITAL;
    _TRISB0 = OUTPUT;
    _TRISB1 = OUTPUT;
    _TRISB2 = OUTPUT;

    // motor two (right):
    // XX (pin x): timer 
    // XX (pin x): motor direction 1
    // XX (pin x): motor direction 2


    // configure PWM to set motor speed
    // initialize timers and interrupts
}

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
    float degrees_per_second = radians_per_second * 180.0f / PI;
    int steps_per_second = (int)(degrees_per_second / DEGREES_PER_STEP);

    if(motor_id & MOTOR_LEFT){
        set_timer_ticks_per_second(MOTOR_TIMER_LEFT, steps_per_second);
        enable_motor(MOTOR_TIMER_LEFT, (int)sign_float(speed));
    }
    
    if(motor_id & MOTOR_RIGHT){
        set_timer_ticks_per_second(MOTOR_TIMER_RIGHT, steps_per_second);
        enable_motor(MOTOR_LEFT, (int)sign_float(speed));
    }
}

void move_forward(float distance){
    float rotations = distance / WHEEL_RADIUS;
    turn_degrees(MOTOR_BOTH, rotations * 360.0f);
}


// INTERNAL FUNCTIONS -----------------------------------------------------

void turn_motor_steps(unsigned short motor_id, int steps){
    if(motor_id == MOTOR_LEFT){
        motor_left_steps = 0;
        motor_left_target = absolute_value_int(steps);
        enable_motor(MOTOR_LEFT, sign_int(steps));
    } else if (motor_id == MOTOR_RIGHT) {
        motor_right_steps = 0;
        motor_right_target = absolute_value_int(steps);
        enable_motor(MOTOR_RIGHT, sign_int(steps));
    }
}


void enable_motor(unsigned short motor_id, int direction){
    if(motor_id == MOTOR_LEFT){
        MOTOR_LEFT_DIRECTION_FORWARD = (direction > 0);
        MOTOR_LEFT_DIRECTION_BACKWARD = (direction < 0);
    } else if (motor_id == MOTOR_RIGHT) {
        MOTOR_RIGHT_DIRECTION_FORWARD = (direction > 0);
        MOTOR_RIGHT_DIRECTION_BACKWARD = (direction < 0);
    }
}


void disable_motor(unsigned short motor_id){
    enable_motor(motor_id, 0);
}

short is_motor_running(unsigned short motor_id){
    return 0;
}

// HARDWARE FUNCTIONS ----------------------------------------------------- 

void motor_left_tick() {
    motor_left_steps += 1;
    if(motor_left_steps >= motor_left_target && motor_left_target > 0){
        disable_motor(MOTOR_LEFT);
        motor_left_steps = 0;
        motor_left_target = 0;
    }
}


void motor_right_tick() {
    motor_right_steps += 1;
    if(motor_right_steps >= motor_left_target && motor_right_target > 0){
        disable_motor(MOTOR_RIGHT);
        motor_right_steps = 0;
        motor_right_target = 0;
    }
}


// INTERRUPTS -------------------------------------------------------------

// inturrupt for rising edge of left motor timer
// calls motor_left_tick() every time to keep track of distance

// inturrupt for rising edge of left motor timer
// calls motor_left_tick() every time to keep track of distance


#endif