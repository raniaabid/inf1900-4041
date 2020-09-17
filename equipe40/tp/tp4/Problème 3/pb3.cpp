#define F_CPU 8000000

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>


const uint8_t DDR_OUT = 0xff;
const uint8_t DDR_GREEN = 01;
const uint8_t DDR_RED = 10;
const uint8_t DDR_OFF = 00;

const uint8_t MAX_INTENSITY = 0xFF;

const uint8_t START_TIMER_PRESCALE_1024 = (1 << CS12) | (1 << CS10);
const uint8_t START_TIMER_PRESCALE_8= (1 << CS11);
const uint8_t TIME_PER_INTENSITY_STATE = 2;	//s
const uint8_t FREQUENCY = 60;				//Hz
const double PERIOD = 1/FREQUENCY;		//s

const int SECOND_DELAY = 1000;          //ms



enum IntensityState{
    I_0,
    I_25,
    I_50,
    I_75,
    I_100
};

IntensityState i_State = I_0;


void changeStates(){
	switch (i_State){
		case I_0:
			i_State = I_25;
			OCR1A = 0.25*MAX_INTENSITY;
            OCR1B = 0.25*MAX_INTENSITY;
			break;
		case I_25:
			i_State = I_50;
			OCR1A = 0.50*MAX_INTENSITY;
            OCR1B = 0.50*MAX_INTENSITY;
			break;
		case I_50:
			i_State = I_75;
			OCR1A = 0.75*MAX_INTENSITY;
            OCR1B = 0.75*MAX_INTENSITY;
			break;
		case I_75:
			i_State = I_100;
			OCR1A = 1*MAX_INTENSITY;
            OCR1B = 1*MAX_INTENSITY;
			break;
		case I_100:
			i_State = I_0;
			OCR1A = 0;
            OCR1B = 0;
			break;
	}
}


void ajustementPWM () {
    cli();
    // mise à un des sorties OC1A et OC1B sur comparaison
    // réussie en mode PWM 8 bits, phase correcte
    // et valeur de TOP fixe à 0xFF (mode #1 de la table 17-6
    // page 177 de la description technique du ATmega324PA)
    OCR1A = 0;

    OCR1B = 0;

    // division d'horloge par 8 - implique une frequence de PWM fixe


    TCCR1A |= (1 << WGM10) | (1 << COM1A1) | (1 << COM1B1);
    TCCR1B |= (1 << CS11);
    TCCR1C = 0;

    sei();
    
}


int main(){

    DDRD = DDR_OUT;
    

    ajustementPWM();
    TCNT1 = START_TIMER_PRESCALE_8;


    while(true){
        _delay_ms(2*SECOND_DELAY);

        changeStates();

    }
}
