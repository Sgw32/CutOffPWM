// Libraries included
#include <avr/io.h>
#include <stdint.h>
#include "pwm.h"


/*      PPM INPUT
 *      ---
 *      ICP5    Pin48 on Arduino Mega
 */
void ppm_input_init(void)
{
    DDRB &= ~( 1 << PORTB0 ); // set ICP5 as an input

	TCCR1A = 0; //Clear TCCR1A
	TCCR1B = 0; //Clear TCCR1B
	TCCR1B |= (1 << CS11) | (0 << CS10); //Set Timer1 prescaler to 8
	TCCR1B |= (1 << ICNC1); //Enable input capture noise canceller
    TCCR1B |= (1 << ICES1);
	
	TIMSK |= (1 << TICIE1); //Enable input capture
	TIFR = (1 << ICF1); //Clear interrupt flag

	sei(); //Enable Interrupts
}

void set_desired(int _desired);

// Interrupt service routine for reading PPM values from the radio receiver.
ISR( TIMER1_CAPT_vect )
{
    static uint16_t tStart;

    uint16_t t = ICR1;
    if (TCCR1B & (1 << ICES1)) { // rising
	    TCCR1B &= ~(1 << ICES1);
	    tStart = t;
    }
	else { // falling
	    TCCR1B |= (1 << ICES1);
		uint16_t ppm = (t - tStart) / 2;
		if (ppm >= 1000 && ppm <= 2000)
			set_desired(ppm > 1500 ? 0 : 1);
    }
}