 /* Nom: tp2 pb2
 * Copyright (C) 2019 Abid.R Sukhov.S
 * License http://www.gnu.org/copyleft/gpl.html GNU/GPL
 * Description: Ceci est un exemple simple de programme qui change la 
 couleur de la DEL libre en fonction de la facon dont on actionne le bouton-poussoir 
 (appuyer/maintenir/relacher)
 * Version: 1.1
 */

#define F_CPU 8'000'000
#include <avr/io.h> 
#include <util/delay.h>
#include <avr/interrupt.h>

volatile uint8_t minuterieExpiree;

volatile uint8_t boutonPoussoir; 


ISR (TIMER1_COMPA_vect) {

minuterieExpiree = 1;

}


ISR (INT0_vect) {

boutonPoussoir = 1;

// anti-rebond
_delay_ms(30);

}

void partirMinuterie ( uint16_t duree ) {

minuterieExpiree = 0;

// mode CTC du timer 1 avec horloge divisée par 1024

// interruption après la durée spécifiée

TCNT1 = 0 ;

OCR1A = (duree* F_CPU)/1024;

TCCR1AN |=0  ;

TCCR1B |= (1 << WGM12) | (1<<CS10)| (1<< CS12);

TCCR1C = 0;

TIMSK1 |= (1<< OCIE1A) ;

}
int main()
{
DDRD = 0x00; // PORT D est en mode entree
DDRA = 0xff; // PORT A est en mode sortie
DDRB = 0xff; // PORT B est en mode sortie
DDRC = 0xff; // PORT C est en mode sortie

PORTC= 0;
delay_ms(10000);
PORTC=2;
partirMinuterie ( 1000);// DEMANDER AU CHARGER le temps 


do {

// attendre qu'une des deux variables soit modifiée

// par une ou l'autre des interruptions.

} while ( minuterieExpiree == 0 && boutonPoussoir == 0 );


if( minuterieExpiree == 0){
	PORTC=2;
	}
	else {
	PORTC=1;
	}
// Une interruption s'est produite. Arrêter toute

// forme d'interruption. Une seule réponse suffit.

cli ();}

// Verifier la réponse




