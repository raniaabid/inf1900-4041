 /* Nom: tp2 pb2
 * Copyright (C) 2019 Abid.R Sukhov.S
 * License http://www.gnu.org/copyleft/gpl.html GNU/GPL
 * Description: Ceci est un exemple simple de programme qui change la 
 couleur de la DEL libre en fonction de la facon dont on actionne le bouton-poussoir 
 (appuyer/maintenir/relacher)
 * Version: 1.1
 
 etat present    A    etat prochain     sortie (z)
 init            0       init             rouge 
 init            1        EA              Ambre
 EA              1        EA              Ambre
 EA              0        EB              Vert
 EB              0        EB              Vert
 EB              1        EC              rouge
 EC              1        EC              rouge 
 EC              0        ED              eteind
 ED              0        ED              eteind
 ED              1        EE              vert
 EE              1        EE              vert
 EE              0        init            rouge 
 
 */

#define F_CPU 8'000'000
#include <avr/io.h> 
#include <util/delay.h>



 int main()
{
  DDRA = 0xff; // PORT A est en mode sortie
  DDRB = 0xff; // PORT B est en mode sortie
  DDRC = 0xff; // PORT C est en mode sortie
  DDRD = 0x00; // PORT D est en mode sortie
  // le compteur est initialise a 0.
                            // c'est un compteur de 32 bits
 enum STATE {INIT,EA, EB, EC, ED, EE};
 STATE state = INIT;  

  for(;;)  // boucle sans fin
  
  {  
	  
	  switch(state){
		  case INIT:
		  PORTC = 2;
		  if ( PIND & 0x04 ){ 
	      _delay_ms(10);
          if ( PIND & 0x04 ){
				  state = EA;
				  }}
	      
		  break;
		  case EA: 
		   while(PIND & 0x04){
				  PORTC = 1;// >> 16; 
				 _delay_ms(3);
				  PORTC =2 ; // >> 16;
				 _delay_ms(3);
				  } 
				  state = EB;
	      
		  break;
		  
		  case EB:
		  PORTC = 1;
		  if ( PIND & 0x04 ){ 
	      _delay_ms(10);
          if ( PIND & 0x04 ){
				  state = EC;
				  }}
				  break;
				  
		  case EC:
		  while(PIND & 0x04){
		  PORTC = 2;
		  }
		  state = ED;
		  break;
		  	 
		  case ED:
		  PORTC = 0;
		  if ( PIND & 0x04 ){ 
	      _delay_ms(10);
          if ( PIND & 0x04 ){
				  state = EE;
	      break;
	      
	      case EE:
	      while(PIND & 0x04){
	      PORTC=1;
	      }
	      state = INIT;
	      break;
	  }}
		  }
                           
}}
