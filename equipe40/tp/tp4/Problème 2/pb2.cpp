#define F_CPU 8000000

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

const uint8_t DDR_OUT = 0xff;
const uint8_t DDR_GREEN = 01;
const uint8_t DDR_RED = 10;
const uint8_t DDR_OFF = 00;


const uint8_t BUTTON_PRESSED = 0x04;
const uint8_t BUTTON_UNPRESSED = 0x00;

const int SECOND_DELAY = 1000;       //ms
const uint8_t DELAY_REBOUND = 30;        //ms

const int CYCLE_PRESCALER = 1024;                    
const int CYCLE_PER_SECOND = F_CPU/CYCLE_PRESCALER;     
const uint8_t START_TIMER_PRESCALE_1024 = (1 << CS12) | (1 << CS10);
const uint8_t STOP_TIMER = ~(1 << CS12) & ~(1 << CS11) & ~(1 << CS10);

volatile uint8_t previousInput = BUTTON_UNPRESSED;

volatile bool timeOut = false;
volatile bool buttonPressed = false;
bool buttonCanBePressed = false;



bool rebound(){
    previousInput = PIND;

    _delay_ms(DELAY_REBOUND); 

    return PIND == previousInput;
}

ISR(INT0_vect){

    if(rebound() && buttonCanBePressed){
        buttonPressed = true;
        
    }
        
    EIFR |= (1 << INTF0);
}

ISR(TIMER1_COMPA_vect){
    
    timeOut = true;
    
}

void startTimerOneSecond(){
    TCCR1B |= START_TIMER_PRESCALE_1024;

}

void initialisation () {

    cli();

    EIMSK |= (1 << INT0);
    EICRA |= (1 << ISC01) | (1 << ISC00);

    TCNT1 = 0;
    OCR1A = CYCLE_PER_SECOND;
    TCCR1A |= (1 << COM1A1) | (1 << WGM12);
    TCCR1C = 0;
    TIMSK1 |= (1 << OCIE1A);

    sei();
}

int main(){

    DDRB = DDR_OUT;
    PORTB = DDR_OFF;

    initialisation();

    _delay_ms(10*SECOND_DELAY);
    PORTB = DDR_RED;
    _delay_ms(SECOND_DELAY/10);
    PORTB = DDR_OFF;

    buttonCanBePressed = true;

    startTimerOneSecond();

    do{
    }while(!timeOut && !buttonPressed);

    cli();

    if(timeOut){
        PORTB = DDR_RED;
    }else{
        PORTB = DDR_GREEN;
    }

}