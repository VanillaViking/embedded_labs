#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>
#include <stdbool.h>
#include <math.h>

#include "usart.h"
#include "bit.h"

int setPrescaler_tc0(char option);
int set_tc0_mode(char option);
double get_frequency(char note);
uint8_t find_top(double frequency);

/* ISR(TIMER0_OVF_vect) */
/* { */
/* } */

/* ISR(TIMER0_COMPA_vect) */
/* { */
/* } */

int main(void) {
    usart_init(8);
    bitSet(DDRD, PIND6);

    //setting up OCnx pwm pin
    bitSet(TCCR0A, COM0A0); // set to toggle OC0x every time it reaches TOP
    bitClear(TCCR0A, COM0A1); 

    // set TOP
    set_tc0_mode('4');

    while (1) {
      OCR0A = find_top(get_frequency('C'));
      _delay_ms(200);
      OCR0A = find_top(get_frequency('D'));
      _delay_ms(200);
      OCR0A = find_top(get_frequency('E'));
      _delay_ms(200);
      OCR0A = find_top(get_frequency('F'));
      _delay_ms(200);
      OCR0A = find_top(get_frequency('G'));
      _delay_ms(200);
      OCR0A = find_top(get_frequency('A'));
      _delay_ms(200);
      OCR0A = find_top(get_frequency('B'));
      _delay_ms(200);
      

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

uint8_t find_top(double frequency) {
  double top_values[6];
  uint8_t top;

  top_values[0] = 999;
  top_values[1] = 16000000/(2 * frequency * 1);
  top_values[2] = 16000000/(2 * frequency * 8);
  top_values[3] = 16000000/(2 * frequency * 64);
  top_values[4] = 16000000/(2 * frequency * 256);
  top_values[5] = 16000000/(2 * frequency * 1024);

  for (int i = 0; i < 6; i++) {
    if (top_values[i] < 256) {
      usart_tx_float(i + 0.0, 3, 1);
      usart_tx_float(top_values[i], 3, 1);
       top = round(top_values[i]);
       setPrescaler_tc0(i + '0');
       break;
    }
  }
  
  return top;
}

double get_frequency(char note) {
  switch (note) {
    case 'C':
      return 1046.5;
      break;
    case 'D':
      return 1174.66;
      break;
    case 'E':
      return 1318.51;
      break;
    case 'F':
      return 1396.91;
      break;
    case 'G':
      return 1567.91;
      break;
    case 'A':
      return 1760.0;
      break;
    case 'B':
      return 1975.53;
      break;
  }
}
