#define F_CPU 8000000

#include <util/delay.h>
#include <avr/io.h> 




const uint8_t DDR_OUT = 0xff;
const uint8_t DDR_GREEN = 01;
const uint8_t DDR_RED = 10;
const uint8_t DDR_OFF = 00;


const double FADING_TIME = 3;           //s
const double FREQUENCY = 1000;          //Hz
const double PERIOD = 1/FREQUENCY;      //s
const double TO_MICR0SECOND = 1000000;
const double MAX_INTENSITY = 1;
const int LATENCY_REDUCER = 10;


void variableMicroSecDelay(int delay){
    
    for(int i = 0; i < delay/LATENCY_REDUCER; ++i){
        _delay_us(1*LATENCY_REDUCER);

    }
}

int main()
{
    double intensity = MAX_INTENSITY;
    int8_t couleur = DDR_RED;
    double highTime = 0;
    double lowTime = 0;


    DDRB = DDR_OUT;

    while(true){

        intensity = MAX_INTENSITY;
        
        for(double i = 0; i < FADING_TIME ;i = i + PERIOD){
            PORTB = couleur;

            highTime = intensity*PERIOD;   

            variableMicroSecDelay(highTime*TO_MICR0SECOND);

            PORTB = DDR_OFF;


            lowTime = PERIOD - highTime;         

            variableMicroSecDelay(lowTime*TO_MICR0SECOND);

            intensity = intensity - MAX_INTENSITY*(PERIOD/FADING_TIME);

        }

        couleur = (couleur == DDR_RED)? DDR_GREEN : DDR_RED;
         
        

    }
    return 0;
}