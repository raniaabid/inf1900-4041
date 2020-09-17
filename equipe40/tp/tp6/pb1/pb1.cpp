/*
Programme qui compte 10 fois le nombre de secondes pendant lesquelles on appui sur le bouton-poussoir
(max 12 secondes), puis après que la DEL ait clignoté en vert, elle clignote en rouge pendant 
5 fois le temps durant lequel on a appuyé sur le bouton-poussoir. Après, la DEL s'allume en vert
pendant une seconde puis revient à sont état initial et attend que le bouton-poussoir soit enfoncé.
file pb1.cpp
author Laurence Robert (1960716) & Hugo Palisson (1957983)
date 2019-02-18
Créé le 2019-02-11


Tableau d'états : 

|         État présent          |            Condition de changement d'état             |          État suivant         | Sortie "PORTB" (Couleur) |
|                               |                                                       |                               |                          |
|             INIT              |        -Nothing                                       |             INIT              |           OFF            |
|             INIT              |        -Button is pressed                             |       INCREMENT_COUNTER       |           OFF            |
|       INCREMENT_COUNTER       |        -Button is kept pressed                        |       INCREMENT_COUNTER       |           OFF            |
|       INCREMENT_COUNTER       |        -Button is released or 12                      |       DEL_FLASHES_GREEN       |           OFF            |
|                               |         seconds have passed in this state             |                               |                          |
|       DEL_FLASHES_GREEN       |        -Nothing                                       |       DEL_FLASHES_GREEN       |      GREEN (FLASH)       |
|       DEL_FLASHES_GREEN       |        -0.5 second has passed in this state           |    WAIT_AND_DEL_LIGHTS_RED    |      GREEN (FLASH)       |
|    WAIT_AND_DEL_LIGHTS_RED    |        -Nothing                                       |    WAIT_AND_DEL_LIGHTS_RED    |           RED            |
|    WAIT_AND_DEL_LIGHTS_RED    |        -2 seconds have passed in this state           |         DEL_FLASHES_RED       |           RED            |
|         DEL_FLASHES_RED       |        -Nothing                                       |         DEL_FLASHES_RED       |       RED (FLASH)        |
|         DEL_FLASHES_RED       |        -Has stayed in this state for 5 times the      |        DEL_LIGHTS_GREEN       |       RED (FLASH)        |
|                               |         time it stayed in the INCREMENT_COUNTER state |                               |                          |
|        DEL_LIGHTS_GREEN       |        -Nothing                                       |        DEL_LIGHTS_GREEN       |          GREEN           |
|        DEL_LIGHTS_GREEN       |        -1 second has passed in this state             |             INIT              |          GREEN           |



*/


#define F_CPU 8000000


#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

const uint8_t DDR_IN = 0x00;
const uint8_t DDR_OUT = 0xff;
const uint8_t DDR_VERT = 01;    //B0-B1
const uint8_t DDR_ROUGE = 10;   //B0-B1
const uint8_t DDR_OFF = 00;

const uint8_t BUTTON_PRESSED = 0x0C;
const uint8_t BUTTON_UNPRESSED = 0x00;
const uint8_t D3_INPUT = 0x04;
const uint8_t D4_INPUT = 0x08;

const int ONE_SECOND = 1000;    //ms
const int DELAY_REBOUND = 30;    //ms

uint8_t previousInput = BUTTON_UNPRESSED;

uint8_t compteur = 0;



enum State {
    INIT,
    INCREMENT_COUNTER,
    DEL_FLASHES_GREEN,
    WAIT_AND_DEL_LIGHTS_RED,
    DEL_FLASHES_RED,
    DEL_LIGHTS_GREEN
};

State state = INIT;

bool rebound(){
    previousInput = PIND;

    _delay_ms(DELAY_REBOUND); 

    return ((PIND & BUTTON_PRESSED) == (previousInput & BUTTON_PRESSED));
}


void stateAction(){

    switch (state){
        case INIT: 

            if(!(PIND & D3_INPUT) && rebound()){
                state = INCREMENT_COUNTER;
            }
            break;

        case INCREMENT_COUNTER: 
            _delay_ms(ONE_SECOND/10);
            ++compteur;
            if(compteur >= 120 || (PIND & D3_INPUT))
                state = DEL_FLASHES_GREEN;
            break;

        case DEL_FLASHES_GREEN: 
            PORTB = DDR_VERT;
            _delay_ms(ONE_SECOND/2);
            PORTB = DDR_OFF;
            state = WAIT_AND_DEL_LIGHTS_RED;

            break;
                
        case WAIT_AND_DEL_LIGHTS_RED: 
            _delay_ms(2*ONE_SECOND);
            PORTB = DDR_ROUGE;
            state = DEL_FLASHES_RED;
            
            break;

        case DEL_FLASHES_RED: 
            if(compteur > 1){
                compteur -= 2;
            }
            else
            {
                compteur = 0;
            }

            PORTB = DDR_ROUGE;
            _delay_ms(ONE_SECOND/4);
            PORTB = DDR_OFF;
            _delay_ms(ONE_SECOND/4);

            if(compteur == 0){
                state = DEL_LIGHTS_GREEN;
            }
                    
            break;
            
        case DEL_LIGHTS_GREEN: 
            PORTB = DDR_VERT;
            _delay_ms(ONE_SECOND);
            PORTB = DDR_OFF;

            state = INIT;

            break;
   
    }

}


int main(){
    DDRB = DDR_OUT;
    DDRD = DDR_IN;

    while(true){
        stateAction();
    }

}