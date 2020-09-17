h#define F_CPU 8000000

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "lib4041.h"


const uint8_t DDR_OUT = 0xff;
const uint8_t DDR_GREEN_1 = (1 << 0);     //DEL sur A1 et A2
const uint8_t DDR_RED_1 = (1 << 1);

const int RIGHT_TURN_POWER = 250;
const int LEFT_TURN_POWER = 200;

const uint8_t NORMAL_WHEEL_POWER = 150;
const uint8_t STOP_WHEEL = 0;

const uint8_t DBT = 0x01;
const uint8_t ATT = 0x02;
const uint8_t DAL = 0x44;
const uint8_t DET = 0x45;
const uint8_t SGO = 0x48;
const uint8_t SAR = 0x09;
const uint8_t MAR1 = 0x60;
const uint8_t MAR2 = 0x61;
const uint8_t MAV = 0x62;
const uint8_t MRE = 0x63;
const uint8_t TRD = 0x64;
const uint8_t TRG = 0x65;
const uint8_t DBC = 0xC0;
const uint8_t FBC = 0xC1;
const uint8_t FIN = 0xFF;

const double demiPeriodes[] = {
4.54545454545455,
4.29033778491762,
4.04953962791165,
3.82225643297141,
3.60772966356503,
3.405243356538,
3.21412173266696,
3.03372694129618,
2.86345693157929,
2.70274344318825,
2.5510501097937,
2.40787066899808,
2.27272727272727,
2.14516889245881,
2.02476981395501,
1.91112821648571,
1.80386483178186,
1.702621678269,
1.60706086633296,
1.51686347064763,
1.43172846578964,
1.35137172159413,
1.27552505489718,
1.20393533449933,
1.13636363636364,
1.07258444622917,
1.01238490697771,
0.955564108242853,
0.901932415891094,
0.8513108391345,
0.803530433166482,
0.758431735323929,
0.715864232894822,
0.675685860796972,
0.637762527448506,
0.601967667249592,
0.568181818181818,
};

int nbInstructions = 0;



void boucle(uint8_t nbBoucle, int* position, Memoire24CXXX &memoire);

void attendre(uint8_t operande){
    for (uint8_t i = 0; i < operande; i++){	
        _delay_ms(25);
    }
}

void allumerDel(){
    PORTA &= DDR_OFF_1;
    PORTA |= DDR_GREEN_1;
}

void eteindreDel(){
    PORTA &= DDR_OFF_1;
}
    
void avancerMoteurs(uint8_t puissance){
    
    setPhaseCorrectPWM (puissance, puissance);
    PORTB &= ~(1 << 2);
    PORTB &= ~(1 << 5);
    
}

void reculerMoteurs(uint8_t puissance){
    
    setPhaseCorrectPWM (puissance, puissance);
    PORTB |= (1 << 2);
    PORTB |= (1 << 5);
    
}

void arreterMoteurs(){
    setPhaseCorrectPWM (STOP_WHEEL, STOP_WHEEL);
}


void tournerDroite(){
    PORTB &= ~(1 << 2);
    PORTB &= ~(1 << 5);
    setPhaseCorrectPWM(0, RIGHT_TURN_POWER);
    _delay_ms(1200);
    arreterMoteurs();
}

void tournerGauche(){
    PORTB &= ~(1 << 2);
    PORTB &= ~(1 << 5);
    setPhaseCorrectPWM(LEFT_TURN_POWER, 0);
    _delay_ms(1200);
    arreterMoteurs();
}

void partirMinuterie ( double secondes ) {

    TCCR1A |= (1 << COM1A0);

    TCCR1B |= (1 << CS10) | (1 << WGM12);

    TCNT1 = 0;

    OCR1A = (secondes * F_CPU);

}

void jouerNote(uint8_t parametre){
    if(parametre >= 45 && parametre <= 81){
        partirMinuterie(0.001*demiPeriodes[parametre-45]);
    }
}

void arreterNote(){
    TCCR1B &= ~(1 << CS10) & ~(1 << CS11) & ~(1 << CS11);
}

void fin(){
    eteindreDel();
    arreterNote();
    arreterMoteurs();
}


uint8_t executerInstruction(int* instruction, Memoire24CXXX &memoire){

    uint8_t mnemonic = 0;
    memoire.lecture(*instruction, &mnemonic);

    uint8_t parametre = 0;
    memoire.lecture(*instruction+1, &parametre);


    switch (mnemonic)
    {
        case DBT:
            
            break;

        case ATT:
            attendre(parametre);
            break;

        case DAL:
            allumerDel();
            break;

        case DET:
            eteindreDel();
            break;

        case SGO:
            jouerNote(parametre);
            break;

        case SAR:
            arreterNote();
            break;

        case MAR1:
            arreterMoteurs();
            break;

        case MAR2:
            arreterMoteurs();
            break;

        case MAV:
            avancerMoteurs(parametre);
            break;

        case MRE:
            reculerMoteurs(parametre);
            break;

        case TRD:
            tournerDroite();
            break;

        case TRG:
            tournerGauche();
            break;

        case DBC:
            boucle(parametre, instruction, memoire);
            break;

        case FBC:   //la fonction "boucle" détecte sa propre fin, FBC n'a pas besoin de fonction
            
            break;

        case FIN:
            fin();
            break;
        
        default:
            break;
    }

    return mnemonic;
}

void boucle(uint8_t nbBoucle, int* position, Memoire24CXXX &memoire){
    bool finBoucle = false;
    int debut = *position + 2;
    

    for(uint8_t i = 0; i <= nbBoucle; i++)
    {
        finBoucle = false;

        for(*position = debut; !finBoucle ;)
        {
            finBoucle = executerInstruction(position, memoire) == FBC;
            if(!finBoucle){
                (*position)+=2;
            }
            
        } 

    }
    
}


uint16_t appendUint8(const uint8_t MSB, const uint8_t LSB){
    uint16_t appended = 0;

    appended |= MSB << 8;
    appended |= LSB;

    return appended;
}

void lectureNombreInstructions(){
    
    Memoire24CXXX memory = Memoire24CXXX();               
    memory.init();


    uint8_t MSBsize = 0;
    uint8_t LSBsize = 0;
    memory.lecture(0, &MSBsize);
    memory.lecture(1, &LSBsize);

    nbInstructions = appendUint8(MSBsize, LSBsize) + 2;

    
}


int main(){  
    DDRA = DDR_OUT;                                           
    DDRB = DDR_OUT;
    DDRD = DDR_OUT;

    Memoire24CXXX memoire = Memoire24CXXX();
    memoire.init();
    

    lectureNombreInstructions();
    
    int i = 0;
    int *position = &i;

    for(*position = 0; *position < nbInstructions; *position+=2)
    {
        executerInstruction(position, memoire);
    }
    
    
}
