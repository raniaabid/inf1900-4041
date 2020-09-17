#ifndef LUMIN_DETECT_H_
#define LUMIN_DETECT_H_

#include "can.h"
#include <avr/io.h>

uint8_t brightnessDetection(can & converter, uint8_t pos);

#endif  /* LUMIN_DETECT_H_ */