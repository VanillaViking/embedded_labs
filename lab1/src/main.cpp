#include <avr/io.h>
#include "util/delay.h"
#include <stdint.h>

#define ledBuiltIn PINB5
#define delayTime 300

int main(void) {
  DDRD = 255;
  
  while (1) {

    uint8_t port_d_outputs = 0b10000000;

    for (int i = 0; i <= 8; i++) {
      PORTD = port_d_outputs;
      _delay_ms(delayTime);
      port_d_outputs = port_d_outputs >> 1;
    }

    port_d_outputs = 0b00000001;

    for (int i = 0; i <= 8; i++) {
      PORTD = port_d_outputs;
      _delay_ms(delayTime);
      port_d_outputs = port_d_outputs << 1;
    }

    port_d_outputs = 0b11000000;

    for (int i = 0; i <= 8; i++) {
      PORTD = port_d_outputs;
      _delay_ms(delayTime);
      port_d_outputs = port_d_outputs >> 1;
    }

    port_d_outputs = 0b10000000;

    for (int i = 0; i <= 4; i++) {
      PORTD = port_d_outputs;
      _delay_ms(delayTime);
      port_d_outputs = port_d_outputs >> 2;
    }

    port_d_outputs = 0b01000000;

    for (int i = 0; i <= 4; i++) {
      PORTD = port_d_outputs;
      _delay_ms(delayTime);
      port_d_outputs = port_d_outputs >> 2;
    }

  }
}
