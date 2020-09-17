#ifndef TRANSMISSION_UART_H_
#define TRANSMISSION_UART_H_

#include <avr/io.h>
#include "memoire_24.h"

void initialisationUART ( void );

void transmissionUART ( uint8_t donnee );

void transmissionChaineUART(uint8_t* tableau, int taille);

unsigned char receptionUSART();

#endif  /* TRANSMISSION_UART_H_ */