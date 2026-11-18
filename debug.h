#ifndef ROVER_DEBUG
#define ROVER_DEBUG

#include "xc.h"
#include "constants.h"
#include "rover_sensors.h"

#define DEBUG_LED _LATB7
#define DEBUG_BUTTON_PRESSED PORTAbits.RA2

#define DEBUG_LEFT_LINE_SENSOR_SEEING_WHITE _RB15
#define DEBUG_CENTER_LINE_SENSOR_SEEING_WHITE _RB14
#define DEBUG_RIGHT_LINE_SENSOR_SEEING_WHITE _RB13

#define DEBUG_WALL_ON_LEFT _RB2
#define DEBUG_WALL_AHEAD _RB13
#define DEBUG_WALL_ON_RIGHT _RA4


unsigned int debug_led_enabled = FALSE;

void initialize_debug(){
    // _ANSB14 = DIGITAL;
    _TRISB7 = OUTPUT;

    _ANSA2 = DIGITAL;
    _TRISA2 = INPUT;
}

void enable_debug_led(){
    debug_led_enabled = TRUE;
    DEBUG_LED = debug_led_enabled;
}

void disable_debug_led(){
    debug_led_enabled = FALSE;
    DEBUG_LED = debug_led_enabled;
}

void set_debug_led_state(unsigned short state){
    if(state) enable_debug_led();
    else disable_debug_led();
}

void debug_qrd_sensors(){
    disable_debug_led();
    while(DEBUG_BUTTON_PRESSED){}

    while(!DEBUG_BUTTON_PRESSED){
        if(!DEBUG_LEFT_LINE_SENSOR_SEEING_WHITE){
            enable_debug_led();
        } else {
            disable_debug_led();
        }
    }

    while(DEBUG_BUTTON_PRESSED){}
    
    while(!DEBUG_BUTTON_PRESSED){
        if(!DEBUG_CENTER_LINE_SENSOR_SEEING_WHITE){
            enable_debug_led();
        } else {
            disable_debug_led();
        }
    }
    
    while(DEBUG_BUTTON_PRESSED){}
    while(!DEBUG_BUTTON_PRESSED){
        if(!DEBUG_RIGHT_LINE_SENSOR_SEEING_WHITE){
            enable_debug_led();
        } else {
            disable_debug_led();
        }
    }

    while(DEBUG_BUTTON_PRESSED){}
}


void debug_proximity_sensors(){
    disable_debug_led();

    while(DEBUG_BUTTON_PRESSED){}
    
    while(!DEBUG_BUTTON_PRESSED){
        const float test_distance = 20.0f;
        // if(_RB13 || _RB2 || _RB4){
        if(get_wall_distance_center() < test_distance || get_wall_distance_left() < test_distance || get_wall_distance_right() < test_distance){
            enable_debug_led();
        } else {
            disable_debug_led();
        }
    }

    while(DEBUG_BUTTON_PRESSED){}
}

void debug_proximity_sensors_individually(){
    disable_debug_led();
    const float test_distance = 5.0f;

    while(DEBUG_BUTTON_PRESSED){}

    while(!DEBUG_BUTTON_PRESSED){
        set_debug_led_state(get_wall_distance_left() < test_distance);
    }

    while(DEBUG_BUTTON_PRESSED){}
    
    while(!DEBUG_BUTTON_PRESSED){
        set_debug_led_state(get_wall_distance_center() < test_distance);
    }
    
    while(DEBUG_BUTTON_PRESSED){}
    while(!DEBUG_BUTTON_PRESSED){
        set_debug_led_state(get_wall_distance_right() < test_distance);
    }

    while(DEBUG_BUTTON_PRESSED){}

}
#endif