/*
 * CutOffPWM.c
 *
 * Created: 13.05.2018 18:21:56
 * Author : 1
 */ 
# define F_CPU 16000000UL

#define TICKS_PER_MS (F_CPU / 1000)

#define RELAY_DELAY 100 // millis - задержка размыкания реле
#define PWM_DELAY 2048 // millis - задержка до замыкания реле
#define INTER_DELAY 3000
#define PWM_MIN 16
#define PWM_MAX 32 // ticks of TIMER2
#define PWM_PER_TICK (PWM_MAX - PWM_MIN) / PWM_DELAY

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "millis.h"
#include "pwm.h"

unsigned int a,b,c,high,period;

const int timer2_prescaler = (1 << CS22) | (1 << CS21) | (1 << CS20);
millis_t relay_done, pwm_done;
 
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
	PORTC = PORTC ^ 2;	
}

ISR (INT0_vect)
{
	TCNT2 = 0;
	timer2_start();
	PORTC = PORTC ^ 1;
}

void inputinterrupt_init()
{
	MCUCR |= (1 << ISC01) | (1 << ISC00);    // set INT0 to trigger on ANY logic change
	GICR |= (1 << INT0);      // Turns on INT0
}

void timer2_init()
{
	OCR2 = 0x7F;	
	TIMSK |= (1 << OCIE2);
	TCNT2 = 0xFF;
}

int main(void)
{
    PORTD = 0x00;
    DDRD = 0b11000011; //0b11000011, some input(PWM)
	
	PORTB = 0x00;
	DDRB = 0b11111110; //All output,expect last one

	PORTC = 0x00;
	DDRC = 0b11111111; //0b11110000, some input(PWM)
	
	ppm_input_init();
	millis_init();
	timer2_init();
	inputinterrupt_init();
	
	ppm.ch[0]=0;
	
	
	int pwm_limit = 0;
	//const int pwm_lim_per_tick = PWM_MAX / PWM_DELAY;
	
	// t2 prescaler 1024
	// ~16khz freq
	// 1 tick = 1/16 ms
	// 700ms = 700*16 ticks
	//pwm_max_ticks = 700*16
	// @7000: 2000 (2ms)
	// 2ms = 32 ticks
	// 
	
	int desired = 1;
	
	sei();	
	
    while (1) 
    {
		millis_t now = millis();
		
		if (ppm.ch[0]>0)  	
		{
			int rc_desired = (ppm.ch[0]>3043) ? 0 : 1; //1 - проходит(при старте)
			if (rc_desired != desired) {
				desired = rc_desired;
				if (desired)
					relay_done = now + RELAY_DELAY;
				else
					pwm_done = now + PWM_DELAY;
			}
			ppm.ch[0]=0;
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
		
		if (TCNT2 < pwm_limit) {
			PORTB=(PORTB&0b11000011)|(PIND&(0b00111100)); //После выключения реле передаём данные с порта.			
		}	
		else
		{
			PORTB&=0b11000011;
		}
    }
}

