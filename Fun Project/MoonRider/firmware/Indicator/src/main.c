#include <avr/interrupt.h>
#include <util/delay.h>
#include "gpio.h"
#include "timer.h"
#include "uart.h"
#include <avr/io.h>
volatile uint8_t left_pressed = 0;
volatile uint8_t right_pressed = 0;
volatile uint8_t hazard_mode = 0;

void check_buttons() {
    if ((PINC & (1 << PC2)) == 0) {
        _delay_ms(1000); 
        if ((PINC & (1 << PC2)) == 0) {
            if (hazard_mode) hazard_mode = 0;
            else left_pressed = !left_pressed;
        }
    }

    if ((PINC & (1 << PC3)) == 0) {
        _delay_ms(1000); 
        if ((PINC & (1 << PC3)) == 0) {
            if (hazard_mode) hazard_mode = 0;
            else right_pressed = !right_pressed;
        }
    }

    if (((PINC & (1 << PC2)) == 0) && ((PINC & (1 << PC3)) == 0)) {
        _delay_ms(1000);
        if (((PINC & (1 << PC2)) == 0) && ((PINC & (1 << PC3)) == 0)) {
            hazard_mode = 1;
        }
    }
}

ISR(TIMER1_COMPA_vect) {
    static uint16_t counter = 0;
    counter++;
    if (counter >= 300) {
        if (hazard_mode) {
            PORTC ^= (1 << PC0) | (1 << PC1);
        } else if (left_pressed) {
            PORTC ^= (1 << PC0);
            PORTC &= ~(1 << PC1);
        } else if (right_pressed) {
            PORTC ^= (1 << PC1);
            PORTC &= ~(1 << PC0);
        } else {
            PORTC &= ~((1 << PC0) | (1 << PC1));
        }
        counter = 0;
    }
}

int main(void) {
    gpio_init();
    timer_init();
    uart_init();

    sei();

    while (1) {
        check_buttons();
        _delay_ms(100);
    }
}
