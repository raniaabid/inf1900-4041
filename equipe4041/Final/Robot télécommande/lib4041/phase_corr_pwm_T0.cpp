#include "phase_corr_pwm_T0.h"

const uint8_t DDR_OUT = 0xff;


void setPhaseCorrectPWM_T0 (uint8_t powerSignal1, uint8_t powerSignal2) {
    cli();

    //DDRB = DDR_OUT;

    TCCR0A = 0;
    TCCR0B = 0;
    
    // mise à un des sorties OC1A et OC1B sur comparaison
    // réussie en mode PWM 8 bits, phase correcte
    // et valeur de TOP fixe à 0xFF (mode #1 de la table 17-6
    // page 177 de la descriptio n technique du ATmega324PA)
    OCR0A = powerSignal1;

    OCR0B = powerSignal2;

    // division d'horloge par 8 - implique une frequence de PWM fixe
    
    TCNT0 = 0;

    TCCR0A |= (1 << WGM00);
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1);
    TCCR0B |= (1 << CS00) ;

    //TIMSK0 |= (1 << OCIE0A) | (1 << OCIE0A);
    
    sei();
    
}



