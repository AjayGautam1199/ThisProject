#ifndef UART_H
#define UART_H

#include <avr/io.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU / (16UL * BAUD)) - 1)

void uart_init(void);
void uart_send_char(char data);
void uart_send_string(const char *str);

#endif
