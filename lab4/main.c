#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>

#include "usart.h"
#include "bit.h"

int setPrescaler_tc0(char option);
void my_delay_us(unsigned long x);
int minimum(int remainder_list[]); 

void my_delay_1e6us();

volatile unsigned long numOV = 0;
volatile int is_waiting = 0;

ISR(TIMER0_OVF_vect)
{
  numOV++;
}

ISR(TIMER0_COMPA_vect)
{
  is_waiting = 0;
}

#define PART2


#ifdef PART1
int main(void) {
    DDRD = 0xFF;
    usart_init(8);
    bitSet(TIMSK0, TOIE0);
    /* bitSet(TCCR0A, WGM01); */
    /* bitSet(TIMSK0, OCIE0A); */
    sei();

    while (1) {
      usart_tx_string(">a:");
      usart_transmit('1');
      usart_transmit('\n');
      my_delay_1e6us();
      /* my_delay_us(10000); */

      usart_tx_string(">a:");
      usart_transmit('0');
      usart_transmit('\n');
      _delay_us(1e6);
      
      /* my_delay_us(10000); */
      /* bitSet(PORTD, PIND5): */

    }
}
#endif
#ifdef PART2
int main(void) {
    DDRD = 0xFF;
    usart_init(8);
    bitSet(TCCR0A, WGM01);
    bitSet(TIMSK0, OCIE0A);
    sei();

    while (1) {
      my_delay_us(10000);
      bitSet(PORTD, PIND5);
      my_delay_us(10000);
      bitClear(PORTD, PIND5);
    }
}
#endif

void my_delay_1e6us() {
  unsigned long numOV_max = 62500;

  numOV = 0;
  TCNT0 = 0;
  /* bitSet(TCCR0B, CS00); */
  setPrescaler_tc0('1');
  while (numOV < numOV_max -1);

  bitClear(TCCR0B, CS00);
}

void my_delay_us(unsigned long x) {
  int ticks;
			  // setting extremely high value just to start out
  int remainder_list[5] = {10000, 10000, 10000, 10000, 10000};
  is_waiting = 1;
  
  // if x is small enough that prescaler=8 can be used, simply use it as it is the most accurate
  if (x < 128) {
    ticks = x * 2;
    setPrescaler_tc0('2');
    OCR0A = ticks;
    while (is_waiting);
    setPrescaler_tc0('0');
    return;
  }

  if (x < 1024) {
    remainder_list[2] = x % 4;
  }

  if (x < 4096) {
    remainder_list[3] = x % 16;
  }

  if (x < 16384) {
    remainder_list[4] = x % 64;
  }
  
  // need to figure out which prescaler would be the most precise
  int prescaler = minimum(remainder_list);

  // basically
  // if P=64, ticks to wait = x / 4
  // if P=256, ticks to wait = x / 16
  // if P=1024, ticks to wait = x / 64
  ticks = round(x / (double)(round(pow(4, prescaler -1))));

  // convert int to char
  char P = (prescaler+1) + '0';

  /* usart_tx_string("prescaler:"); */
  /* usart_transmit(P); */

  setPrescaler_tc0(P);
  OCR0A = ticks;
  /* usart_tx_string("ticks:"); */
  /* usart_tx_float(ticks, 4, 1); */
  while (is_waiting);
  setPrescaler_tc0('0');
  return;
}

int setPrescaler_tc0(char option) {

  switch (option) {
    case '0':
      bitClear(TCCR0B, CS00);
      bitClear(TCCR0B, CS01);
      bitClear(TCCR0B, CS02);
      break;
    case '1':
      bitSet(TCCR0B, CS00);
      bitClear(TCCR0B, CS01);
      bitClear(TCCR0B, CS02);
      break;
    case '2':
      bitClear(TCCR0B, CS00);
      bitSet(TCCR0B, CS01);
      bitClear(TCCR0B, CS02);
      break;
    case '3':
      bitSet(TCCR0B, CS00);
      bitSet(TCCR0B, CS01);
      bitClear(TCCR0B, CS02);
      break;
    case '4':
      bitClear(TCCR0B, CS00);
      bitClear(TCCR0B, CS01);
      bitSet(TCCR0B, CS02);
      break;
    case '5':
      bitSet(TCCR0B, CS00);
      bitClear(TCCR0B, CS01);
      bitSet(TCCR0B, CS02);
      break;
  }

}

int minimum(int remainder_list[]) {
  int min = 1000;
  int min_idx;

  for (int i = 0; i < 5; i++) {
    if (remainder_list[i] < min) {
      min = remainder_list[i];
      min_idx = i;
    }
  }

  return min_idx;
}
