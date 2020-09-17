#include <string.h>

#include <util/delay.h>
#include <avr/io.h>
#include "memoire_24.h"

#define F_CPU 8000000

const uint8_t DDR_OUT = 0xff;
const uint8_t DDR_GREEN = 01;
const uint8_t DDR_RED = 10;
const uint8_t DDR_OFF = 00;

int main(){

    

    DDRB = DDR_OUT;
    bool isIdentical = true;

    Memoire24CXXX memoire = Memoire24CXXX();

    unsigned char message[] = "*P*O*L*Y*T*E*C*H*N*I*Q*U*E* *M*O*N*T*R*E*A*L*";
    //unsigned char* ptrMessage = message;

    unsigned char copieMessage[sizeof(message)];
    //unsigned char* ptrCopieMessage = copieMessage;

    // for(int i = 0; i < sizeof(message)-1; ++i){
    //     memoire.ecriture(1+i, message[i]);

    // }

    // _delay_ms(100);

    // for(int i = 0; i < sizeof(message)-1; ++i){
    //     memoire.lecture(1+i, &(copieMessage[i]));
        
    // }



    memoire.ecriture(0x01, message, sizeof(message));

    _delay_ms(100);

    memoire.lecture(0x01, copieMessage, sizeof(message));

    if(message == copieMessage){
        PORTB = DDR_GREEN;
    }

    for(int i = 0; i < sizeof(message)-1 && isIdentical; ++i){
        if(message[i] != copieMessage[i]){
            isIdentical = false;
        }
    }

    if(isIdentical)
        PORTB = DDR_GREEN;
    else
        PORTB = DDR_RED;

}