#define F_CPU 8000000

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "lib4041.h"

const uint8_t DDR_OUT = 0xFF;
const uint8_t DDR_IN = 0x00;
const uint8_t DDR_RED = 0x01;
const uint8_t DDR_GREEN = 0x02;
const uint8_t DDR_OFF = 0x00;

const int RIGHT_NORMAL_POWER = 90;
const int LEFT_NORMAL_POWER = 90;

const int RIGHT_FAST_POWER = 150;
const int LEFT_FAST_POWER = 150;

//const uint8_t FAST_WHEEL_POWER = 110;
//const uint8_t NORMAL_WHEEL_POWER = 75;
const uint8_t STOP_WHEEL = 0;
const uint8_t SLOW_TURN_POWER = 50;


const uint8_t DELAY_REBOUND = 30;        //ms

const int CYCLE_PRESCALER = 1024;                    
const int CYCLE_PER_2_SECOND = 2*F_CPU/CYCLE_PRESCALER;     
const uint8_t START_TIMER_PRESCALE_1024 = (1 << CS12) | (1 << CS10);
const uint8_t STOP_TIMER_1 = ~((1 << CS12) | (1 << CS11) | (1 << CS10));


const uint8_t nbCapteursLignes = 5;
bool capteursLignes[nbCapteursLignes] = {};
const uint8_t maxAnalogSignal = 255;
const float lineDetectSensibility = 0.3;
volatile uint8_t dels = 0;
const uint8_t clearDels = 0b11100000;


bool sectionEstFinie = false;


bool debutCircuit3 = false;
int tempsEntreBarres = 0;
int tempsAttendu = 700;

bool gaucheDabord = false;
bool droiteDabord = false;
bool circuit3Fin = false;

bool rectangleMode = false;
uint8_t NB_MAX_RECTANGLE = 6;
uint8_t compteurRectangle = 0;
uint8_t compteurVirage = 0;

volatile uint8_t buttonInput = 0;

uint8_t section = 0;
uint8_t sectionDepart = 0;
const uint8_t NB_SECTIONS = 4;
const uint8_t NB_POINTS = 9;
const uint8_t MAX_BUTTON_INPUT_ALLOWED = 9;
uint8_t buttonMaxInput = NB_SECTIONS;

volatile bool receivedButtonInput = false;
volatile uint8_t previousButtonInput = 0;
volatile uint8_t pointDestination = 1;



const uint16_t tempsXpoints [3]= {
    700,1000,1400
};
const uint16_t tempsYpoints [3]= {
    700,1000,1400
};

can detectLine;

