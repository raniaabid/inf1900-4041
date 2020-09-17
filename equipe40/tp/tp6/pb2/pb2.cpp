/*
Programme qui change la couleur d'une DEL en fonction de la luminosité à laquelle une photorésistance est exposée.
À luminosité "normale", la DEL est de couleur ambrée. Plus la luminosité augmente, plus la DEL tourne au rouge.
Plus la luminosité diminue, plus la DEL tourne au vert.
file pb2.cpp
author Laurence Robert (1960716) & Hugo Palisson (1957983)
date 2019-02-18
Créé le 2019-02-14
*/


#define F_CPU 8000000

#include "can.h"
#include <math.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

const uint8_t DDR_IN = 0x00;
const uint8_t DDR_OUT = 0xff;
const uint8_t DDR_GREEN = 01;
const uint8_t DDR_RED = 10;
const uint8_t DDR_OFF = 00;

const uint8_t VOLTAGE = 5;

const uint8_t GREEN_CORRECTION = 200;   //Augmente la quantité de vert dans la DEL
const float RED_SCALING = 5;            //Augmente l'influence de la luminosité sur la présence du rouge dans la DEL

uint16_t newBrightness = 0;
uint8_t brightness = 0;
uint8_t BRIGHTNESS_MAX = 0xFF;

void brightnessAdjustment(){
    
    if(brightness < GREEN_CORRECTION){
        brightness = 0;
    }
    else{
        brightness -= GREEN_CORRECTION;
    }

    if(brightness*RED_SCALING > BRIGHTNESS_MAX){
        brightness = BRIGHTNESS_MAX;
    }
    else{
        brightness *= RED_SCALING;
    }
}


void brightnessDetection(can& converter){
    newBrightness = converter.lecture(VOLTAGE);  
    brightness = (uint8_t)(newBrightness >> 2);
}

void delColorCycle(){
    for(uint8_t i = 0; i < BRIGHTNESS_MAX; ++i){
        if(brightness > i){
            PORTB = DDR_RED;
        }
        else{
            PORTB = DDR_GREEN;
        }
    }
}

int main(){
    DDRB = DDR_OUT;
    can converter;

    while(true){

        brightnessDetection(converter);

        brightnessAdjustment();

        delColorCycle();
        
    }

}