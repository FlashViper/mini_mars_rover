#include "rover_sensors.h"
#include "rover_logic.h"
#include "constants.h"
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
        
        enable_debug_led();
        float distance_fwd = 1.0f;
        rover_move_distance(distance_fwd, 3.0f, MOTOR_FWD);
        rover_turn_degrees(90.0f, 2.5f, 0.0f, CLOCKWISE);
        rover_move_distance(distance_fwd, 3.0f, MOTOR_FWD);
        rover_turn_degrees(180.0f, 2.5f, 0.0f, CLOCKWISE);
        rover_move_distance(distance_fwd, 3.0f, MOTOR_FWD);
        disable_debug_led();


        // rover_turn_degrees(45.0f, 2.5f, 0.0f, CLOCKWISE);
        
        // while(!t2flag){
        //     if(toggle){
        //         if(!DEBUG_BUTTON_PRESSED){
        //             toggle = 0;
        //         }
        //     } else if (DEBUG_BUTTON_PRESSED) {
        //         break;
        //     }

        // }

        // disable_motor_left();
        // disable_motor_right();
    }

    return 0;

    // initialize_rover_logic();
    
    // while(1){
    //     rover_logic_run();
    // }
    // return 0;
}