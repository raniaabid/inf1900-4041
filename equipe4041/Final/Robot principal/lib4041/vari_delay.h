#ifndef VARI_DELAY_H_
#define VARI_DELAY_H_

#ifndef F_CPU
#define F_CPU 8000000

#endif /* F_CPU */

#include <util/delay.h>
#include <avr/io.h> 



void variableMicroSecDelay(int delay);

void variableMiliSecDelay(int delay);

#endif  /* VARI_DELAY_H_ */