#include "vari_delay.h"

const uint8_t LATENCY_REDUCER = 10;


void variableMicroSecDelay(int delay){
    
    for(int i = 0; i < delay/LATENCY_REDUCER; ++i){
        _delay_us(1*LATENCY_REDUCER);

    }
}

void variableMiliSecDelay(int delay){
    for(int i = 0; i < delay; ++i){
        _delay_ms(1);
    }
}