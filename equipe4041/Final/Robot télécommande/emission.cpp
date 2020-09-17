#define F_CPU 8000000

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "lib4041.h"

const uint8_t DDR_OUT = 0xFF;
const uint8_t DDR_IN = 0x00;
const uint8_t DDR_RED = 0x01;
const uint8_t DDR_GREEN = 0x02;
const uint8_t DDR_OFF = ~((1 << 0) | (1 << 1));

const uint8_t BUTTON_PRESSED = 0x04;
const uint8_t BUTTON_UNPRESSED = 0x00;

const int SECOND_DELAY = 1000;       //ms
const uint8_t DELAY_REBOUND = 30;        //ms

const int CYCLE_PRESCALER = 1024;                    
const int CYCLE_PER_2_SECOND = 2*F_CPU/CYCLE_PRESCALER;     
const uint8_t START_TIMER_PRESCALE_1024 = (1 << CS12) | (1 << CS10);
const uint8_t STOP_TIMER_1 = ~((1 << CS12) | (1 << CS11) | (1 << CS10));
const uint8_t STOP_TIMER_0 = ~((1 << CS00) | (1 << CS01) | (1 << CS02));

const uint16_t TRANSMISSION_START_TIME = 2400;
const uint16_t TRANSMISSION_PAUSE_TIME = 600;
const uint16_t TRANSMISSION_TIME_ZERO = 600;
const uint16_t TRANSMISSION_TIME_ONE = 1200;

uint8_t delInstruction = 0x01;
uint16_t message = 0;
const uint8_t MESSAGE_SIZE = 15;

volatile bool isTransmitting = false;
volatile uint8_t previousInput = BUTTON_UNPRESSED;
volatile uint8_t timesButtonPressed = 0;


volatile bool buttonPressed = false;

bool rebound(){
    previousInput = PIND;

    _delay_ms(DELAY_REBOUND); 

    return PIND == previousInput;
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
    PORTC |= DDR_RED;

    if(rebound() && !isTransmitting){
        stopTimer1();
        resetTimer1();
        startTimer1();
        timesButtonPressed = (timesButtonPressed + 1 == 10) ? 1 : timesButtonPressed + 1;
        //buttonPressed = true;
        
    }
        
    EIFR |= (1 << INTF0);
}

ISR(TIMER1_COMPA_vect){
    
    //PORTB |= DDR_RED;
    if(!isTransmitting){
        isTransmitting = true;
        stopTimer1();
        resetTimer1();
    }
    
}

void initialisationTimer1 () {

    cli();

    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1C = 0;

    EIMSK = 0;
    EICRA = 0;

    EIMSK |= (1 << INT0);
    EICRA |= (1 << ISC01) | (1 << ISC00);

    TCNT1 = 0;
    OCR1A = CYCLE_PER_2_SECOND;
    TCCR1A |= (1 << COM1A1)| (1 << WGM12);
    TCCR1C = 0;
    TIMSK1 |= (1 << OCIE1A);

    sei();
}

void initializeCTC_T0 () {

    TCCR0A = 0;
    TCCR0B = 0;


    //TCCR0A |= (1 << COM0A0);

    TCCR0B &= ~((1 << CS00) | (1 << CS01) | (1 << CS02));
    TCCR0A |= (1 << WGM01);
    TCCR0B |= (1 << CS00);

    //TCNT0 = 0;

    OCR0A = 104;

}

void startTransmit(){
    //OCR0A = 104;
    //TCCR0B |= (1 << CS00);
    TCCR0A |= (1 << COM0A0);
}

void stopTransmit(){
    TCCR0A &= ~(1 << COM0A0) & ~(1 << COM0A1);
    //OCR0A = 0;
    //TCCR0B &= ~((1 << CS00) | (1 << CS01) | (1 << CS02));
    //TCNT0 = 0;
}


void transmitInstructions(){
    message = timesButtonPressed;
    message |= delInstruction << 7;

    
    //message = delInstruction << 8;
    //message |= timesButtonPressed;
while(true){
    startTransmit();
    _delay_us(TRANSMISSION_START_TIME);
    stopTransmit();
    _delay_us(TRANSMISSION_PAUSE_TIME);



    for(uint8_t i = 0; i < MESSAGE_SIZE; ++i)
    {
        startTransmit();
        if(message & (1 << i)){
            _delay_us(TRANSMISSION_TIME_ONE);
        }
        else{
            _delay_us(TRANSMISSION_TIME_ZERO);
        }
        stopTransmit();
        _delay_us(TRANSMISSION_PAUSE_TIME);
        

    }

    _delay_ms(45);
    
}
/*
    startTransmit();
    _delay_us(TRANSMISSION_START_TIME*1);
    stopTransmit();
    _delay_us(TRANSMISSION_PAUSE_TIME*1);
*/
/*
    startTransmit();
    _delay_ms(TRANSMISSION_START_TIME*10);
    stopTransmit();
    _delay_ms(TRANSMISSION_PAUSE_TIME*10);
    */

/*
    if(nbUns%2 == 0){
        startTransmit();
        _delay_ms(TRANSMISSION_TIME_ONE*10);
        stopTransmit();
    }
*/
    
    


    timesButtonPressed = 0;
    isTransmitting = false;

    //stopTransmit();
}

int main(){

    DDRB = DDR_OUT;
    DDRA = DDR_OUT;
    DDRD = DDR_IN;
    DDRC = 0x3;
    //PORTB = DDR_OFF;

    initialisationTimer1();
    initializeCTC_T0();
    
    //startTransmit();
    //stopTransmit();
    


    while(true){

        if(isTransmitting){
            transmitInstructions();
            //PORTB = 1111;
            //_delay_ms(500);
            //PORTB &= 0;
            //stopTransmit();
            
        }
        
        
    }

    

}