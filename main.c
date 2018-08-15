#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "millis.h"

#define TICKS_PER_MS (F_CPU / 1000)

#define RELAY_DELAY 100 // millis - задержка размыкания реле
#define PWM_DELAY 2048 // millis - задержка до замыкания реле
#define INTER_DELAY 3000
#define PWM_MIN 16
#define PWM_MAX 32 // ticks of TIMER2
#define PWM_PER_TICK (PWM_MAX - PWM_MIN) / PWM_DELAY

volatile uint8_t rc_desired = 1;

const int timer2_prescaler = (1 << CS22) | (1 << CS21) | (1 << CS20);
 
inline void timer2_start()
{
	TCCR2 = timer2_prescaler;
}

inline void timer2_stop()
{
	TCCR2 = 0;
}

ISR (TIMER2_COMP_vect)
{
	timer2_stop();
	//PORTC = PORTC ^ 2;	
}

void timer2_init()
{
	OCR2 = 0x7F;
	TIMSK |= (1 << OCIE2);
	TCNT2 = 0xFF;
}

ISR (INT0_vect)
{
	TCNT2 = 0;
	timer2_start();
	PORTC = PORTC ^ 1;
}

void inputinterrupt_init()
{
	MCUCR |= (1 << ISC01) | (1 << ISC00);    // INT0 on rising edge
	GICR |= (1 << INT0);      // Turns on INT0
}

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
}

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
			rc_desired = ppm > 1500 ? 0 : 1;
	}
}

int main(void)
{
    PORTD = 0x00;
    DDRD = 0b11000011; //0b11000011, some input(PWM)
	
	PORTB = 0x00;
	DDRB = 0b11111110; //All output,expect last one

	PORTC = 0x00;
	DDRC = 0b11111111; //All output

	millis_init();	
	ppm_input_init();
	timer2_init();
	inputinterrupt_init();

	int pwm_limit = 0;
	
	millis_t relay_done = 0, pwm_done = 0;
	uint8_t desired = rc_desired;
	
	sei();	
	
    while (1) 
    {
		millis_t now = millis();
		
		if (rc_desired != desired) {
			desired = rc_desired;
			if (desired)
				relay_done = now + RELAY_DELAY;
			else
				pwm_done = now + PWM_DELAY;
		}
		
			
		if (desired) { //при старте эта ветка
			PORTD&=~(1<<PORTD7); // turn off relay immediately
			if (now < relay_done)
				pwm_limit = 0;
			else if (now > relay_done + PWM_DELAY)
				pwm_limit = PWM_MAX;
			else
				pwm_limit = PWM_MIN + (now - relay_done) * PWM_PER_TICK;
		}
		else {
			pwm_limit = (now < pwm_done) ? ((pwm_done - now) * PWM_PER_TICK + PWM_MIN) : 0;
			if (now > pwm_done + INTER_DELAY)
				PORTD|=(1<<PORTD7);
		}
		
		if (TCNT2 < pwm_limit)
			PORTB=(PORTB&0b11000011)|(PIND&(0b00111100)); //После выключения реле передаём данные с порта.			
		else
			PORTB&=0b11000011;
		
		if (desired)
			PORTC|=(1<<PORTC1);
		else
			PORTC&=~(1<<PORTC1);
    }
}

