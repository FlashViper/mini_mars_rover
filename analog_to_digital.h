#ifndef A2D
#define A2D

#include "xc.h"
#include "constants.h"

#define UNLOCK_ANALOG_TO_DIGITAL_CONFIGURATION() _ADON = OFF
#define LOCK_ANALOG_TO_DIGITAL_CONFIGURATION() _ADON = ON

// copied straight off of learningsuite
// https://byu.app.box.com/s/bbi7hcwldgxpurbjmuden07zo9xwq1ly/file/2543496136338
void configure_analog_to_digital(unsigned short number_of_channels){
    UNLOCK_ANALOG_TO_DIGITAL_CONFIGURATION();    // AD1CON1<15> -- Turn off A/D during config
    
    // AD1CON1 register
    _ADSIDL = 0;  // AD1CON1<13> -- A/D continues in idle mode
    _MODE12 = 0;  // AD1CON1<10> -- 12-bit A/D operation
    _FORM = 0;    // AD1CON1<9:8> -- Unsigned integer output
    _SSRC = 7;    // AD1CON1<7:4> -- Auto conversion (internal counter)
    _ASAM = 1;    // AD1CON1<2> -- Auto sampling

    // AD1CON2 register
    _PVCFG = 0;   // AD1CON2<15:14> -- Use VDD as positive
                  // ref voltage
    _NVCFG = 0;   // AD1CON2<13> -- Use VSS as negative
                  // ref voltage
    _BUFREGEN = 1;// AD1CON2<11> -- Result appears in buffer
                  // location corresponding to channel
    _CSCNA = 1;   // AD1CON2<10> -- Scans inputs specified
                  // in AD1CSSx registers
    
                  // AD1CON2<6:2> -- Every 9th conversion sent (number of channels sampled -1)
    _SMPI = number_of_channels - 1; 
                  // to buffer (if sampling 10 channels)
    _ALTS = 0;    // AD1CON2<0> -- Sample MUXA only
    
    // AD1CON3 register
    _ADRC = 0;    // AD1CON3<15> -- Use system clock
    _SAMC = 0;    // AD1CON3<12:8> -- Auto sample every A/D
                  // period TAD
    _ADCS = 0b00000011;    // AD1CON3<7:0> -- TAD needs to be at least 600 ns. Thus, _ADCS = 0b00000010 will give us the fastest AD clock given a 4 MHz system clock. 0b00000011 is slower than we could go, but still fast enough for our needs.

    // AD1CSS registers
    // AD1CSSL = 0b0011111000011111; // choose A2D channels you'd like to scan here.
    AD1CSSL = 0; // choose A2D channels you'd like to scan here.
    
    LOCK_ANALOG_TO_DIGITAL_CONFIGURATION();    // AD1CON1<15> -- Turn on A/D
}

// returns pin voltage in millivolts
float read_pin_voltage(unsigned int pin_output){
    const float max_voltage = 3300; // millivolts
    const float register_size = 1024;
    
    return max_voltage * ((float)pin_output / register_size);
}

#endif