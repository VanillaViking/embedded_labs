#include <avr/io.h>
#include "util/delay.h"
#include "usart.h"

#define LED_BUILTIN PINB5
#define DELAY 500


int main(void) {
  DDRB = 0xFF;

  while (1) {
    PORTB ^= 1 << LED_BUILTIN;
    _delay_ms(DELAY);
  }
}
