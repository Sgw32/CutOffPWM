/*
 * CutOffPWM.c
 *
 * Created: 13.05.2018 18:21:56
 * Author : 1
 */ 
# define F_CPU 16000000UL

#define TICKS_PER_MS (F_CPU / 1000);

#define RELAY_DELAY 100 // millis
#define PWM_DELAY 700 // millis

#define PWM_MAX (2500 * TICKS_PER_MS) / 1000 // ticks of TIMER1?

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "pwm.h"

unsigned int a,b,c,high,period;
int enabled;
unsigned long relay_cnt;

/*ISR( TIMER2_COMP_vect)
{
	if (enabled)
		PORTB=(PORTB&0b11000011)|(PIND&(0b00111100));
}*/


int main(void)
{
    PORTD = 0x00;
    DDRD = 0b11000011; //0b11000011, some input(PWM)
	
	PORTB = 0x00;
	DDRB = 0b11111110; //All output,expect last one

	PORTC = 0x00;
	DDRC = 0b11000011; //0b11110000, some input(PWM)
	
	enabled=1;
	ppm_input_init();
	ppm.ch[0]=0;
	//relay_cnt=100000;
	sei();
	
	int desired = 1;
/*	int relay_cnt;
	int pwm_cnt;*/
	
	int pwm_limit = PWM_MAX;
	const int pwm_lim_per_tick = PWM_MAX / PWM_DELAY;
	
	int relay_done = 0;
	int pwm_done = 0;
	
    while (1) 
    {
		int now = millis();
		
		if(ppm.ch[0]>0)  	
		{
			int rc_desired = (ppm.ch[0]>3043) ? 0 : 1;
			if (rc_desired != desired) {
				desired = rc_desired;
				if (desired)
					relay_done = now + RELAY_DELAY;
				else
					pwm_done = now + PWM_DELAY;
			}
			ppm.ch[0]=0;
		}
		
		if (desired) {
			PORTD&=~(1<<PORTD7); // turn off relay immediately
			if (now < relay_done)
				pwm_limit = 0;
			else if (now > relay_done + PWM_DELAY)
				pwm_limit = PWM_MAX;
			else
				pwm_limit = (now - relay_done) * pwm_lim_per_tick;
		}
		else {			
			pwm_limit = (now < pwm_done) ? (pwm_done - now) * pwm_lim_per_tick : 0;
			if (now > pwm_done)/
				PORTD|=(1<<PORTD7);
		}
		
		if (pwm_timer < pwm_limit) {
			PORTB=(PORTB&0b11000011)|(PIND&(0b00111100)); //После выключения реле передаём данные с порта.			
		}
		
		
		/*if (relay_cnt==0)
		{
			//Подождали пока ESC выключится, и включили торможение через реле.
			PORTD|=(1<<PORTD7);
			relay_cnt++;
		}
		
		if (enabled)
		{
			//Сначала реле выключается, потом включается ESC
			if (relay_cnt < 434000)
				relay_cnt++;
			else				
				PORTB=(PORTB&0b11000011)|(PIND&(0b00111100)); //После выключения реле передаём данные с порта.
		}
		else
		{
			//Сначала ESC выключается, потом реле включается.
			PORTB=(PORTB&0b11000011)|(PINC&(0b00111100));
			relay_cnt--;
		}*/		
    }
}

