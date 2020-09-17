#include "lumin_detect.h"

uint8_t brightnessDetection(can& converter, uint8_t voltage){
    return (uint8_t)(converter.lecture(voltage) >> 2);
}



