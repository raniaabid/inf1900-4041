/*
 * Fichier: pb1.cpp
 * Auteurs: Sukhov Semen(1962066) et Abid Rania(1963179)
 * Date: 25 janvier 2019
 * Description: */

#define F_CPU 8000000 // 8 MHz
#include <avr/io.h>
#include <util/delay.h>

void delay_ms(int ms) {
  for(int i = 0; i < ms; i++) _delay_ms(1);	
}

int main()
{

  DDRA = 0xff; // PORT A est en mode sortie
  
 
  float a; float b;  
	a = 0;
	b = 100-a;
for (;;) // boucle sans fin
  {
 
  for(int i=0; i<4; i++){
	  for(int j=0; j<=2000/16.6666; j++) {
		
		  
		  float ta = (a/100)*16.6666;
		  float tb = (b/100)*16.6666;
		  PORTC=1;
		  delay_ms(ta);
		  
		  PORTC=0;
		  delay_ms(tb);
		  
	  }
	  //a += 25;
	}
	}
  
    
  return 0;
}

