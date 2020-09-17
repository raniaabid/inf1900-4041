/*
Programme qui allume une del en rouge pendant une seconde lorsqu'un boutton-poussoir a été enfoncé et relaché 5 fois.
file Probleme1.cpp
author Laurence Robert (1960716) & Hugo Palisson (1957983)
date 2019-01-17
Créé le 2019-01-14


Tableau d'états : 

| État présent | Entrée "counterUnpressed" | État suivant | Sortie "PORTB" |
|              |                           |              |                |
|     INIT     |             0             |     INIT     |       00       |
|     INIT     |             1             |     INIT     |       00       |
|     INIT     |             2             |     INIT     |       00       |
|     INIT     |             3             |     INIT     |       00       |
|     INIT     |             4             |     INIT     |       00       |
|     INIT     |             5             |    DEL_RED   |       00       |
|    DEL_RED   |             X             |     INIT     |       10       |

*/

#define F_CPU 8000000

#include <util/delay.h>
#include <avr/io.h>

const uint8_t DDR_OUT = 0xff;
const uint8_t DDR_GREEN = 01;
const uint8_t DDR_RED = 10;
const uint8_t DDR_OFF = 00;

const uint8_t BUTTON_PRESSED = 0x04;
const uint8_t BUTTON_UNPRESSED = 0x00;

const double DELAY_REBOUND = 10;
const double SECOND_DELAY = 1000;

const int BUTTON_COUNT = 5;

uint8_t previousPin = BUTTON_UNPRESSED;


bool rebound(){
    previousPin = PIND;

    _delay_ms(DELAY_REBOUND);

    return PIND == previousPin;
}
    
bool wasButtonReleased(){
    return !rebound() && PIND == BUTTON_UNPRESSED; //!reboud() pour vérifier que le changement d'état du boutton ne soit pas dû à un court faux-contact.
}

void turnDelRed(){
    PORTB = DDR_RED;
    _delay_ms(SECOND_DELAY);
    PORTB = DDR_OFF;
}

enum State{
    INIT,
    DEL_RED
};

void stateActions(State& state, int& counterUnpressed){
    switch(state){
        case INIT: 
            if(wasButtonReleased()){
                ++counterUnpressed;
            }

            if(counterUnpressed%BUTTON_COUNT == 0 && counterUnpressed != 0){
                counterUnpressed = 0;
                state = DEL_RED;
            }
            break;

        case DEL_RED:
            turnDelRed();
            state = INIT;
            break;
    }
}

int main(){
    State state = INIT;
    DDRB = DDR_OUT;
    
    int counterUnpressed = 0;

    while(true){

        stateActions(state, counterUnpressed);

    }
    
} 