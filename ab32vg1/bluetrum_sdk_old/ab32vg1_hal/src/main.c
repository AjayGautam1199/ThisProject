#include "ab32vg1_hal.h"
#include "ab32vg1_hal_gpio.h"
#include "ab32vg1_hal_rcu.h"

#define LED1_PIN GPIO_PIN_0  // LED1 connected to PA0
#define LED2_PIN GPIO_PIN_1  // LED2 connected to PA1

// Simple delay function in milliseconds
void delay_ms(uint32_t ms) {
    // Adjust the loop based on system clock for approximate timing
    for (uint32_t i = 0; i < ms * 1000; i++) {
        asm("nop");  // No operation to consume CPU cycles
    }
}

int main(void) {
    // GPIO initialization structure
    struct gpio_init gpio_init_struct;
    gpio_init_t gpio_init_ptr = &gpio_init_struct;
    gpio_init_ptr->pin = LED1_PIN | LED2_PIN;
    gpio_init_ptr->dir = GPIO_DIR_OUTPUT;
    gpio_init_ptr->de = GPIO_DIGITAL;
    gpio_init_ptr->pull = GPIO_NOPULL;
    hal_gpio_init(GPIOA_BASE, gpio_init_ptr); // Pass pointer

    // Infinite loop to blink LEDs 
    while (1) {
        // Toggle the state of both LEDs
        hal_gpio_toggle(GPIOA_BASE, LED1_PIN);
        hal_gpio_toggle(GPIOA_BASE, LED2_PIN);

        // Delay for 500ms to make blinking visible
        delay_ms(500);
    }

    return 0;  // Never reached
}
