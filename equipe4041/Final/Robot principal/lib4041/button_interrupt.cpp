#include "button_interrupt.h"

//active le interrupt pour le bouton
void initInterruptButton() {

    cli ();

    EIMSK |= (1 << INT0) ;

    EICRA |= (1 << ISC00) ;

    sei ();

}
