/* Nom: tp6 pb2
 * Copyright (C) 2019 Abid.R Sukhov.S
 * License http://www.gnu.org/copyleft/gpl.html GNU/GPL
 * Description: Programme qui permet de bronzer la DEL selon 
 * du niveau de lumiere disponible.
 * ********************************************************************/
#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> 
#include "can.h"

const uint8_t ROUGE = 0x01;
const uint8_t VERTE = 0x02;
// maximum de 255

//limite fixee a 230
const uint8_t LIMITE_ROUGE = 230; 
//nous allons fixer la limite du vert a 175 (plus petite que la limite du rouge)
const uint8_t LIMITE_VERT = 175;

int main(){

    DDRA = 0x00;
    DDRB = 0xFF; 
    uint8_t valeur;
   
    can converter;

    for(;;){
        
        valeur = converter.lecture(1)>>2;
        
        //decallage de bits
      
        if(valeur< LIMITE_VERT)
            PORTB = 0x02;

        else if(valeur < LIMITE_ROUGE && valeur > LIMITE_VERT)
        {
            PORTB = 0x01;
            _delay_ms(10);
            PORTB = 0x02;
            _delay_ms(10);
        }

        else if (valeur> LIMITE_ROUGE)
        PORTB = 0x01;
    }
    return 0;
}
