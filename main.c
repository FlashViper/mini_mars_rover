#include "rover_sensors.h"
#include "rover_logic.h"
#include "rover_timing.h"
#include "rover_motors.h"
#include "debug.h"


int main(){
    initialize_rover_timing();
    initialize_rover_sensors();
    initialize_rover_motors();
    initialize_debug();
    
    initialize_rover_logic();
    
    while(1){
        disable_debug_led();
        wait_for_input();
        enable_debug_led();
        
        run_milestone_5(14, 0.8f, 0.75f);

        // rover_logic_run();
    }
    return 0;
}