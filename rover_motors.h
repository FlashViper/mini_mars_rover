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

#define MOTOR_RIGHT_DIRECTION _LATB9
#define MOTOR_RIGHT_DISABLED _LATB8

#define MOTOR_LEFT 0b01
#define MOTOR_RIGHT 0b10
#define MOTOR_BOTH (MOTOR_LEFT & MOTOR_RIGHT)

// define statements linking motor left and pwm 2
#define SET_MOTOR_LEFT_PWM_PERIOD(period) set_pwm_2_period(period)
#define SET_MOTOR_LEFT_PWM_DUTY_CYCLE(cycle) set_pwm_2_duty_cycle(cycle)
#define MOTOR_LEFT_INTERRUPT_ENABLE_FLAG _OC2IE
#define MOTOR_LEFT_INTERRUPT_FUNCTION _OC2Interrupt(void)

// define statements linking motor right and pwm 1
#define SET_MOTOR_RIGHT_PWM_PERIOD(period) set_pwm_1_period(period)
#define SET_MOTOR_RIGHT_PWM_DUTY_CYCLE(cycle) set_pwm_1_duty_cycle(cycle)
#define MOTOR_RIGHT_INTERRUPT_ENABLE_FLAG _OC1IE
#define MOTOR_RIGHT_INTERRUPT_FUNCTION _OC1Interrupt(void)


static int motor_left_steps = 0;
static int motor_right_steps = 0;


// external (used by other programs)
void initialize_rover_motors();
void rover_turn_degrees(float angle, float duration, float pivot_point, int direction);
void enable_motor_left();
void enable_motor_right();
void disable_motor_left();
void disable_motor_right();

// internal (used inside this script)
void turn_motor_ticks(int left_ticks, int right_ticks);

// hardware (low level motor control)
void motor_left_tick();
void motor_right_tick();

// TO DO: UPDATE WITH ACTUAL PINS
void initialize_rover_motors(){
    // set motor pins as a digital output
    
    // motor one (left):
    // B0 (pin 4): pwm signal (OC2)
    // A0 (pin 2): motor disable
    // A1 (pin 3): motor direction
    _ANSB0 = DIGITAL;
    _ANSA0 = DIGITAL;
    _ANSA1 = DIGITAL;

    _TRISA0 = OUTPUT;
    _TRISA1 = OUTPUT;
    _TRISA2 = OUTPUT;
    _TRISB2 = OUTPUT;
    
    // motor two (right):
    // A6 (pin 154: pwm signal (OC1)
    // B12 (pin 23): motor direction
    // B13 (pin 24): motor disable
    _TRISB12 = OUTPUT;
    _TRISB13 = OUTPUT;
    

    SET_MOTOR_LEFT_PWM_DUTY_CYCLE(0.5f);
    SET_MOTOR_RIGHT_PWM_DUTY_CYCLE(0.5f);


    // initialize timers and interrupts
    MOTOR_RIGHT_INTERRUPT_ENABLE_FLAG = DISABLED;
    MOTOR_LEFT_INTERRUPT_ENABLE_FLAG = DISABLED;
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
void rover_turn_degrees(float angle, float duration, float pivot_point, int direction){
    enable_debug_led();

    float pivot_point_rescaled = 0.5 * (pivot_point + 1); // rescale to between 0 and 1
    if (direction == CLOCKWISE){
        MOTOR_RIGHT_DIRECTION = MOTOR_BWD;
        MOTOR_LEFT_DIRECTION = MOTOR_FWD;
    } else {
        MOTOR_RIGHT_DIRECTION = MOTOR_FWD;
        MOTOR_LEFT_DIRECTION = MOTOR_BWD;
    }

    float left_distance = (PI / 180) * angle * WHEEL_TO_CENTER_DISTANCE * (1.0f - pivot_point_rescaled);
    int left_ticks = STEPS_PER_REVOLUTION * left_distance / (WHEEL_DIAMETER * PI);
    int period_left = left_ticks / duration;
    SET_MOTOR_LEFT_PWM_PERIOD(period_left);


    float right_distance = (PI / 180) * angle * WHEEL_TO_CENTER_DISTANCE * (pivot_point_rescaled);
    int right_ticks = STEPS_PER_REVOLUTION * right_distance / (WHEEL_DIAMETER * PI);
    int period_right = right_ticks / duration;
    SET_MOTOR_RIGHT_PWM_PERIOD(period_right);

    turn_motor_ticks(left_ticks, right_ticks);

    disable_motor_left();
    disable_motor_right();
}

void rover_move_distance(float distance, float time, int direction) {
    disable_debug_led();

    MOTOR_LEFT_DIRECTION = direction;
    MOTOR_RIGHT_DIRECTION = direction;
    int ticks = STEPS_PER_REVOLUTION * distance / (WHEEL_DIAMETER * PI);
    int period = ticks / time;
    SET_MOTOR_LEFT_PWM_PERIOD(period);
    SET_MOTOR_RIGHT_PWM_PERIOD(period);
    turn_motor_ticks(ticks, ticks);

    disable_motor_left();
    disable_motor_right();
}

void enable_motor_left() {
    SET_MOTOR_LEFT_PWM_DUTY_CYCLE(0.5);
    MOTOR_LEFT_INTERRUPT_ENABLE_FLAG = ENABLED;
}

void enable_motor_right() {
    SET_MOTOR_RIGHT_PWM_DUTY_CYCLE(0.5);
    MOTOR_RIGHT_INTERRUPT_ENABLE_FLAG = ENABLED;
}

void disable_motor_left() {
    SET_MOTOR_LEFT_PWM_DUTY_CYCLE(0);
    MOTOR_LEFT_INTERRUPT_ENABLE_FLAG = DISABLED;
}

void disable_motor_right() {
    SET_MOTOR_RIGHT_PWM_DUTY_CYCLE(0);
    MOTOR_RIGHT_INTERRUPT_ENABLE_FLAG = DISABLED;
}

// INTERNAL FUNCTIONS -----------------------------------------------------

void turn_motor_ticks(int ticks_left, int ticks_right){
    // do some timer mumbo jumbo to finish when it's done
    if(ticks_left > 0){
        motor_left_steps = ticks_left;
        enable_motor_left();
    } else {
        motor_left_steps = 0;
        disable_motor_left();
    }

    if(ticks_right > 0){
        motor_right_steps = ticks_right;
        enable_motor_right();
    } else {
        motor_right_steps = 0;
        disable_motor_right();
    }

    // wait until both motors are finished
    while(motor_right_steps > 0 && motor_left_steps > 0){

    }
}

// HARDWARE FUNCTIONS ----------------------------------------------------- 

void motor_left_tick() {
    motor_left_steps -= 1;

    if(motor_left_steps <= 0){
        disable_motor_left();
    }
}


void motor_right_tick() {
    motor_right_steps -= 1;

    if(motor_right_steps <= 0){
        motor_right_steps = 0;
        MOTOR_RIGHT_INTERRUPT_ENABLE_FLAG = DISABLED;
    }
}


// INTERRUPTS -------------------------------------------------------------

// inturrupt for rising edge of left motor timer
// calls motor_left_tick() every time to keep track of distance
INTERRUPT MOTOR_RIGHT_INTERRUPT_FUNCTION{
    _OC1IF = 0;
    motor_right_tick();
}

INTERRUPT MOTOR_LEFT_INTERRUPT_FUNCTION{
    _OC2IF = 0;
    motor_left_tick();
}

// inturrupt for rising edge of left motor timer
// calls motor_left_tick() every time to keep track of distance


#endif