#include <avr/io.h>
#include "util/delay.h"

#define buzzerP PINB0
#define buzzerA PINC0

#define PART2

typedef struct {
    volatile uint8_t *port_ptr;
    volatile uint8_t pin;
} buzzer_t;

buzzer_t buzzer_a = {
    .port_ptr = &PORTC,
    .pin = PINC0,
};

buzzer_t buzzer_p = {
    .port_ptr = &PORTD,
    .pin = PIND7,
};

void playSound(float F, float x, int playtime_ms, buzzer_t buzzer);
float getFrequency(float octave[], char note);

int main(void) {
    DDRD |= (1 << PIND7);
    DDRC |= (1 << PINC0);


    float note5[] = {523.25, 587.33, 659.26, 698.46, 783.99, 880.00, 987.77};
    float *pNote5 = note5;
    
    #ifdef PART1
    while (1) {
        char i = 0;
        while ( i < sizeof(note5)/sizeof(float)) {
            int playtime = 100;
            buzzer_t current_buzzer = buzzer_p;

            if (i % 2 == 0) {
                playtime = 200;
                current_buzzer = buzzer_a;
            }
            playSound(*(pNote5+i), 0.5, playtime, current_buzzer);
            _delay_ms(100);
            i++;
        }
        _delay_ms(500);
    }
    #endif

    #ifdef PART2
    char melody[] = "EEFGGFEDCCDEEDDEEFGGFEDCCDEDCC";

    while (1) {
        for (int i = 0; i < sizeof(melody)/sizeof(char); i++) {
            playSound(getFrequency(note5, melody[i]), 0.1, 100, buzzer_a);
            _delay_ms(100);
        }
        _delay_ms(500);
    }
    #endif
}

void playSound(float F, float x, int playtime_ms, buzzer_t buzzer) {
    float T = 1/F*1e6;
    int Ton= x*T;
    int Toff = T - Ton;

    for (int j = 0; j < ((2e3/T)*playtime_ms); j++) {
        *(buzzer.port_ptr) |= (1 << buzzer.pin);
        for (int i = 0; i < Ton; i++) {
            _delay_us(1);
        }
        *(buzzer.port_ptr) = *(buzzer.port_ptr) & ~(1 << buzzer.pin);
        for (int i = 0; i < Toff; i++) {
            _delay_us(1);
        }
    }
}

float getFrequency(float octave[], char note) {
    switch (note) {
        case 'C': 
            return octave[0];
            break;
        case 'D': 
            return octave[1];
            break;
        case 'E': 
            return octave[2];
            break;
        case 'F': 
            return octave[3];
            break;
        case 'G': 
            return octave[4];
            break;
        case 'A': 
            return octave[5];
            break;
        case 'B': 
            return octave[6];
            break;
    }
}


