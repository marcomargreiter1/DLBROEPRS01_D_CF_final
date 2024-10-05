#define F_CPU 16000000UL  // CPU-Frequenz definieren
#define BAUD 9600         // Baudrate definieren

#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

void uart_init(void) {
    // Baudrate anpassen
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    // Empfangen und Senden
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);

    // Format Einstellung: 8 data bits, 1 stop bit
    UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
}

void uart_putchar(char c) {
    // Warten auf Buffer
    while (!(UCSR0A & (1<<UDRE0)));
    // Daten in Buffer laden, senden der Daten
    UDR0 = c;
}

char uart_getchar(void) {
    // Warten auf empfangene Daten
    while (!(UCSR0A & (1<<RXC0)));
    // Zurücksenden der Daten
    return UDR0;
}

void uart_putstr(const char* str) {
    while (*str) {
        uart_putchar(*str++);
    }
}

void uart_getstr(char* buffer, uint8_t max_length) {
    uint8_t i = 0;
    char c;
    while (i < max_length - 1) {
        c = uart_getchar();
        if (c == '\n' || c == '\r') { 
            break;
        }
        buffer[i++] = c;
    }
    buffer[i] = '\0';
}

// Funktion zur Berechnung eines einfachen mathematischen Ausdrucks wie "3*3"
int calculate(const char* expression) {
    char operand1[10], operand2[10], operator;
    int i = 0, j = 0;

    // Operand 1 extrahieren
    while (isdigit(expression[i]) && i < strlen(expression)) {
        operand1[i] = expression[i];
        i++;
    }
    operand1[i] = '\0';  // Null-terminiere den ersten Operanden

    // Operator extrahieren
    if (i < strlen(expression)) {
        operator = expression[i++];
    } else {
        return 0;  // Ungültiger Ausdruck
    }

    // Operand 2 extrahieren
    while (isdigit(expression[i]) && i < strlen(expression)) {
        operand2[j++] = expression[i++];
    }
    operand2[j] = '\0';  // Null-terminiere den zweiten Operanden

    // Konvertiere Operanden zu Integern
    int num1 = atoi(operand1);
    int num2 = atoi(operand2);

    // Berechnung basierend auf dem Operator
    switch (operator) {
        case '+': return num1 + num2;
        case '-': return num1 - num2;
        case '*': return num1 * num2;
        case '/': return (num2 != 0) ? num1 / num2 : 0; // Division durch Null verhindern
        default: return 0; // Ungültiger Operator
    }
}

int main(void) {
    uart_init();
    char buffer[50];  // Buffer für empfangene Nachrichten
    char result_str[20];  // Buffer für das Ergebnis als Zeichenkette

    while (1) {
        uart_getstr(buffer, sizeof(buffer));  // String vom UART empfangen

        // Berechnung durchführen und Ergebnis senden
        int result = calculate(buffer);
        itoa(result, result_str, 10);  // Konvertiere das Ergebnis in eine Zeichenkette
        uart_putstr(result_str);
        uart_putchar('\n');  // Zeilenumbruch senden
    }
}
