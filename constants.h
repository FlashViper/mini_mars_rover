#ifndef ROVER_CONSTANTS
#define ROVER_CONSTANTS

#define OUTPUT 0
#define INPUT 1

#define DIGITAL 0
#define ANALOG 1

#define PI 3.14159265f
#define OSCILATOR_SPEED 8e6

#define TRUE 1
#define FALSE 0

#define ENABLED 1
#define DISABLED 0

#define CLOCKWISE 0
#define COUTERCLOCKWISE 1

#define MOTOR_FWD 0
#define MOTOR_BWD 1

// learned about this handy trick from stackoverflow
// it lets you pass around a function as a variable :)
// https://stackoverflow.com/questions/9410/how-do-you-pass-a-function-as-a-parameter-in-c
typedef void (*function)();

// use this to replace complicated interrupt declarations
#define INTERRUPT void __attribute__((interrupt, no_auto_psv))

#endif