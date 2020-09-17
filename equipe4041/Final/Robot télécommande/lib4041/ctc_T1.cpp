#include "ctc_T1.h"

void partirCTC_T1 ( double secondes ) {

    TCCR1A |= (1 << COM1A0);

    TCCR1B |= (1 << CS10) | (1 << WGM12);

    TCNT1 = 0;

    OCR1A = (secondes * F_CPU);

}