const uint8_t INDEX_SON_AIGU = 35;
const uint8_t INDEX_SON_GRAVE = 0;
const double demiPeriodesPiezo[] = {
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





/***********************************************************************
v Communication à distance v
************************************************************************/

volatile bool low = false;
volatile uint8_t message = 0;
volatile bool messageValide = false;
volatile uint8_t indexMessage = 0;
volatile bool receivedRemoteInstruction = false;




bool echantillon[400];


void start_T2() {//600us = 11 unités en prescaler 1024

    cli();

    TCCR2A = 0;
    TCCR2B = 0;

    TCCR2A |= (1 << COM2A0);
    TCCR2A |= (1 << WGM21);
    TCCR2B |= (1 << CS20) | (1 << CS21) | (1 << CS22);

    OCR2A = 11;

    TCNT2 = 0;

    sei();
}

void resetMessage(){
    low = false;
    messageValide = false;
    indexMessage = 0;
}

ISR(TIMER2_COMPA_vect){
    resetMessage();
}


ISR(PCINT2_vect){
    
    low = !low;
    
}

void setInterrupt(){
    cli();

    PCICR |= (1 << PCIE2);
    PCMSK2 |= (1 << PCINT21);   //Interruption sur la PIN 21 (PORT C PIN 6)
    
    sei();
}



void reception() {

    bool receptionFinie = false;
    uint8_t temp;

    start_T2 ();
    setInterrupt();
    low = false;
    //while(!receivedRemoteInstruction && !receivedButtonInput){
        indexMessage = 0;
        messageValide = false;
         

        for(int i = 0; i < 400; i++)
        {
            echantillon[i] = low;
            _delay_us(600);
        }

        int sommetsConsecutifs = 0;
        int lowConsecutifs = 0;
        for(int i = 0; i < 400 && !receptionFinie; i++)
        {

            if(!echantillon[i] && sommetsConsecutifs == 4){
                indexMessage = 0;
                messageValide = true;
            }
            else if(!echantillon[i] && sommetsConsecutifs == 2 && messageValide){
                message |= (1 << indexMessage);
                ++indexMessage;
            }
            else if(!echantillon[i] && sommetsConsecutifs == 1 && messageValide){
                message &= ~(1 << indexMessage);
                ++indexMessage;
            }
            
            if(indexMessage == 7){
                receptionFinie = true;
            } 


            if(echantillon[i]){
                sommetsConsecutifs++;
                lowConsecutifs = 0;
            }else
            {
                lowConsecutifs++;
                sommetsConsecutifs = 0;
            }
            
        }
        
    //}

    
    
    if(message <= buttonMaxInput && receptionFinie){
        receivedRemoteInstruction = true;
    }
    
}






/***********************************************************************
v Bouton d'interruption v
************************************************************************/


bool rebound(){
    previousButtonInput = PIND;

    _delay_ms(DELAY_REBOUND); 

    return PIND == previousButtonInput;
}

void resetTimer1(){
    TCNT1 = 0;
}

void stopTimer1(){
    TCCR1B &= STOP_TIMER_1;
}

void startTimer1(){
    TCCR1B |= START_TIMER_PRESCALE_1024;

}

ISR(INT0_vect){

    if(rebound() && !receivedButtonInput){
        stopTimer1();
        resetTimer1();
        startTimer1();
        buttonInput = (buttonInput == MAX_BUTTON_INPUT_ALLOWED) ? 1 : buttonInput + 1;
        
    }
        
    EIFR |= (1 << INTF0);
}

ISR(TIMER1_COMPA_vect){
    
    
    if(buttonInput < 1 || buttonInput > buttonMaxInput){
        buttonInput = 0;
    }
    else if(!receivedButtonInput){
        receivedButtonInput = true;
        stopTimer1();
        resetTimer1();
    }
    
    
    
}

void initialisationTimer1 () {

    buttonInput = 0;

    cli();

    EICRA |= (1 << ISC01) | (1 << ISC00);
    EIMSK |= (1 << INT0);

    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1C = 0;

    TCNT1 = 0;
    OCR1A = CYCLE_PER_2_SECOND;
    TCCR1A |= (1 << COM1A1) | (1 << WGM12);
    TCCR1B &= STOP_TIMER_1;
    TCCR1C = 0;
    TIMSK1 |= (1 << OCIE1A);

    sei();
}

void attendreInput(int maxInput){

    buttonInput = 0;
    message = 0;
    receivedButtonInput = false;
    receivedRemoteInstruction = false;
    buttonMaxInput = maxInput;
    initialisationTimer1();

    while(!receivedButtonInput && !receivedRemoteInstruction){
        reception();
    }

    

    stopTimer1();
    resetTimer1();

    if(receivedRemoteInstruction){
        buttonInput = message;
    }

    receivedRemoteInstruction = true;
    receivedButtonInput = true;
    


}





void lireCapteurs(){

    for(uint8_t i = 0; i < nbCapteursLignes; i++)
    {
        if(maxAnalogSignal*lineDetectSensibility > brightnessDetection(detectLine, i)){
            capteursLignes[i] = true;
            dels |= (1 << i);
        }
        else
        {
            capteursLignes[i] = false;
        }
            
    }

}



/***********************************************************************
v Piezo v
************************************************************************/

void partirMinuterie ( double secondes ) {

    

    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1C = 0;


    TCCR1A |= (1 << COM1A0);

    TCCR1B |= (1 << CS10) | (1 << WGM12);

    TCNT1 = 0;

    OCR1A = (secondes * F_CPU);

}

void commencerNote(uint8_t indexNote){
    if(indexNote >= 0 && indexNote <= 36){
        partirMinuterie(0.001*demiPeriodesPiezo[indexNote]);
    }
}

void arreterNote(){
    TCCR1B &= ~(1 << CS10) & ~(1 << CS11) & ~(1 << CS12);
}

void jouerNote(uint8_t indexNote, int duree){
    commencerNote(indexNote);
    variableMiliSecDelay(duree);    //Fonction en librairie. Plus précise si donné un multiple de 10 en paramètre. 
    arreterNote();
}







/***********************************************************************
v Déplacements v
************************************************************************/

void slowForward(){
    setPhaseCorrectPWM_T0(LEFT_NORMAL_POWER, RIGHT_NORMAL_POWER);
}
void slowForwardLeft(){
    setPhaseCorrectPWM_T0(SLOW_TURN_POWER, RIGHT_NORMAL_POWER);
}
void slowForwardRight(){
    setPhaseCorrectPWM_T0(LEFT_NORMAL_POWER, SLOW_TURN_POWER);
}

void forwardLeft(){
    setPhaseCorrectPWM_T0(LEFT_NORMAL_POWER, LEFT_FAST_POWER);
}
void left(){
    setPhaseCorrectPWM_T0(STOP_WHEEL,RIGHT_FAST_POWER);
}
void forwardRight(){
    setPhaseCorrectPWM_T0(LEFT_FAST_POWER, RIGHT_NORMAL_POWER);
}
void right(){
    setPhaseCorrectPWM_T0(LEFT_FAST_POWER, STOP_WHEEL);
}
void forward(){
    setPhaseCorrectPWM_T0(LEFT_FAST_POWER, RIGHT_FAST_POWER);
}

void stop(){
    setPhaseCorrectPWM_T0(STOP_WHEEL, STOP_WHEEL);
}

void selectActionNormal(){

    if(capteursLignes[2] && capteursLignes[1] && capteursLignes[0] && compteurRectangle == 0){       //À tester sur le circuit
        forward();
        _delay_ms(100);
        
        sectionEstFinie = true;
        
        
    }
    else if(capteursLignes[2] && capteursLignes[3] && capteursLignes[4]){

        right();
        //_delay_ms(50);
        
    }
    else if(capteursLignes[1]){
        forwardLeft();
        
    }
    else if(capteursLignes[3]){
        forwardRight();

    }
    else if(capteursLignes[4]){
        right();

    }
    else if(capteursLignes[0]){
        left();

    }
    else if(capteursLignes[2] && capteursLignes[1]){
        forwardLeft();
        //_delay_ms(100);
    }
    else if(capteursLignes[2] && capteursLignes[3]){
        forwardRight();
        //_delay_ms(100);
    }
    else if(capteursLignes[2]){
        forward();
    }
    else{
        //stop();
    }
        
    
}

/**
 * Sélectionne l'action des roues à effectuer en fonction des lectures des capteurs de ligne.
 * Inclue des actions supplémentaire si un rectangle est rencontré.
 * 
 */
void selectActionRectangle(){

    

    if(!rectangleMode){
        selectActionNormal();
    }
    else if(capteursLignes[4] && capteursLignes[0]){
        forward();
        
    }
    else if(capteursLignes[4] || capteursLignes[3]){
        forwardLeft();
        
    }
    else if(capteursLignes[1] || capteursLignes[0]){
        forwardRight();
        
    }
    else{
        forward();
        
    }

    
    if(/*capteursLignes[0] &&*/ capteursLignes[1] && capteursLignes[2] && capteursLignes[3] && capteursLignes[4] && !sectionEstFinie){
        if(compteurRectangle > 0){
            --compteurRectangle;
            rectangleMode = !rectangleMode;
            forward();

            jouerNote(INDEX_SON_AIGU, 50);
            _delay_ms(20);
            jouerNote(INDEX_SON_AIGU, 50);

            _delay_ms(300);
        }
        //else{
        //    sectionEstFinie = true;
        //}
    }
    
    if(sectionEstFinie){
        stop();
        _delay_ms(1000);
    }
    
}


void selectActionVirages(bool& dernierVirageAGauche){


    if(/*capteursLignes[0] &&*/ capteursLignes[1] && capteursLignes[2] && capteursLignes[3] && capteursLignes[4]){
        sectionEstFinie = true;
    }

    if(compteurVirage == 0){
        dernierVirageAGauche = false;
    }

    if(!sectionEstFinie){
        if(capteursLignes[4]){
            right();
            _delay_ms(100);
        }
        else if(capteursLignes[1] && dernierVirageAGauche){
            slowForwardLeft();
            _delay_ms(100);
        }
        else if(capteursLignes[3] && dernierVirageAGauche){
            slowForwardRight();
            _delay_ms(100);
        }
        //else if(capteursLignes[2] && dernierVirageAGauche){
        //    slowForward();
        //}
        else{
            selectActionNormal();
        }


        if(capteursLignes[1]){
            dernierVirageAGauche = true;
        }
        else{
            dernierVirageAGauche = false;
        }
    }

    
    

    
}



void selectActionCircuit3(){

    
    if(capteursLignes[2]){
        forward();
        PORTD = DDR_RED;
    }
    else if(capteursLignes[1]){
        forwardLeft();
        //forwardLeft();

        PORTD = DDR_RED;
    }
    else if(capteursLignes[3]){
        forwardRight();

        PORTD = DDR_RED;
    }
    
        
    
}

void IncrementerMinuterie ( ) {
    _delay_ms(10);
    tempsEntreBarres += 10;
}

void setDels(){
    PORTC &= clearDels;
    PORTC |= dels;

}



bool circuit3(){
    dels = 0;

    selectActionCircuit3();

    

    if(debutCircuit3 && (gaucheDabord != droiteDabord) && !circuit3Fin){
        IncrementerMinuterie();
        
    }

    if(debutCircuit3 && capteursLignes[0] && capteursLignes[1]){
        if(droiteDabord){
            circuit3Fin = true;
        }else{
            gaucheDabord = true;
        }
        
        
    }
    else if(debutCircuit3 && capteursLignes[3] && capteursLignes[4]){
        if(gaucheDabord){
            circuit3Fin = true;
        }else{
            droiteDabord = true;
        }

    }


    if(circuit3Fin){

        if(gaucheDabord){
            if(tempsEntreBarres > tempsAttendu){
                dels |= (1 << 0); //D1
            }else{
                dels |= (1 << 1); //D2
            }
                
            
        }
        if(droiteDabord){
            if(tempsEntreBarres > tempsAttendu){
                dels |= (1 << 2); //D3
            }else{
                dels |= (1 << 3); //D4
            }

            
        }
        stop();
        sectionEstFinie = true;
        return true;
    }   

    if(capteursLignes[0] && capteursLignes[1] && capteursLignes[2] && capteursLignes[3] && capteursLignes[4]){
        debutCircuit3 = true;
        _delay_ms(300);
    }

    return false;
}


    

void parcoursRectangle(){

    sectionEstFinie = false;
    rectangleMode = false;
    compteurRectangle = NB_MAX_RECTANGLE;
    compteurVirage = 0;
    

    while(!sectionEstFinie){
        dels = 0;
        lireCapteurs();
        setDels();
        
        selectActionRectangle();



        //_delay_ms(75);
    }

    stop();
    
    dels = 0;
    setDels();

}


void parcoursMilieu(){
    compteurRectangle = 0;

    sectionEstFinie = false;

    while(!sectionEstFinie){
        dels = 0;
        lireCapteurs();
        setDels();
        
        selectActionNormal();
    }

    stop();


    attendreInput(MAX_BUTTON_INPUT_ALLOWED);

    sectionEstFinie = false;

    while(!sectionEstFinie){
        lireCapteurs();
        setDels();
        dels = 0;
        
        circuit3();

        
    }

    stop();
    setDels();

    attendreInput(MAX_BUTTON_INPUT_ALLOWED);

    sectionEstFinie = false;
    

    while(!sectionEstFinie){
        dels = 0;
        lireCapteurs();
        setDels();
        selectActionNormal();
        /*
        if(capteursLignes[0] && capteursLignes[1]){
            sectionEstFinie = true;
        }
        */
    }
    
}

void parcoursVirages(){
    sectionEstFinie = false;
    compteurVirage = 0;
    compteurRectangle = 0;

    bool dernierVirageAGauche = false;
    
    
    while(!sectionEstFinie){

        dels = 0;
        lireCapteurs();
        setDels();
        
        
        selectActionVirages(dernierVirageAGauche);
                
    }
    

    dels = 0;
    setDels();
    
}




/*
void delay_ms(int ms) {
  for(int i = 0; i < ms; i++) _delay_ms(1);	
}
*/

/*
void virage90(bool direction){
     commencerNote(40);
    _delay_ms(3000);
    arreterNote();

    if(direction){
        left(); 
    } else{
        right();
    }
    _delay_ms(900);
    stop();

    commencerNote(40);
    _delay_ms(3000);
    arreterNote();
}
*/

void virage90Droite(){
    
    right();
    _delay_ms(1600);
    stop();
    _delay_ms(900);

}


void virage90Gauche(){
    
    left();
    _delay_ms(1300);
    stop();
    _delay_ms(900);

}

void circuit9PointsPreparation(){
    sectionEstFinie = false;

    while(!sectionEstFinie){
        dels = 0;
        lireCapteurs();
        setDels();

        selectActionNormal();
    }

    forward();
    _delay_ms(200);
    stop();


}

void circuit9PointsPlacement(uint8_t point){

    left();
    _delay_ms(1500);
    stop();
    _delay_ms(900);

    right();
    _delay_ms(500);
    forward();
    switch(point%3){
    case 0:
        _delay_ms(tempsXpoints[2]);
        
        break;
    case 1:
        _delay_ms(tempsXpoints[1]);
        
        break;
    case 2:
        _delay_ms(tempsXpoints[0]);
        
        break;
    }
    stop();
    _delay_ms(1000);

    virage90Droite();

    
    forward();
    switch(point/3){
    case 0:
        _delay_ms(tempsYpoints[2]);
        break;

    case 1:
        _delay_ms(tempsYpoints[1]);
        break;

    case 2:
        _delay_ms(tempsYpoints[0]);

        break;
    }
    stop();
    _delay_ms(1000);

    jouerNote(INDEX_SON_GRAVE, 3000);
}

bool auMoinsUnDetecteur(){
    return (capteursLignes[0] || capteursLignes[1] || capteursLignes[2] || capteursLignes[3] || capteursLignes[4]);
}


void actualiserLecture(){
    dels = 0;
    lireCapteurs();
    setDels();
}


void circuit9PointsRetour(uint8_t point){
    virage90Droite();
    jouerNote(INDEX_SON_GRAVE, 3000);

    forward();

    switch(point%3){
    case 0:
        _delay_ms(tempsXpoints[2]);
        
        break;
    case 1:
        _delay_ms(tempsXpoints[1]);
        
        break;
    case 2:
        _delay_ms(tempsXpoints[0]);
        
        break;
    }

    stop();
    _delay_ms(1000);

    virage90Gauche();
    _delay_ms(1000);

    actualiserLecture();


    while(!auMoinsUnDetecteur()){
        
        actualiserLecture();

        if(!auMoinsUnDetecteur()){
            
            forwardRight();
            _delay_ms(600);

            actualiserLecture();

            if(!auMoinsUnDetecteur()){
                forwardLeft();
                _delay_ms(600);
            }


        }


        
    }

}

void parcoursPoints(){
    //compteurVirage = 0;
    sectionEstFinie = false;


    circuit9PointsPreparation();

    attendreInput(NB_POINTS);

    circuit9PointsPlacement(buttonInput - 1);
    circuit9PointsRetour(buttonInput - 1);

    dels = 0;
    lireCapteurs();
    setDels();

    sectionEstFinie = false;
    while(!sectionEstFinie){
        if(capteursLignes[0] && capteursLignes[1] && capteursLignes[2]){
            sectionEstFinie = true;
            forward();
            _delay_ms(500);
            stop();
        }else{
            lireCapteurs();
            setDels();
            dels = 0;
            selectActionNormal();
        }
        
    }

    if(sectionEstFinie){
        stop();
        _delay_ms(1000);
    }
    
}



void selectionParcours(){

    left();
    _delay_ms(1000);

    switch(section){
        case 1:
            parcoursPoints();
        break;

        case 2:
            parcoursVirages();
        break;

        case 3:
            parcoursMilieu();
        break;

        case 4:
            parcoursRectangle();
        break;
    }


}









int main(){
    
    //DDRD = DDR_OUT;
    //DDRD &= ~(1 << 2);

    DDRA = DDR_IN;
    DDRB = DDR_OUT;
    DDRC = DDR_OUT;
    DDRC &= ~(1<<5);
    DDRD = DDR_IN;
    DDRD |= 0b00110000; // Mets en sortie les pins du PWM pour le piezo

    

    attendreInput(NB_SECTIONS);
    

    section = buttonInput;
    sectionDepart = section;

    dels = 1 << (section-1);
    setDels();
    
    _delay_ms(3000);
    dels = 0;
    setDels();
    
    do{
        selectionParcours();
        section = (section < NB_SECTIONS)?(section+1):1;
    }while(section != sectionDepart);

    stop();
    
    jouerNote(INDEX_SON_AIGU, 2000);
    
}