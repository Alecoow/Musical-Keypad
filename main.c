/*
 * GccApplication2.c
 *
 * Created: 3/13/2025 11:30:37 PM
 * Author : Alex Cooper
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#define F_CPU 16000000UL

const unsigned char keypad[4][4] = {
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

char Keypad_Scan() {
	for (int i = 4; i < 8; i++)  {
		PORTD &= ~(1 << i); // Disable row pullup resistor
		for (int j = 0; j < 4; j++) {
			if (!(PINB & (1 << j))) {
				return keypad[i-4][j];
			}
		}
		PORTD |= (1 << i); // reenable row pullup resistor
	}
	return '?';
}

void Timer_Count(float frequency) {
	TCCR0A |= (1 << WGM01);
	OCR0A = frequency;
	OCR0B = frequency / 2.0; // 50% duty cycle
	TCCR0B |= (1 << 2) | (1 << 0);
	PORTC |= (1 << 4) | (1 << 5); // enable speaker output
	
	while (!(TIFR0 & (1 << OCF0B))) {} // wait for OCR0B overflow
	TIFR0 |= 1 << OCF0B; // reset flag
	
	PORTC &= ~(1 << 4) & ~(1 << 5); // reset speaker output
	
	while (!(TIFR0 & (1 << OCF0A))) {} // wait for OCR0A overflow
	TIFR0 |= 1 << OCF0A; // reset flag
}

float Calculate_Frequency(char key) { 
	double period = 1.059463; // 2 ^ (1/12)
	period = pow(period, (double)key); // period ^ key
	return (1.0 / (440 * period)) * 16000.0; // 1 / (440HZ * period) * clock speed
}

int main(void) {
	Init(); // Set up register flags
	
    while (1) {
		char input = Keypad_Scan(); // Scan keypad for input
		if (input != '?') {
			float key = Calculate_Frequency(input);
			Timer_Count(key);
		}
    }
	
	return 0;
}

