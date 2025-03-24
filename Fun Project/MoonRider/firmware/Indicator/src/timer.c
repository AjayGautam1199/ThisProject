#include "timer.h"

void timer_init(void) {
    TCCR1B |= (1 << WGM12) | (1 << CS12) | (1 << CS10);
    OCR1A = 15624;  
    TIMSK1 |= (1 << OCIE1A);
}
