#ifndef ROVER_TIMING
#define ROVER_TIMING

#define TIMER_1 0
#define TIMER_2 0

void initialize_rover_timing();
void set_timer_ticks_per_second(unsigned short timer_id, unsigned int ticks_per_second);


#endif