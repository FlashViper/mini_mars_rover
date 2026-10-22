#ifndef ROVER_TIMING
#define ROVER_TIMING

#include "xc.h"
#include "constants.h"

#define TIMER_1 T2CONbits
#define TIMER_2 0
#define PRESCALE_1 0b10
#define PRESCALE_8 0b10
#define PRESCALE_64 0b10
#define PRESCALE_128 0b11

#define T5_PRESCALER 64

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

int flag_timer_1_elapsed = 1;

void set_pwm_1_period(int pulses_per_second);
void set_pwm_1_duty_cycle(float duty_cycle); // cycle is between 0 (0%) and 1 (100%)
void set_pwm_2_period(int pulses_per_second);
void set_pwm_2_duty_cycle(float duty_cycle); // cycle is between 0 (0%) and 1 (100%)



void initialize_rover_timing() {
    // clear all internal flags
    flag_timer_1_elapsed = FALSE;

    // configure timer 5 as a motor timer with a prescaler of 256
    T5CON = 0x0000;
    T4CONbits.T32 = 0; // disable 32-bit timer mode (which is stored in T4CONbits, not T5)
    T5CONbits.TON = 1;
    T5CONbits.TCKPS = PRESCALE_64;
    T5CONbits.TCS = 0;
    T5CONbits.TGATE = 0;


    // Initialize pwm one on pin [XX]
    OC1CON1 = 0x1c06; // magic registers
    OC1CON2 = 0x001f; // ditto

    // set pwm pulses to timer 5 instead of built in oscilator
    OC1CON1bits.OCTSEL = 0b011;
    
    // Initialize pwm two on pin B0
    OC2CON1 = 0x1c06; // magic registers
    OC2CON2 = 0x001f; // ditto
    // set pwm pulses to timer 5 instead of built in oscilator
    OC2CON1bits.OCTSEL = 0b011;


    T1CON = 0x0000; // reset timer 1
    TIMER_1.TON = 0; // disable timer 1 by default
    TIMER_1.TCKPS = PRESCALE_64;
    TIMER_1.TCS = 0;
}


INTERRUPT _T1Interrupt (void){
    _T1IF = 0; // clear the interrupt flag
    flag_timer_1_elapsed = TRUE;
}


void set_pwm_1_period(int pulses_per_second) {
    pwm_1_period = OSCILATOR_SPEED / T5_PRESCALER / pulses_per_second / 2; // math?
    PWM_1_PERIOD_REGISTER = pwm_1_period;
    set_pwm_1_duty_cycle(pwm_1_duty_cycle); // update the duty cycle
}

// cycle is between 0 (0%) and 1 (100%)
void set_pwm_1_duty_cycle(float duty_cycle) {
    // clamp between 0 and 1
    if(duty_cycle < 0.001f) pwm_1_duty_cycle = 0.0f;
    else if (duty_cycle > 1.0f) pwm_1_duty_cycle = 1.0f;
    else pwm_1_duty_cycle = duty_cycle;

    PWM_1_DUTY_CYCLE_REGISTER = pwm_1_duty_cycle * pwm_1_period;
}

void set_pwm_2_period(int pulses_per_second) {
    pwm_2_period = OSCILATOR_SPEED / T5_PRESCALER / pulses_per_second / 2; // math?
    PWM_2_PERIOD_REGISTER = pwm_2_period;
    set_pwm_2_duty_cycle(pwm_2_duty_cycle); // update the duty cycle
}

// cycle is between 0 (0%) and 1 (100%)
void set_pwm_2_duty_cycle(float duty_cycle) {
    // clamp between 0 and 1
    if(duty_cycle < 0.0f) pwm_2_duty_cycle = 0.0f;
    else if (duty_cycle > 1.0f) pwm_2_duty_cycle = 1.0f;
    else pwm_2_duty_cycle = duty_cycle;

    PWM_2_DUTY_CYCLE_REGISTER = pwm_2_duty_cycle * pwm_2_period;
}


void set_timer_ticks_per_second(unsigned short timer_id, unsigned int ticks_per_second){
    // TO DO
}
#endif