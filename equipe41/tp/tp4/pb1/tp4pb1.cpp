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



 enum STATE {INIT,EA, EB, EC, ED, EE};
 volatile STATE state = INIT;  
 // placer le bon type de signal d'interruption

// à prendre en charge en argument

ISR ( INT0_vect  ){
  
// laisser un delai avant de confirmer la réponse du

// bouton-poussoir: environ 30 ms (anti-rebond)
 
	_delay_ms(30);
 switch(state){
		  case INIT:
		  state = EA;
		  break;
		  
		  case EA: 
		  state = EB;
		  break;
		  
		  case EB:
		  state = EC;
		  break;
				  
		  case EC:
		  state = ED;
		  break;
		  	 
		  case ED:
		  state = EE;
	      break;
	      
	      case EE:
	      state = INIT;
	      break;
	  }
		  


 // se souvenir ici si le bouton est pressé ou relâché



// changements d'états tels que ceux de la

// semaine précédente



// Voir la note plus bas pour comprendre cette instruction et son rôle

EIFR |= (1 << INTF0) ;

}
void initialisation ( void ) {

// cli est une routine qui bloque toutes les interruptions.


// Il serait bien mauvais d'être interrompu alors que

// le microcontroleur n'est pas prêt...

cli ();


// configurer et choisir les ports pour les entrées

// et les sorties. DDRx... Initialisez bien vos variables

DDRA = 0xff; // PORT A est en mode sortie
DDRB = 0xff; // PORT B est en mode sortie
DDRC = 0xff; // PORT C est en mode sortie
DDRD = 0x00; // PORT D est en mode entree



// cette procédure ajuste le registre EIMSK

// de l’ATmega324PA pour permettre les interruptions externes

EIMSK |= (1 << INT0) ;


// il faut sensibiliser les interruptions externes aux

// changements de niveau du bouton-poussoir

// en ajustant le registre EICRA

EICRA |= (1<<ISC00);


// sei permet de recevoir à nouveau des interruptions.

sei ();
}

 int main()
{
	initialisation();
	for (;;) {
		switch(state){
			case INIT:
		  PORTC = 2;
		  break;
		  
		  case EA: 
				  PORTC = 1;// >> 16; 
				 _delay_ms(3);
				  PORTC =2 ; // >> 16;
				 _delay_ms(3);
		  break;
		  
		  case EB:
		  PORTC = 1;
		  break;
				  
		  case EC:
		  PORTC = 2;
		  break;
		  	 
		  case ED:
		  PORTC = 0;
	      break;
	      
	      case EE:
	      PORTC=1;
	      break;
	}
}}



	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

