 /*
 * Nom: Tp2 pb1
 * Copyright (C) 2019  abid R. sukhov.s
 * License http://www.gnu.org/copyleft/gpl.html GNU/GPL
 * Description: Ceci est un exemple simple de programme qui allume 
  la DEL de couleur rouge apres avoir actionne le bouton-poussoir 5 fois.
 * Version: 1.1
  
 etat present    A    etat prochain     sortie (z)
 E1              0        E1              eteind 
 E1              1        E2              eteind
 E2              1        E2              eteind
 E2              0        E3              eteind
 E3              0        E3              eteind
 E3              1        E4              eteind
 E4              1        E4              eteind
 E4              0        E5              eteind
 E5              0        E5              eteind
 E5              1        A               rouge
 A               0   E1(change tout seul) eteind 
 
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
  enum STATE{E1, E2, E3, E4, E5, A};
  STATE state = E1;
  
  for(;;)  // boucle sans fin
  {
	  switch(state){
		  case E1:
		  PORTC = 0;
		  if(PIND & 0x04){
			  _delay_ms(10);
			  if(PIND & 0x04){
				  while(PIND & 0x04){
					  } 
					  state = E2;
				  }
			  }
		  break;
		  
		  case E2:
		  if(PIND & 0x04){
			  _delay_ms(10);
			  if(PIND & 0x04){
				  while(PIND & 0x04){
					  } 
					  state = E3;
				  }
			  }
		  break;
		  
		  case E3:
		  if(PIND & 0x04){
			  _delay_ms(10);
			  if(PIND & 0x04){
				  while(PIND & 0x04){
					  } 
					  state = E4;
				  }
			  }
		  break;
		  
		  case E4:
		  if(PIND & 0x04){
			  _delay_ms(10);
			  if(PIND & 0x04){
				  while(PIND & 0x04){
					  } 
					  state = E5;
				  }
			  }
		  break;
		  
		  case E5:
		  if(PIND & 0x04){
			  _delay_ms(10);
			  if(PIND & 0x04){
				  while(PIND & 0x04){
					  } 
					  state = A;
				  }
			  }
		  break;
		  
		  case A:
		  PORTC = 2;
		  _delay_ms(1000);
		  state = E1;
		  break;
		}  
  }
  return 0; 
}
