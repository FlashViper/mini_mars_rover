#include "rover_sensors.h"
#include "rover_logic.h"
#include "rover_timing.h"
#include "rover_motors.h"
#include "debug.h"
#include "rover_milestones.h"

int main(){
    initialize_rover_timing();
    initialize_rover_sensors();
    initialize_rover_motors();
    initialize_debug();
    
    initialize_rover_logic();
    enable_debug_led();
    wait_for_input();
    disable_debug_led();
    
    if(!sensor_line_center){
        rover_turn_degrees(45, 0.2, 0, CLOCKWISE);
        rover_turn_degrees(45, 0.2, 0, COUNTERCLOCKWISE);
        while(1){}
        return 0;
    } else {
        test_line_follow();
        return 0;
    }
    
    while(1){        
        rover_logic_run();
    }
    return 0;
}