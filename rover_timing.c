#ifndef ROVER_TIMING
#define ROVER_TIMING

#include "xc.h"
#include "constants.c"

#define TIMER_1 0
#define TIMER_2 0

#define PWM_1_PERIOD_REGISTER OC1RS
#define PWM_1_DUTY_CYCLE_REGISTER OC1R

#define PWM_2_PERIOD_REGISTER OC2RS
#define PWM_2_DUTY_CYCLE_REGISTER OC2R

void initialize_rover_timing();
void set_timer_ticks_per_second(unsigned short timer_id, unsigned int ticks_per_second);

// static so that other scripts can't access or set them directly
// Note: setting them directly won't work. call set_pwm_X_(period/duty_cycle)
static int pwm_1_period;
static int pwm_2_period;
static float pwm_1_duty_cycle = 0.5;
static float pwm_2_duty_cycle = 0.5;

void set_pwm_1_period(int pulses_per_second);
void set_pwm_1_duty_cycle(float duty_cycle); // cycle is between 0 (0%) and 1 (100%)
void set_pwm_2_period(int pulses_per_second);
void set_pwm_2_duty_cycle(float duty_cycle); // cycle is between 0 (0%) and 1 (100%)



void initialize_rover_timing() {
    // Initialize pwm one on pin [XX]
    OC1CON1 = 0x0000; // reset register 1 of pwm
    OC1CON2 = 0x0000;
    OC1CON2bits.SYNCSEL = 0x1f;
    OC1CON2bits.OCTSEL = 0b111; // ???
    OC1CON2bits.OCTRIG = 0;
    OC1CON1bits.OCM = 0b110;

    // Initialize pwm two on pin B0
    OC2CON1 = 0x0000;
    OC2CON2 = 0x0000;
    OC2CON2bits.SYNCSEL = 0x1f;
    OC2CON2bits.OCTSEL = 0b111; // ???
    OC2CON2bits.OCTRIG = 0;
    OC2CON1bits.OCM = 0b110;
}


void set_pwm_1_period(int pulses_per_second) {
    pwm_1_period = OSCILATOR_SPEED / pulses_per_second; // math?
    PWM_1_PERIOD_REGISTER = pwm_1_period;
    set_pwm_1_duty_cycle(pwm_1_duty_cycle); // update the duty cycle
}

// cycle is between 0 (0%) and 1 (100%)
void set_pwm_1_duty_cycle(float duty_cycle) {
    // clamp between 0 and 1
    if(duty_cycle < 0.0f) pwm_1_duty_cycle = 0.0f;
    else if (duty_cycle > 1.0f) pwm_1_duty_cycle = 1.0f;
    else pwm_1_duty_cycle = duty_cycle;

    PWM_1_DUTY_CYCLE_REGISTER = pwm_1_period * pwm_1_period;
}

void set_pwm_2_period(int pulses_per_second) {
    pwm_2_period = OSCILATOR_SPEED / pulses_per_second; // math?
    PWM_2_PERIOD_REGISTER = pwm_2_period;
    set_pwm_2_duty_cycle(pwm_2_duty_cycle); // update the duty cycle
}

// cycle is between 0 (0%) and 1 (100%)
void set_pwm_2_duty_cycle(float duty_cycle) {
    // clamp between 0 and 1
    if(duty_cycle < 0.0f) pwm_2_duty_cycle = 0.0f;
    else if (duty_cycle > 1.0f) pwm_2_duty_cycle = 1.0f;
    else pwm_2_duty_cycle = duty_cycle;

    PWM_2_DUTY_CYCLE_REGISTER = pwm_2_period * pwm_2_period;
}
#endif