#include "rover_sensors.h"
#include "rover_logic.h"
#include "rover_timing.h"
#include "rover_motors.h"
#include "debug.h"

int t2flag = 0;
int toggle = 0;

int main(){

    initialize_rover_timing();
    initialize_rover_sensors();
    initialize_rover_motors();
    initialize_debug();
    
    T2CONbits.TCKPS = 0b11;
    disable_debug_led();

    while(1){
        while(!DEBUG_BUTTON_PRESSED){
            
        }
        
        toggle = 1;

        rover_turn_degrees(90.0f, 2.0, 0.0f);

        while(!t2flag){
            if(toggle){
                if(!DEBUG_BUTTON_PRESSED){
                    toggle = 0;
                }
            } else if (DEBUG_BUTTON_PRESSED) {
                break;
            }

        }

        disable_motor(MOTOR_BOTH);
    }

    return 0;
    rover_turn_degrees(90.0f, 1.0f, 0.0f);

    // initialize_rover_logic();
    
    // while(1){
    //     rover_logic_run();
    // }
    // return 0;
}