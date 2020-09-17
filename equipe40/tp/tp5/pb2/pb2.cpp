#include <string.h>


#include <util/delay.h>
#include <avr/io.h>
#include "memoire_24.h"

#define F_CPU 8000000

const uint8_t DDR_OUT = 0xff;
const uint8_t DDR_OFF = 00;  
  
void initialisationUART ( void ) {

    // 2400 bauds. Nous vous donnons la valeur des deux

    // premier registres pour vous éviter des complications

    
    UBRR0H = 0;

    UBRR0L = 0xCF;
    
    //nt baud = 2400;

    //UBRR0H = (uint8_t)(baud>>8);

    //UBRR0L = (uint8_t)baud;

    // permettre la reception et la transmission par le UART0

    UCSR0A = 0;

    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);

    // Format des trames: 8 bits, 1 stop bits, none parity

    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
    //UCSR0C = (1<<UMSEL01)|(1<<UMSEL00)|(0<<CPHA0)|(0<<CPOL0);

}

    // De l'USART vers le PC

void transmissionUART ( uint8_t donnee ) {

    while ( !( UCSR0A & (1 << UDRE0)) ) {
        
    }
    UDR0 = donnee;
}

int main(){
    initialisationUART();

    char mots[21] = "Le robot en INF1900\n";

    uint8_t i, j;

    for ( i = 0; i < 100; i++ ) {

        for ( j=0; j < 20; j++ ) {

            transmissionUART ( mots[j] );

        }

    }
}