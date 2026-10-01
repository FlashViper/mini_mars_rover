#include "rover_sensors.c"
#include "rover_logic.c"
#include "rover_timing.c"
#include "rover_motors.c"


int main(){
    initialize_rover_timing();
    initialize_rover_sensors();
    initialize_rover_logic();
    
    while(1){
        rover_poll_sensors();
        rover_logic_run();
    }
    return 0;
}