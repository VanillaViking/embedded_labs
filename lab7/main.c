#include <avr/io.h>
#include "util/delay.h"
#include "bit.h"
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdlib.h>

#define FOSC 16000000
#define BAUD 9600
#define MYUBRR FOSC / 16 / BAUD - 1 

#define TRIG PIND7
#define ECHO PIND2
#define BUTTON PIND3

#define SPEED_SOUND 0.034

void USART_init(unsigned int ubrr);
void USART_Transmit(unsigned char data);
void transmit_text(char* str);
long get_pulse(void);
void set_tc0_mode(char mode);
int setPrescaler_tc0(char option);

volatile int numOV = 0;
volatile int duration_end = 0;
volatile bool on = true;

ISR(INT0_vect) {
    duration_end = 1;
}

ISR(INT1_vect) {
    on = !on;
}

ISR(TIMER0_OVF_vect) {
  numOV++;
}

int main(void) {
    USART_init(MYUBRR);
    DDRD |=  1 << TRIG;
    DDRD &= ~(1 << ECHO);
    DDRD &= ~(1 << BUTTON);
    DDRB |= (1 << PINB5);

    PORTB &= ~(1 << PINB5);
    PORTD |= (1 << BUTTON);

    bitSet(EIMSK, INT0);
    bitSet(EICRA, ISC00);

    bitSet(EIMSK, INT1);
    bitSet(EICRA, ISC10);
    bitSet(EICRA, ISC11);

    // set timer 0 overflow
    bitSet(TIMSK0, TOIE0);

    set_tc0_mode('0');
    int p = setPrescaler_tc0('1');

    sei();

    _delay_ms(700);

    while (1) {

	if (on == false) {
	    continue;
	}

	/* int pressed = (PIND & 1 << BUTTON); */
        /* _delay_ms(100); */
	/* if ((PIND & 1 << BUTTON) == !pressed) { */
	/*     PORTB ^= (1 << PINB5); */
	/* } */

        _delay_ms(100);

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

int setPrescaler_tc0(char option)
{
  /* clear all CS0n */
  TCCR0B &= 0b11111000; // Clear bits 2:0

  /* set CS0n accordingly */
  TCCR0B |= (option - '0');

  /* return prescaler value */
  int prescalers[] = {0,1,8,64,256,1024};
  return prescalers[option - '0'];
}

void set_tc0_mode(char mode)
{
  /* clear all WGM0n */
  TCCR0A &= 0b11111100; // Clear bits 1:0
  TCCR0B &= 0b11110111; // Clear bit 3

  /* set WGM0n accordingly */
  TCCR0A |= (0b00000011 & (mode - '0'));
  TCCR0B |= (0b00001000 & ((mode - '0') << 1));
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

  TCNT0 = 0;
  numOV = 0;
  duration_end = 0;
  while (duration_end == 0) {
  }

  return numOV * 16; // in microseconds
}

void delay_microsec() {
  numOV = 0;

  unsigned long numOV_max = 1;
  while (numOV < numOV_max -1);
}
