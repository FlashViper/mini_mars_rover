#ifndef ROVER_MOTORS
#define ROVER_MOTORS

#include "xc.h"
#include "measurements.h"
#include "constants.h"
#include "rover_timing.h"
#include "rover_math.h"

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
void rover_move_distance(float distance, float time, int direction);
void rover_turn_degrees(float angle, float duration, float pivot_point, int direction);
void rover_set_constant_speed(float left_speed, float right_speed);

// internal (used inside this script)
void enable_motor_left();
void enable_motor_right();
void disable_motor_left();
void disable_motor_right();
void turn_motor_ticks(int left_ticks, int right_ticks);
void set_individual_motor_speed(unsigned short motor_id, float speed);
unsigned short convert_to_direction(float input);

// hardware (low level motor control)
void motor_left_tick();
void motor_right_tick();

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
    // A6 (pin 14: pwm signal (OC1)
    // B9 (pin 13): motor direction
    // B8 (pin 12): motor disable
    _TRISB9 = OUTPUT;
    _TRISB8 = OUTPUT;
    
    // set the default duty cycle to 0.5
    SET_MOTOR_LEFT_PWM_DUTY_CYCLE(0.5f);
    SET_MOTOR_RIGHT_PWM_DUTY_CYCLE(0.5f);


    // initialize timers and interrupts
    MOTOR_RIGHT_INTERRUPT_ENABLE_FLAG = DISABLED;
    MOTOR_LEFT_INTERRUPT_ENABLE_FLAG = DISABLED;

    // disable motors by default
    disable_motor_left();
    disable_motor_right();
}

/*
A function for performing specific manuvers (like turning 90 degrees)
Parameters:
    angle: the angle the robot turns (in degrees)
    duration: the time it turns the angle in (warning: may be limited by constraints of the pwm signal and the physical motors)
    pivot_point: the percentage distance that the pivot point is offset relative to the centerpoint between the two wheels

Examples: 
    rover_turn_degrees(90, 0.5, 0) --> turn 90 degrees about a the robot's center, lasting half a second total
    rover_turn_degrees(45, 0.3, 1.0) --> turn 45 degrees about the right wheel
    rover_turn_degrees(-45, 0.3, 0.5) --> turn 45 degrees about the left wheel
    rover_turn_degrees(180, 0.3, 2) --> make a wide 180 degree u-turn
    rover_turn_degrees(90, 0.3, -0.5) --> turn 90 degrees about a point close to the inside wheel
*/
void rover_turn_degrees(float angle, float duration, float pivot_point, int direction){
    // angle is multiplied by 2 to fix a bug I think it's with the timers though
    float signed_angle = 2 * angle * ((direction == CLOCKWISE) ? 1 : -1);
    float pivot_point_rescaled = 0.5 * (pivot_point + 1); // rescale to between 0 and 1

    // convert angle into a distance to travel, then convert that into a number of ticks to turn
    // formula is (angle in radians) * (distance from wheel to pivot point)
    // ^^^ this is just the arc length formula if you're curious
    float left_distance = (PI / 180) * (signed_angle) * WHEEL_TO_CENTER_DISTANCE * (1.0f - pivot_point_rescaled);
    float right_distance = (PI / 180) * (-signed_angle) * WHEEL_TO_CENTER_DISTANCE * (pivot_point_rescaled);
    MOTOR_LEFT_DIRECTION = convert_to_direction(left_distance);
    MOTOR_RIGHT_DIRECTION = convert_to_direction(right_distance);

    // turn distance into number of revolutions and then into steps
    // formula is (steps per revolution) * (distance to travel / wheel circumferance)
    int left_ticks = STEPS_PER_REVOLUTION * left_distance / (WHEEL_DIAMETER * PI);
    int right_ticks = STEPS_PER_REVOLUTION * right_distance / (WHEEL_DIAMETER * PI);
    
    // we know how many ticks each motor needs to turn but not how fast, so we divide
    // the number of ticks by the duration to get the steps per second of the wheel
    // We multiply by 0.5 at the end because it fixes shenanigans with the timer only firing
    // at half of the oscilator speed, but I should probably move that to the function itself
    int period_left = absolute_value_int(left_ticks) / duration * 0.5f;
    int period_right = absolute_value_int(right_ticks) / duration * 0.5f;
    SET_MOTOR_LEFT_PWM_PERIOD(period_left);
    SET_MOTOR_RIGHT_PWM_PERIOD(period_right);

    // now that we've calculated the number of ticks to go we can call this handy function, which
    // will wait until each motor finishes turning
    turn_motor_ticks(left_ticks, right_ticks);

    disable_motor_left();
    disable_motor_right();
}

/*
A function for telling the rover to move a certain distance in a specific amount of time.
Parameters:
    distance: the distance to move (in inches)
    time: the amount of time to move that distance in
    direction: forward (MOTOR_FWD) or backward (MOTOR_BWD)

Examples:
    rover_move_distance(12, 1.0f, MOTOR_FWD) --> move forward 1 foot in one second
    rover_move_distance(24, 1.5f, MOTOR_BWD) --> move backward 2 feet in 1.5 seconds
*/
void rover_move_distance(float distance, float time, int direction) {
    // could do it based off of the sign of distance but I like that it's more explicit
    MOTOR_LEFT_DIRECTION = direction;
    MOTOR_RIGHT_DIRECTION = direction;
    enable_motor_left();
    enable_motor_right();
    
    // formula: (steps per revolution) * (distance / circumferance)
    int ticks = STEPS_PER_REVOLUTION * distance / (WHEEL_DIAMETER * PI);
    int period = ticks / time;
    SET_MOTOR_LEFT_PWM_PERIOD(period);
    SET_MOTOR_RIGHT_PWM_PERIOD(period);
    turn_motor_ticks(ticks, ticks);

    disable_motor_left();
    disable_motor_right();
}

void rover_set_constant_speed(float left_speed, float right_speed){
    set_individual_motor_speed(MOTOR_LEFT, left_speed);
    set_individual_motor_speed(MOTOR_RIGHT, right_speed);
}


// INTERNAL FUNCTIONS -----------------------------------------------------

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
    while(motor_right_steps > 0 && motor_left_steps > 0) {}
}

// Set the fixed speed of an individual motor. The speed will remain constant until 
// a function like rover_turn_degrees is called, or this function is called again.
void set_individual_motor_speed(unsigned short motor_id, float speed){
    // divide speed by the wheel circumferance to get the number of revolutions per second
    unsigned short direction = convert_to_direction(speed);
    float revolutions_per_second = absolute_value_float(speed) / (PI * WHEEL_DIAMETER);
    int ticks_per_second = revolutions_per_second * STEPS_PER_REVOLUTION;

    if(motor_id & MOTOR_LEFT){
        enable_motor_left();
        MOTOR_LEFT_DIRECTION = direction;
        SET_MOTOR_LEFT_PWM_PERIOD(ticks_per_second);
    }

    if (motor_id & MOTOR_RIGHT){
        enable_motor_right();
        MOTOR_RIGHT_DIRECTION = direction;
        SET_MOTOR_RIGHT_PWM_PERIOD(ticks_per_second);
    }
}

unsigned short convert_to_direction(float input){
    if(input < 0) return MOTOR_BWD;
    else return MOTOR_FWD;
}

// HARDWARE FUNCTIONS ----------------------------------------------------- 

void motor_left_tick() {
    motor_left_steps -= 1;

    if(motor_left_steps <= 0){
        motor_left_steps = 0;
        MOTOR_LEFT_INTERRUPT_ENABLE_FLAG = DISABLED;
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