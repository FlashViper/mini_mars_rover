#ifndef ROVER_SERVO
#define ROVER_SERVO

#include "rover_timing.h"

#define SERVO_FREQUENCY 50.0f * 2 // servo runs on 50 Hz
#define MIN_ANGLE_PULSE_LENGTH 0.5e-3f // seconds
#define MAX_ANGLE_PULSE_LENGTH 2.5e-3f // seconds
#define ANGLE_MIN 0
#define ANGLE_MAX 180

void initialize_servo(){
    // configure servo on pin 5 (B1)
    _ANSB1 = DIGITAL;
    set_pwm_3_period(SERVO_FREQUENCY);
}

void set_servo_angle(float angle){
    float corrected_angle = angle;
    if(corrected_angle > ANGLE_MAX) corrected_angle = ANGLE_MAX;
    if(corrected_angle < ANGLE_MIN) corrected_angle = ANGLE_MIN;

    float angle_percent = (corrected_angle - ANGLE_MIN) / (ANGLE_MAX - ANGLE_MIN);
    float duty_cycle_seconds = angle_percent * (MAX_ANGLE_PULSE_LENGTH - MIN_ANGLE_PULSE_LENGTH) + MIN_ANGLE_PULSE_LENGTH;
    float cycle_percent = duty_cycle_seconds * SERVO_FREQUENCY;
    set_pwm_3_duty_cycle(cycle_percent);
}

#endif


// #include "xc.h"
// #pragma FNOSC = FRCDIV // Default postscaler is divide by 2

// int main(void) {
//     _RCDIV = 0b010; //Divide by 4 postscaler
//     OC3CON1 = 0;
//     OC3CON2 = 0;
//     OC3CON1bits.OCTSEL = 0b111;
//     OC3CON2bits.SYNCSEL = 0b11111;
//     OC3CON2bits.OCTRIG = 0;
//     OC3RS = 20000;
//     OC3R = 500;
    
//     OC3CON1bits.OCM = 0b110; // Start PWM on pin 14. Set to zero to disable.
    
//     while(1){
//         while(OC3R<1000)
//         {
//             OC3R++;
            
//             for(int i = 0; i<100; i++);
//         }
//         while(OC3R>500)
//         {
//             OC3R--;
            
//             for(int i = 0; i<100; i++);
//         }
//     }        
    
//     return 0;
// }