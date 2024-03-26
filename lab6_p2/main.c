#include <avr/io.h>
#include "util/delay.h"
#include "usart.h"
#include "bit.h"
#include <math.h>

int set_tc0_mode(char option);
int setPrescaler_tc0(char option);

int main(void) {
  usart_init(8);
  bitSet(DDRD, PIND7);
  bitSet(DDRD, PIND6);

  //setting up OCnx pwm pin
  bitClear(TCCR0A, COM0A0); 
  bitSet(TCCR0A, COM0A1); 
  
  // Fast pwm
  set_tc0_mode('3');
  setPrescaler_tc0('3');

  // reference voltage
  bitSet(ADMUX, REFS0);
  bitClear(ADMUX, REFS1);

  // ADC prescaler
  bitSet(ADCSRA, ADPS2);
  bitSet(ADCSRA, ADPS1);
  bitSet(ADCSRA, ADPS0);

  // ADC input hcannel
  ADMUX &= 0xF0; // A0

  bitSet(ADCSRA, ADEN);

  while (1) {
    bitSet(ADCSRA, ADSC);
    while (ADCSRA & (1 << ADSC));
    uint16_t result = ADCL;
    result |= (ADCH << 8);
    usart_tx_string(">a:");
    usart_tx_float(result + 0.0, 3, 1);

    OCR0A = round((1 - ((result - 40) / 700)) * 255);

    _delay_ms(100);
  }
}

int set_tc0_mode(char option) {
  switch (option) {
    // Normal
    case '0':
    bitClear(TCCR0A, WGM00);
    bitClear(TCCR0A, WGM01);
    bitClear(TCCR0B, WGM02);
    break;
    // PWM, phase correct
    case '1':
    bitSet(TCCR0A, WGM00);
    bitClear(TCCR0A, WGM01);
    bitClear(TCCR0B, WGM02);
    break;
    // CTC
    case '2':
    bitClear(TCCR0A, WGM00);
    bitSet(TCCR0A, WGM01);
    bitClear(TCCR0B, WGM02);
    break;
    // Fast PWM
    case '3':
    bitSet(TCCR0A, WGM00);
    bitSet(TCCR0A, WGM01);
    bitClear(TCCR0B, WGM02);
    break;
    // Fast PWM TOP mode
    case '4':
    bitSet(TCCR0A, WGM00);
    bitSet(TCCR0A, WGM01);
    bitSet(TCCR0B, WGM02);
    break;
  }
}

int setPrescaler_tc0(char option) {

  switch (option) {
    case '0':
      bitClear(TCCR0B, CS00);
      bitClear(TCCR0B, CS01);
      bitClear(TCCR0B, CS02);
      return 0;
      break;
    case '1':
      bitSet(TCCR0B, CS00);
      bitClear(TCCR0B, CS01);
      bitClear(TCCR0B, CS02);
      return 1;
      break;
    case '2':
      bitClear(TCCR0B, CS00);
      bitSet(TCCR0B, CS01);
      bitClear(TCCR0B, CS02);
      return 8;
      break;
    case '3':
      bitSet(TCCR0B, CS00);
      bitSet(TCCR0B, CS01);
      bitClear(TCCR0B, CS02);
      return 64;
      break;
    case '4':
      bitClear(TCCR0B, CS00);
      bitClear(TCCR0B, CS01);
      bitSet(TCCR0B, CS02);
      return 256;
      break;
    case '5':
      bitSet(TCCR0B, CS00);
      bitClear(TCCR0B, CS01);
      bitSet(TCCR0B, CS02);
      return 1024;
      break;
  }

}

