#include <avr/io.h>
#include "util/delay.h"
#include "usart.h"
#include "bit.h"

int main(void) {
  // TOP
  OCR1A = 10000;

  // compare match (50% duty)
  OCR1B = 5000;

  // TC1 phase correct
  bitSet(TCCR1B, WGM13);
  bitSet(TCCR1A, WGM10);

  bitSet(TCCR1A, COM1A1);

  //prescaler: 8
  bitSet(TCCR1B, CS11);

  bitSet(DDRD, PD4);
  bitSet(DDRD, PD3);
  bitSet(DDRB, PB1);

  bitSet(PORTD, PD4);
  bitClear(PORTD, PD3);

  while (1) {}

}
