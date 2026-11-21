#ifndef ROVER_SERVO
#define ROVER_SERVO

#include "rover_timing.h"

#define SERVO_FREQUENCY 50.0f // servo runs on 50 Hz
#define MIN_ANGLE_PULSE_LENGTH 0.5e-3f // seconds
#define MAX_ANGLE_PULSE_LENGTH 2.5e-3f // seconds
#define ANGLE_MIN 0
#define ANGLE_MAX 180

void initialize_servo(){
    set_pwm_3_period(SERVO_FREQUENCY);
}

void set_servo_angle(float angle){
    float corrected_angle = angle;
    if(corrected_angle > ANGLE_MAX) corrected_angle = ANGLE_MAX;
    if(corrected_angle < ANGLE_MIN) corrected_angle = ANGLE_MIN;

    float angle_percent = (corrected_angle - ANGLE_MIN) / (ANGLE_MAX - ANGLE_MIN);
    float duty_cycle_seconds = angle_percent * (MAX_ANGLE_PULSE_LENGTH - MIN_ANGLE_PULSE_LENGTH) + MIN_ANGLE_PULSE_LENGTH;
    float cycle_percent = duty_cycle_seconds / SERVO_FREQUENCY;
    set_pwm_3_duty_cycle(cycle_percent);
}

#endif