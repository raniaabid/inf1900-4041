/*
Programme qui change la couleur d'affichage d'une DEL selon un cycle de 6 états à chaque fois qu'un boutton poussoir est enfoncé ou relaché.
file Probleme2.cpp
author Laurence Robert (1960716) & Hugo Palisson (1957983)
date 2019-01-17
Créé le 2019-01-14


Tableau d'états : 

| État présent | Entrée "isStateChange" | État suivant | Sortie "PORTB" (Couleur) |
|              |                        |              |                          |
|     INIT     |         false          |     INIT     |           ROUGE          |
|     INIT     |         true           |    STATE_1   |           ROUGE          |
|    STATE_1   |         false          |    STATE_1   |           AMBRE          |
|    STATE_1   |         true           |    STATE_2   |           AMBRE          |
|    STATE_2   |         false          |    STATE_2   |           VERT           |
|    STATE_2   |         true           |    STATE_3   |           VERT           |
|    STATE_3   |         false          |    STATE_3   |           ROUGE          |
|    STATE_3   |         true           |    STATE_4   |           ROUGE          |
|    STATE_4   |         false          |    STATE_4   |           OFF            |
|    STATE_4   |         true           |    STATE_5   |           OFF            |
|    STATE_5   |         false          |    STATE_5   |           VERT           |
|    STATE_5   |         true           |     INIT     |           VERT           |


*/

#define F_CPU 8000000

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

const uint8_t DDR_SORTIE = 0xff;
const uint8_t DDR_VERT = 01;
const uint8_t DDR_ROUGE = 10;
const uint8_t DDR_OFF = 00;

const uint8_t BUTTON_PRESSED = 0x04;
const uint8_t BUTTON_UNPRESSED = 0x00;

const double DELAY_REBOUND = 30;        //ms
const double SECOND_DELAY = 1000;

const int NB_STATES = 6;


volatile bool isStateChange = false;
volatile uint8_t previousInput = BUTTON_UNPRESSED;




enum State {
    INIT,
    STATE_1,
    STATE_2,
    STATE_3,
    STATE_4,
    STATE_5
    
};

volatile State state = INIT;



bool rebound(){
    previousInput = PIND;

    _delay_ms(DELAY_REBOUND); 

    return PIND == previousInput;
}



bool hasInputChanged(uint8_t& previousState, bool& isVariation){   
      
    if (rebound() && isVariation){
        previousState = PIND;
        isVariation = false;
        return true;
    }

    isVariation = (PIND != previousState);
    previousState = PIND;
    
    return false;
}


void setDDRToAmber(int counter){
    if(counter%10 == 0)
        PORTB = DDR_ROUGE;
    else
        PORTB = DDR_VERT;
}

void stateAction(int counter){

    switch (state){
            case INIT: 
                PORTB = DDR_ROUGE;
                if(isStateChange)
                    state = STATE_1;
                break;

            case STATE_1: 
                setDDRToAmber(counter);
                if(isStateChange)
                    state = STATE_2;
                break;

            case STATE_2: 
                PORTB = DDR_VERT;
                if(isStateChange)
                    state = STATE_3;
                break;
                
            case STATE_3: 
                PORTB = DDR_ROUGE;
                if(isStateChange)
                    state = STATE_4;
                break;

            case STATE_4: 
                PORTB = DDR_OFF;
                if(isStateChange)
                    state = STATE_5;
                break;

            case STATE_5: 
                PORTB = DDR_VERT;
                if(isStateChange)
                    state = INIT;
                break;
            
    }

    isStateChange = false;
}


ISR(INT0_vect){
    if(rebound())
        isStateChange = true;

    stateAction(0);

    EIFR |= (1 << INTF0);
}


void initialisation () {

    cli ();

    DDRB = DDR_SORTIE;

    EIMSK |= (1 << INT0) ;

    EICRA |= (1 << ISC00) ;

    sei ();

}

int main(){

    initialisation();

    
    int     amberColorFrameCounter = 0;
    

    for(;;++amberColorFrameCounter){

/*
        if(hasInputChanged(previousState, isVariation)){
            isStateChange = true;
        }
*/
        stateAction(amberColorFrameCounter);

    }

}