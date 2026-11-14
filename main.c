#pragma config OSCIOFNC = OFF // turn off the clock output so we can use pin 8 as a digital io
// #pragma config FNOSC=FRC // 8 MHz oscilator

#include "rover_sensors.h"
#include "rover_logic.h"
#include "rover_timing.h"
#include "rover_motors.h"
#include "debug.h"
#include "laser.h"
#include "rover_milestones.h"

int main(){
    initialize_rover_timing();
    initialize_rover_sensors();
    initialize_rover_motors();
    initialize_laser();
    initialize_debug();

    while(1){
        // if(_RB13 || _RB2 || _RB4){
        if(sensor_distance_forward || sensor_line_left || sensor_line_right){
            enable_debug_led();
        } else {
            disable_debug_led();
        }

    }

    if(DEBUG_BUTTON_PRESSED){
        debug_proximity_sensors();
        debug_qrd_sensors();
    }
    
    initialize_rover_logic();
    enable_debug_led();
    wait_for_input();
    
    while(1){        
        rover_logic_run();
    }
    
    return 0;
}