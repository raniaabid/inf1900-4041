/*
 * Fichier: pb1.cpp
 * Auteurs: Sukhov Semen(1962066) et Abid Rania(1963179)
 * Date: 25 janvier 2019
 * Description: ce programme attenue la couleur rouge de la led en 
   abaissant lintensite de la lumiere */

#define F_CPU 8000000 // 8 MHz
#include <avr/io.h>
#include <util/delay.h>


void attente(int temps){
  for(int i=0; i<temps;i++)
    _delay_us(1);
}
int main()
{

  DDRA = 0xff; // PORT A est en mode sortie
  
  
  uint8_t rouge = 0x02;
  uint8_t eteint = 0x00;

  for (;;) // boucle sans fin
  {
     int a=1200;//delai pour le rouge
  int const b=1200;//delai pour eteint
      while (a>0){
       PORTA=rouge;
       attente(a);
       PORTA=eteint;
       attente(b-a);
       a--;       
    }
  }
    
  return 0;
}
