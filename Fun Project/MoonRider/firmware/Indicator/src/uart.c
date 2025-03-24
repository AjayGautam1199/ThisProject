#include "uart.h"

void uart_init(void) {
    UBRR0H = (UBRR_VALUE >> 8);
    UBRR0L = UBRR_VALUE;
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_send_char(char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

void uart_send_string(const char *str) {
    while (*str) {
        uart_send_char(*str++);
    }
}
