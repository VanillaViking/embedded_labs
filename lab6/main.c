#include <avr/io.h>
#include "util/delay.h"
#include "usart.h"
#include "bit.h"

void swap();

int is_y = 1;

int main(void) {
  // reference voltage
  usart_init(8);
  bitSet(ADMUX, REFS0);
  bitClear(ADMUX, REFS1);

  // ADC prescaler
  bitSet(ADCSRA, ADPS2);
  bitSet(ADCSRA, ADPS1);
  bitSet(ADCSRA, ADPS0);

  // ADC input hcannel
  bitSet(ADMUX, MUX0);
  bitClear(ADMUX, MUX1);
  bitClear(ADMUX, MUX2);
  bitClear(ADMUX, MUX3);


  bitSet(ADCSRA, ADEN);

  while (1) {
    bitSet(ADCSRA, ADSC);
    while (ADCSRA & (1 << ADSC));
    uint16_t result = ADCL;
    result |= (ADCH << 8);
    if (is_y) {
      usart_tx_string(">y:");
      usart_tx_float(result, 5, 1);
    } else {
      usart_tx_string(">x:");
      usart_tx_float(result, 5, 1);

    }
    usart_transmit('\n');
    swap();
    _delay_ms(10);
  }
}

void swap() {
  if (is_y == 1) {
    is_y = 0;
    bitClear(ADMUX, MUX0);
    bitClear(ADMUX, MUX1);
    bitClear(ADMUX, MUX2);
    bitClear(ADMUX, MUX3);
  } else {
    is_y = 1;
    bitSet(ADMUX, MUX0);
    bitClear(ADMUX, MUX1);
    bitClear(ADMUX, MUX2);
    bitClear(ADMUX, MUX3);
  }
}
