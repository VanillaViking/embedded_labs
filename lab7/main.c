#include <avr/io.h>
#include "util/delay.h"
#include <stdlib.h>

#define FOSC 16000000
#define BAUD 9600
#define MYUBRR FOSC / 16 / BAUD - 1 

#define BITSET(reg, n) (reg |= 1 << n)
#define BITREAD(reg, n) (reg & 1 << n)
#define BITCLEAR(reg, n) (reg &= ~(1 << n))

#define TRIG PIND7
#define ECHO PIND6
#define BUTTON PIND3

#define SPEED_SOUND 0.034

void USART_init(unsigned int ubrr);
void USART_Transmit(unsigned char data);
void transmit_text(char* str);
long get_pulse(void);

int main(void) {
    USART_init(MYUBRR);
    DDRD |=  1 << TRIG;
    DDRD &= ~(1 << ECHO);
    DDRD &= ~(1 << BUTTON);
    DDRB |= (1 << PINB5);

    PORTB &= ~(1 << PINB5);
    PORTD |= (1 << BUTTON);

    _delay_ms(700);

    while (1) {

	int pressed = (PIND & 1 << BUTTON);
        _delay_ms(100);
	if ((PIND & 1 << BUTTON) == !pressed) {
	    PORTB ^= (1 << PINB5);
	}

	PORTD &= ~(1 << TRIG);
	_delay_us(2);
	PORTD |= 1 << TRIG;
	_delay_us(10);
	PORTD &= ~(1 << TRIG);
	long time = get_pulse();

	double distance_cm = time * SPEED_SOUND/2;
	char char_buf[8];

	if (!(PINB & (1 << PINB5))) {
	    dtostrf(distance_cm, 7, 1, char_buf);
	    transmit_text(">a:");
	    transmit_text(char_buf);
	    USART_Transmit('\n');
	} else {
	    dtostrf(distance_cm * 0.3937, 7, 3, char_buf);
	    transmit_text(">b:");
	    transmit_text(char_buf);
	    USART_Transmit('\n');
	}


    }

}

void USART_init(unsigned int ubrr) {
    // baud rate
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    
    // enable receiver and transmitter
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);

    //set frame format: 8 data, 2stop bit
    UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void USART_Transmit(unsigned char data) {
    // wait for empty transmit  buffer
    while (!(UCSR0A & (1<<UDRE0)));
    UDR0 = data;
}

void transmit_text(char* str) {
    while (*str != '\0') {
        USART_Transmit(*str);
        str++;
    }
}

long get_pulse(void) {
  long time_elapsed = 0;

  while (!(PIND & (1 << ECHO))) {
  }

  while ((PIND & (1 << ECHO))) {
    _delay_us(1);
    time_elapsed += 1;
  }


  return time_elapsed;
}
