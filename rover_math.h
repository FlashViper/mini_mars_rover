#ifndef ROVER_MATH
#define ROVER_MATH


float sign_float(float input);
int sign_int(int input);



float sign_float(float input){
    if(input > 0) return 1.0f;
    if(input < 0) return -1.0f;
    return 0.0f;
}


int sign_int(int input){
    if(input > 0) return 1;
    if(input < 0) return -1;
    return 0;
}

int absolute_value_int(int input){
    return (input >= 0) ? input : -input;
}

float absolute_value_float(float input){
    return (input >= 0) ? input : -input;
}

#endif