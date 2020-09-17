#include "phase_corr_pwm.h"

const uint8_t DDR_OUT = 0xff;


void setPhaseCorrectPWM (uint8_t powerSignal1, uint8_t powerSignal2) {
    cli();

    DDRD = DDR_OUT;

    // mise à un des sorties OC1A et OC1B sur comparaison
    // réussie en mode PWM 8 bits, phase correcte
    // et valeur de TOP fixe à 0xFF (mode #1 de la table 17-6
    // page 177 de la description technique du ATmega324PA)
    OCR1A = powerSignal1;

    OCR1B = powerSignal2;

    // division d'horloge par 8 - implique une frequence de PWM fixe

    TCCR1A |= (1 << WGM10) | (1 << COM1A1) | (1 << COM1B1);
    TCCR1B |= (1 << CS11);
    TCCR1C = 0;

    sei();
    
}



