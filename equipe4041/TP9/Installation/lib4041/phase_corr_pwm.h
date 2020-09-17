#ifndef PHASE_CORR_PWM_H
#define PHASE_CORR_PWM_H

#include <avr/io.h>
#include <avr/interrupt.h>

void setPhaseCorrectPWM (uint8_t powerSignal1, uint8_t powerSignal2);

#endif  /* PHASE_CORR_PWM_H */