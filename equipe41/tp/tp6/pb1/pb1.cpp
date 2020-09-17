/* Nom: tp6 pb1
 * Copyright (C) 2019 Abid.R Sukhov.S
 * License http://www.gnu.org/copyleft/gpl.html GNU/GPL
 * Description: 
***********************************************************************
 */
#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> 

const uint8_t SORTIE = 0xFF;
const uint8_t ENTREE = 0x00;
const uint8_t ETEINTE = 0x00;
const uint8_t ROUGE = 0x01;
const uint8_t VERTE = 0x02;
volatile uint8_t state = 0;

ISR (INT0_vect) {
	_delay_ms (30);
	if(state == 3)
		state = 0;
	else state++;
	EIFR |= (1 << INTF0); 
}

void initialisation (void) {
	cli (); 
	DDRD = ENTREE; 
	DDRA = SORTIE;
	EIMSK |= (1 << INT0); 
	EICRA |= (1 << ISC00); 
	sei ();
}


int main() {
	int compteur = 0;
	initialisation();

	while (true) {
		switch (state) {	
			//DEL eteinte
			case 0: 
				PORTA = ETEINTE;			
				break;

			case 1: 
				if(compteur < 120){ 
					compteur += 10;
					_delay_ms(1000);
				}
				else state = 2;
				break;

			case 2: 
				//clignotenment vert
				for (int i = 0; i < 2; i++){
					PORTA = VERTE;
					_delay_ms(150);
					PORTA= ETEINTE;
					_delay_ms(100);		
				}	
				state = 3;				
				break;

			case 3: 
				_delay_ms(2000);							
				// clignotement rouge
				
				for (int i = 0; i < (compteur/10); i++){
					for (int j = 0; j < 5; j++) {
						PORTA = ROUGE;
						_delay_ms(125);
						PORTA = ETEINTE;
						_delay_ms(75);
					}
				}
				PORTA = VERTE;
				_delay_ms(1000);
				PORTA = ETEINTE;
				state = 0;
				break;
		}
	}
	return 0;
}
