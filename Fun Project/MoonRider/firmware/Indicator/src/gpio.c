#include "gpio.h"

void gpio_init(void) {
    DDRC |= (1 << PC0) | (1 << PC1);  
    DDRB |= (1 << PB0);  
    DDRC &= ~((1 << PC2) | (1 << PC3)); 
    PORTC |= (1 << PC2) | (1 << PC3);  
}
