#define F_CPU 8000000

#include <avr/io.h>
#include "transmission_uart.h"

const uint8_t DDR_OUT = 0xff;
const uint8_t DDR_GREEN = 01;
const uint8_t DDR_RED = 10;


const uint8_t DBT = 0x01;
const uint8_t FIN = 0xFF;

/*
void lectureInstructions(){
    
    Memoire24CXXX memory = Memoire24CXXX();
    memory.init();

    bool hasStarted = false;
    uint8_t address = 0;
    unsigned char caractere = receptionUSART();     //1er octet inutil
    unsigned char nbInstructions = receptionUSART() - 2;  //2e octet donne le nombre d'octets reçu

    if(nbInstructions >= 2){
        
        while(caractere != FIN && address < nbInstructions){
            if (hasStarted && caractere != DBT){
                caractere = receptionUSART();       //Instruction
                memory.ecriture(address, caractere);
                ++address;

                caractere = receptionUSART();       //Paramètre
                memory.ecriture(address, caractere);
                ++address;

            }

            if(caractere == DBT){
                nbInstructions -=2;      //Ignore DBT et son paramètre dans le compte
                hasStarted = true;
                caractere = receptionUSART();   //Saute aussi par-dessus le paramètre de DBT
            }
        }
    }
}
*/

void lectureInstructions(){
    
    Memoire24CXXX memory = Memoire24CXXX();
    memory.init();

    bool hasStarted = false;
    bool hasEnded = false;
    uint8_t addresse = 0;
    unsigned char caractere = ' ';     

    
    caractere = receptionUSART();       //insère les 2 octets de taille au début de la mémoire
    memory.ecriture(addresse, caractere);
    ++addresse;
    caractere = receptionUSART();
    caractere -= 2;                     //pour ne pas compter les octets de taille dans le nombre d'instructions
    memory.ecriture(addresse, caractere);
    ++addresse;

    while (!hasEnded){
        
        caractere = receptionUSART();

        if(caractere == DBT){
            hasStarted = true;
        }
        
        if(hasStarted){
            memory.ecriture(addresse, caractere);
            ++addresse;
        }

        if(caractere == FIN){
            hasEnded = true;
            memory.ecriture(addresse, caractere);   //ajoute le parametre de FIN à la memoire
        }
        
    }

}


int main(){
    DDRA = DDR_OUT;
    PORTA = DDR_RED;
    initialisationUART();

    lectureInstructions();

    PORTA = DDR_GREEN;

    

}