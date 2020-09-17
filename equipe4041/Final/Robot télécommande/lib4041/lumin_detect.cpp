#include "lumin_detect.h"

uint8_t brightnessDetection(can & converter, uint8_t pos){
    return (uint8_t)(converter.lecture(pos) >> 2);
}



