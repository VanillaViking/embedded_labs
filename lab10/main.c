#include <avr/io.h>
#include <stdint.h>
#include "util/delay.h"
#include "usart.h"
#include "bit.h"
#include <stdbool.h>
#include <avr/interrupt.h>

int setPrescaler_tc2(char option);

volatile unsigned long numOV;
volatile uint16_t high_start = 0;
volatile uint16_t high_end = 0;
volatile double freq;
volatile double measured_duty;

volatile bool is_high = false;

ISR(TIMER1_CAPT_vect) {
      /* usart_transmit('d'); */
  if (is_high) {
    is_high = false;
    high_end = ICR1;
    bitSet(TCCR1B, ICES1);
  } else {
    if (high_start > 0 && high_end > 0) {
      uint16_t now = ICR1;
      uint16_t duty_off = now - high_end;
      uint16_t duty_on = high_end - high_start;

      /* usart_tx_float(high_start, 5, 1); */
      /* usart_transmit(','); */
      /* usart_tx_float(high_end, 5, 1); */
      /* usart_transmit(','); */
      /* usart_tx_float(now, 5, 1); */
      /* usart_transmit('\n'); */

      measured_duty = 1 - ((double)duty_on / (duty_on + duty_off));
      freq = 1/((duty_on + duty_off)*0.000004);
      high_end = 0;
      high_start = 0;
      is_high = false;
    } else {
      is_high = true;
      // set to detect falling
      high_start = ICR1 + (numOV);
      bitClear(TCCR1B, ICES1);
    }
  }
}

ISR(TIMER1_OVF_vect) {
  /* numOV++; */
  /* high_start = 0; */
  /* high_end = 0; */
  /* is_high = false; */
}



int main(void) {
  cli();
  usart_init(8);
  double dutyCycle = 0.1;
  bitSet(DDRB, PB3);
  bitClear(DDRD, PD7);

  //tc1 normal mode
  bitClear(TCCR1A, WGM10);
  bitClear(TCCR1A, WGM11);
  bitClear(TCCR1B, WGM12);
  bitClear(TCCR1B, WGM13);

  bitSet(TIMSK1, TOIE1);
  
  // tc2 pwm signal
  OCR2A = (uint8_t)(256*dutyCycle);
  bitSet(TCCR2A, COM2A1);
  bitSet(TCCR2A, WGM20);
  setPrescaler_tc2(2);

  // analog comp input capture
  bitSet(ACSR, ACBG);
  bitSet(ACSR, ACIC);
  bitSet(TIMSK1, ICIE1);

  bitSet(TCCR1B, ICES1);
  // prescaler 8 TC1
  bitSet(TCCR1B, CS11);
  bitSet(TCCR1B, CS10);

  sei();

  while (1) {
    /* usart_tx_string("a>:"); */
    /* usart_tx_float(dutyCycle, 3, 1); */

    /* usart_tx_string("freq>:"); */
    /* usart_tx_float(freq, 5, 1); */

    usart_tx_string(">real:");
    usart_tx_float(dutyCycle * 100, 5, 3);
    usart_transmit('\n');
    usart_tx_string(">measured:");
    usart_tx_float(measured_duty * 100, 5, 3);
    usart_transmit('\n');


    /* usart_transmit('\n'); */

    _delay_ms(100);

    dutyCycle = dutyCycle + 0.1;
    if (dutyCycle >= 1.0) {
      dutyCycle = 0.1;
    }
    OCR2A = (uint8_t)(256*dutyCycle);
  }
}

int setPrescaler_tc2(char option)
{
  /* clear all CS0n */
  TCCR2B &= 0b11111000; // Clear bits 2:0

  /* set CS0n accordingly */
  TCCR2B |= (option - '0');

  /* return prescaler value */
  int prescalers[] = {0,1,8,64,256,1024};
  return prescalers[option - '0'];
}
