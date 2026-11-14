#ifndef ROVER_SENSORS
#define ROVER_SENSORS

#include "xc.h"
#include "analog_to_digital.h"
#include "constants.h"

#define CHANGE_NOTIFICATION _CNInterrupt(void)
#define INCHES_PER_MILLIVOLT 12.0f / 300.0f

#define LEFT_LINE_SENSOR_SEEING_WHITE _RB15
#define CENTER_LINE_SENSOR_SEEING_WHITE _RB14
#define RIGHT_LINE_SENSOR_SEEING_WHITE _RB13

#define WALL_ON_LEFT _RB2
#define WALL_AHEAD _RB12
#define WALL_ON_RIGHT _RB4

#define GET_ULTRASONIC_VOLTAGE_LEFT() read_pin_voltage(ADC1BUF4)
#define GET_ULTRASONIC_VOLTAGE_CENTER() read_pin_voltage(ADC1BUF14)
#define GET_ULTRASONIC_VOLTAGE_RIGHT() read_pin_voltage(ADC1BUF12)

static unsigned short sensor_distance_left = 0;
static unsigned short sensor_distance_forward = 0;
static unsigned short sensor_distance_right = 0;

static float distance_to_wall_left = 0.0f;
static float distance_to_wall_front = 0.0f;
static float distance_to_wall_right = 0.0f;

static unsigned short sensor_line_left = 0;
static unsigned short sensor_line_center = 0;
static unsigned short sensor_line_right = 0;

static unsigned short sensor_ball_color = 0;

static unsigned short sensor_ir_sample = 0;
static unsigned short sensor_ir_antenna = 0;

void update_line_sensors();

void initialize_rover_sensors(){
    // set up qrd sensors to fire the change notification interrupt whenever
    // one of them flips from high to low
    // (see page 166 of packet I pretty much copied it word for word)
    
    // Initialize color sensor (left)
    _ANSB15 = DIGITAL;
    _TRISB15 = INPUT; 
    _CN11IE = ENABLED; // enable change interrupt on pin 18 (Change notification 11)
    _CN11PUE = DISABLED; // disable pull up resistor
    _CN11PDE = DISABLED; // disable pull down resistor
    
    // (center)
    _ANSB14 = DIGITAL;
    _TRISB14 = INPUT; 
    _CN12IE = ENABLED; // enable change interrupt on pin 17 (Change notification 12)
    _CN12PUE = DISABLED; // disable pull up resistor
    _CN12PDE = DISABLED; // disable pull down resistor (we already have one)

    // (right)
    _ANSB13 = DIGITAL;
    _TRISB13 = INPUT;
    _CN13IE = ENABLED;
    _CN13PUE = DISABLED;
    _CN13PDE = DISABLED;

    // (ball)
    // _ANSB13 = DIGITAL;
    // _TRISB13 = INPUT;
    // _CN13IE = ENABLED;
    // _CN13PUE = DISABLED;
    // _CN13PDE = DISABLED;        
    
    // Initialize IR proximity sensors

    // Left (temporarily on pin 6 (B2, CN6) until we add in the laser photodiode)
    // _ANSB2 = DIGITAL;
    // _TRISB2 = INPUT;
    // _CN6IE = ENABLED;
    // _CN6PUE = DISABLED;
    // _CN6PDE = ENABLED;

    // // Right (pin 9 (B4, CN1))
    // _ANSB4 = DIGITAL;
    // _TRISB4 = INPUT;
    // _CN1IE = ENABLED;
    // _CN1PUE = DISABLED;
    // _CN1PDE = ENABLED;
    
    // // Center (pin 16 (B12, CN14))
    // _ANSB12 = DIGITAL;
    // _TRISB12 = INPUT;
    // _CN14IE = ENABLED;
    // _CN14PUE = DISABLED;
    // _CN14PDE = ENABLED;
    
    // Enable change notification interrupts
    _CNIE = ENABLED;
    _CNIP = 6; // set priority
    _CNIF = 0; // clear interrupt flag
    
    // Initialize Ultrasonic distance sensors
    // Left (temporarily on pin 6 (B2, CN6) until we add in the laser photodiode)
    _ANSB2 = ANALOG;
    _TRISB2 = INPUT;

    // // Right (pin 8 (A3, CN14))
    _ANSA3 = ANALOG;
    _TRISA3 = INPUT;

    // // Center (pin 16 (B12, CN14))
    _ANSB12 = ANALOG;
    _TRISB12 = INPUT;

    configure_analog_to_digital(3);
    UNLOCK_ANALOG_TO_DIGITAL_CONFIGURATION();
    _CSS4 = 1; // enable a2d on pin 6
    _CSS14 = 1; // enable a2d on pin 8
    _CSS12 = 1; // enable a2d on pin 16
    LOCK_ANALOG_TO_DIGITAL_CONFIGURATION();
    
    // Initialize IR Sensor (left)
    // Initialize IR Sensor (right)
    // etc

    update_line_sensors();

    // initialize pin change interrupts
}

void update_line_sensors(){
    // QRD sensors
    // invert because high voltage means it's over a dark surface
    // (condition) ? <true value> : <false value> is a shortcut for using if statements
    sensor_line_left = LEFT_LINE_SENSOR_SEEING_WHITE ? FALSE : TRUE;
    sensor_line_center = CENTER_LINE_SENSOR_SEEING_WHITE ? FALSE : TRUE;
    sensor_line_right = RIGHT_LINE_SENSOR_SEEING_WHITE ? FALSE : TRUE;
}

void update_proximity_sensors(){
    sensor_distance_left = WALL_ON_LEFT ? TRUE : FALSE;
    sensor_distance_forward = WALL_AHEAD ? TRUE : FALSE;
    sensor_distance_right = WALL_ON_RIGHT ? TRUE : FALSE;
}

unsigned short is_on_line() {
    return sensor_line_center || sensor_line_left || sensor_line_right;
}


float get_wall_distance_left(){
    return INCHES_PER_MILLIVOLT * GET_ULTRASONIC_VOLTAGE_LEFT();
}

float get_wall_distance_center(){
    return INCHES_PER_MILLIVOLT * GET_ULTRASONIC_VOLTAGE_CENTER();
}

float get_wall_distance_right(){
    return INCHES_PER_MILLIVOLT * GET_ULTRASONIC_VOLTAGE_RIGHT();
}


// INTERRUPTS -------------------------------------------------

// see pages 165 and 166 of packet
INTERRUPT CHANGE_NOTIFICATION {
    _CNIF = 0; // clear interrupt flag
    update_line_sensors();
    update_proximity_sensors();
}

#endif