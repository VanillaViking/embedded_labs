#include <avr/io.h>
#include "util/delay.h"
#include "usart.h"
#include "bit.h"

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
  ADMUX &= 0xF0; // A0

  bitSet(ADCSRA, ADEN);

  while (1) {
    bitSet(ADCSRA, ADSC);
    while (ADCSRA & (1 << ADSC));
    uint16_t result = ADCL;
    result |= (ADCH << 8);
    usart_tx_string(">a:");
    usart_tx_float(result + 0.0, 3, 1);
    _delay_ms(100);
  }
}
