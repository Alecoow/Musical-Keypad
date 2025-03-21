/*
 * GccApplication2.c
 *
 * Created: 3/13/2025 11:30:37 PM
 * Author : Alex Cooper
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>


const unsigned int keypad[4][4] = {
	{0, 1, 2, 3},
	{4, 5, 6, 7},
	{8, 9, 10, 11},
	{12, 13, 14, 15},
};

void Init() {
	// Initialize keypad
	DDRD |= (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7); // Set registers PD4-7 to high (output) // Acts as rows
	PORTD |=  (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7); // Set rows to high
	DDRB &= ~(1 << 0) & ~(1 << 1) & ~(1 << 2) & ~(1 << 3); // Set registers PB0-3 to low (input) // Acts as columns
	PORTB |=  (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3); // enable pullup resistors for the columns
	// Initialize audio
	DDRC |= (1 << 4) | (1 << 5); // audio output
}

int Keypad_Scan() {
	for (int i = 4; i < 8; i++)  {
		PORTD &= ~(1 << i); // Disable row pullup resistor
		for (int j = 0; j < 4; j++) {
			if (!(PINB & (1 << j))) {
				PORTD |= (1 << i); // reenable row pullup resistor
				return keypad[i-4][j];
			}
		}
		PORTD |= (1 << i); // reenable row pullup resistor
	}
	return '?';
}

void Play_Sound(float frequency) {
	TCCR0A |= (1 << WGM01); // ctc
	OCR0A = frequency; // 
	OCR0B = frequency / 2.0; // 50% duty cycle
	TCCR0B |= (1 << CS02); // prescale 256
	PORTC |= (1 << 4) | (1 << 5); // enable speaker output
	
	while (!(TIFR0 & (1 << OCF0B))) {} // wait for OCR0B overflow
	TIFR0 |= 1 << OCF0B; // reset flag
	
	PORTC &= ~(1 << 4) & ~(1 << 5); // reset speaker output
	
	while (!(TIFR0 & (1 << OCF0A))) {} // wait for OCR0A overflow
	TIFR0 |= 1 << OCF0A; // reset flag
}

float Calculate_Frequency(int key) { 
	float period = 1.059463; // 2 ^ (1/12)
	period = 440 *(pow(period, (double)key)); // 440 (middle frequency) * period ^ key
	return (F_CPU/(256*period)) - 1;
}
/*
	float period = 1.059463; // 2 ^ (1/12)
	period = 440 *(pow(period, (double)key)); // 440 * period ^ key
	return (F_CPU/(256*period)) - 1;
	
	float frequency = 440 * pow(pow(2.0, (1.0 / 12.0)), index);
	return (F_CPU/(256*frequency)) - 1;
*/
int main(void) {
	Init(); // Set up register flags
	
    while (1) {
		int input = Keypad_Scan(); // Scan keypad for input
		if (input != '?') {
			float key = Calculate_Frequency(input);
			Play_Sound(key);
		}
    }
	
	return 0;
}


