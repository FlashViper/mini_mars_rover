#ifndef ROVER_MOTORS
#define ROVER_MOTORS

#include "xc.h"
#include "measurements.h"
#include "constants.h"
#include "rover_timing.h"
#include "rover_math.h"
#include "debug.h"

#define MOTOR_LEFT_DIRECTION _LATA1
#define MOTOR_LEFT_DISABLED _LATA0

#define MOTOR_RIGHT_DIRECTION _LATB12
#define MOTOR_RIGHT_DISABLED _LATB13

#define MOTOR_LEFT 0b01
#define MOTOR_RIGHT 0b10
#define MOTOR_BOTH (MOTOR_LEFT & MOTOR_RIGHT)

// define statements linking motor left and pwm 2
#define MOTOR_LEFT_PWM(a, b)

static int motor_left_steps = 0;
static int motor_right_steps = 0;


// external (used by other programs)
void initialize_rover_motors();
void turn_degrees(unsigned short motor_ids, float degrees);
void set_motor_speed(unsigned short motor_id, float speed); // speed is in inches per second
void move_forward(float distance);
void rover_turn_degrees(float angle, float duration, float pivot_point);


// internal (used by external functions to control the motor)
void turn_motor_steps(unsigned short motor_id, int steps);
void set_motor_enabled(unsigned short motor_id, int enabled);
void set_motor_direction(unsigned short motor_id, int direction);
void disable_motor(unsigned short motor_id);

// hardware (low level motor control)
void motor_left_tick();
void motor_right_tick();

// TO DO: UPDATE WITH ACTUAL PINS
void initialize_rover_motors(){
    // set motor pins as a digital output
    
    // motor one (left):
    // B0 (pin 4): pwm signal
    // A0 (pin 2): motor disable
    // A1 (pin 3): motor direction
    _ANSB0 = DIGITAL;
    _ANSA0 = DIGITAL;
    _ANSA1 = DIGITAL;

    _TRISA0 = OUTPUT;
    _TRISA1 = OUTPUT;
    _TRISA2 = OUTPUT;
    _TRISB2 = OUTPUT;

    set_pwm_2_duty_cycle(0.5f);
    
    // motor two (right):
    // A6 (pin 154: pwm signal 
    // B12 (pin 23): motor direction
    // B13 (pin 24): motor disable
    _TRISB12 = OUTPUT;
    _TRISB13 = OUTPUT;
    
    set_pwm_1_duty_cycle(0.5f);


    // initialize timers and interrupts
    _OC1IE = 0;
    _OC2IE = 0;
}


// utility function for performing specific manuvers (like turning 90 degrees)
// angle is the angle it turns
// duration is the time it turns the angle in (warning: may be limited by physical constraints)
// pivot_point is the percentage distance that the pivot point is offset relative to the centerpoint between the two wheels
// ex: rover_turn_degrees(90, 0.5, 0) --> turn 90 degrees about a the robot's center, lasting half a second total
//     rover_turn_degrees(45, 0.3, 1.0) --> turn 45 degrees about the right wheel
//     rover_turn_degrees(-45, 0.3, 0.5) --> turn 45 degrees about the left wheel
//     rover_turn_degrees(180, 0.3, 2) --> make a wide 180 degree u-turn
//     rover_turn_degrees(90, 0.3, -0.5) --> turn 90 degrees about a point close to the inside wheel
void rover_turn_degrees(float angle, float duration, float pivot_point){
    float left_distance = (PI / 180) * angle * WHEEL_TO_CENTER_DISTANCE * (1.0f - pivot_point);
    int left_ticks = STEPS_PER_REVOLUTION * left_distance / (WHEEL_DIAMETER * 2 * PI);
    int period_left = left_ticks / duration;
    set_pwm_1_period(period_left);


    float right_distance = (PI / 180) * angle * WHEEL_TO_CENTER_DISTANCE * (-pivot_point);
    int right_ticks = STEPS_PER_REVOLUTION * right_distance / WHEEL_DIAMETER;
    int period_right = right_ticks / duration;
    set_pwm_2_period(period_right);

    // do some timer mumbo jumbo to finish when it's done
    motor_right_steps = 0;
    motor_left_steps = 0;
    _OC1IE = 1;
    _OC2IE = 1;
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

// sets the motor speed in inches per second
// MOTOR_LEFT: left stepper motor
// MOTOR_RIGHT: right stepper motor
// MOTOR_BOTH: both motors
void set_motor_speed(unsigned short motor_id, float speed){
    float radians_per_second = speed / WHEEL_RADIUS;
    float degrees_per_second = radians_per_second * 180.0f / PI;
    int steps_per_second = (int)(degrees_per_second / DEGREES_PER_STEP);

    if(motor_id & MOTOR_LEFT){
        set_timer_ticks_per_second(MOTOR_TIMER_LEFT, steps_per_second);
        // enable_motor(MOTOR_TIMER_LEFT, (int)sign_float(speed));
    }
    
    if(motor_id & MOTOR_RIGHT){
        set_timer_ticks_per_second(MOTOR_TIMER_RIGHT, steps_per_second);
        // enable_motor(MOTOR_LEFT, (int)sign_float(speed));
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
        set_motor_enabled(MOTOR_LEFT, TRUE);
    } else if (motor_id == MOTOR_RIGHT) {
        motor_right_steps = 0;
        motor_right_target = absolute_value_int(steps);
        set_motor_enabled(MOTOR_RIGHT, TRUE);
    }
}


void set_motor_enabled(unsigned short motor_id, int enabled){
    if(motor_id == MOTOR_LEFT){
        MOTOR_LEFT_DISABLED = enabled ? FALSE : TRUE;
    } else if (motor_id == MOTOR_RIGHT) {
        MOTOR_RIGHT_DISABLED = enabled ? FALSE : TRUE;
    }
}


void disable_motor(unsigned short motor_id){
    set_motor_enabled(motor_id, FALSE);
}

short is_motor_running(unsigned short motor_id){
    return 0;
}

// HARDWARE FUNCTIONS ----------------------------------------------------- 

void motor_left_tick() {
    enable_debug_led();
    motor_left_steps -= 1;

    if(motor_left_steps <= 0){
        disable_debug_led();
        disable_motor(MOTOR_LEFT);
        set_pwm_2_period(0);
        set_pwm_2_duty_cycle(0);
        motor_left_steps = 0;
        _OC2IE = 0;
    }
}


void motor_right_tick() {
    motor_right_steps -= 1;

    if(motor_right_steps <= 0){
        disable_motor(MOTOR_RIGHT);
        motor_right_steps = 0;
        _OC1IE = 0;
    }
}


// INTERRUPTS -------------------------------------------------------------

// inturrupt for rising edge of left motor timer
// calls motor_left_tick() every time to keep track of distance
void __attribute__((interrupt, no_auto_psv)) _OC1Interrupt(void){
    _OC1IF = 0;
    motor_right_tick();
}

void __attribute__((interrupt, no_auto_psv)) _OC2Interrupt(void){
    _OC2IF = 0;
    motor_left_tick();
}

// inturrupt for rising edge of left motor timer
// calls motor_left_tick() every time to keep track of distance


#endif