#define F_CPU 8000000

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "lib4041.h"

const uint8_t DDR_OUT = 0xFF;
const uint8_t DDR_RED = 0x01;
const uint8_t DDR_OFF = 0x00;


const int RIGHT_NORMAL_POWER = 50;
const int LEFT_NORMAL_POWER = 70;

const uint8_t FAST_WHEEL_POWER = 100;
const uint8_t NORMAL_WHEEL_POWER = 75;
const uint8_t STOP_WHEEL = 0;

const uint8_t nbCapteursLignes = 5;
bool capteursLignes[nbCapteursLignes] = {};
const uint8_t maxAnalogSignal = 255;
const float lineDetectSensibility = 0.3;
uint8_t dels = 0;
const uint8_t clearDels = 0b11100000;


void forwardLeft(){
    setPhaseCorrectPWM_T0(LEFT_NORMAL_POWER, FAST_WHEEL_POWER);
}
void left(){
    setPhaseCorrectPWM_T0(FAST_WHEEL_POWER,0);
}

void forwardRight(){
    setPhaseCorrectPWM_T0(FAST_WHEEL_POWER, RIGHT_NORMAL_POWER);
}
void right(){
    setPhaseCorrectPWM_T0(0, RIGHT_NORMAL_POWER);
}
void forward(){
    setPhaseCorrectPWM_T0(LEFT_NORMAL_POWER, RIGHT_NORMAL_POWER);
}

void stop(){
    setPhaseCorrectPWM_T0(STOP_WHEEL, STOP_WHEEL);
}


void selectAction(){
    if(capteursLignes[1] && capteursLignes[3]){
        forward();
        PORTD = DDR_RED;
    }
    else if(capteursLignes[2] && capteursLignes[1]){
        left();
        PORTD = DDR_RED;
    }
    else if(capteursLignes[2] && capteursLignes[3]){
        right();
        PORTD = DDR_RED;
    }
    else if(capteursLignes[1]){
        forwardLeft();

        PORTD = DDR_RED;
    }
    else if(capteursLignes[3]){
        forwardRight();
        PORTD = DDR_RED;
    }
    else{
        PORTD = DDR_OFF;
        //stop();
    }
        
    //ici code pour la section du circuit du milieu//

//del3 et4  on ajoute le temps ensuite pour les diférencier
    if(capteursLignes[0] && capteursLignes[4]){
        _delay_ms(100);
        if(capteursLignes[0]){
          _delay_ms(100);
            if(capteursLignes[4])
            dels |= (1 << i); //jsp quel commande pour del 1,2,3,4
                }
              }
//del1 et 2 on ajoute le temps pour les differencier 
if(capteursLignes[0] && capteursLignes[4]){
        _delay_ms(100);
        if(capteursLignes[4]){
          _delay_ms(100);
            if(capteursLignes[0])
            dels |= (1 << i); //jsp quel commande pour del 1,2,3,4
                }
              }
    // fin code du milieu

void setDels(){
    PORTC &= clearDels;
    PORTC |= dels;

}


int main(){

    DDRD = DDR_OUT;
    DDRC = DDR_OUT;
    DDRB = DDR_OUT;

    can detectLine;


    while(true){

        dels = 0;
        
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
            
            selectAction();

            

        }
        setDels();

        _delay_ms(100);
    }




}
