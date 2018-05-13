/*
 * CutOffPWM.c
 *
 * Created: 13.05.2018 18:21:56
 * Author : 1
 */ 
# define F_CPU 3686400UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "pwm.h"

unsigned int a,b,c,high,period;
int enabled;

ISR( TIMER2_COMP_vect)
{
	if (enabled)
		PORTB=(PORTB&0b11000011)|(PIND&(0b00111100));
}


int main(void)
{
    PORTD = 0x00;
    DDRD = 0b11000011; //0b11000011, some input(PWM)
	
	PORTB = 0x00;
	DDRB = 0b11111110; //All output,expect last one

	enabled=1;
	ppm_input_init();
	ppm.ch[0]=0;
	sei();
	
    while (1) 
    {
		
		if(ppm.ch[0]>0)  	
		{
			enabled = (ppm.ch[0]>700) ? 0 : 1;
			ppm.ch[0]=0;
		}
		
		if (enabled)
		{
			PORTB=(PORTB&0b11000011)|(PIND&(0b00111100));
			PORTD|=(1<<PORTD7);
		}
		else
		{
			PORTD&=~(1<<PORTD7);
			PORTB=PORTB&0b11000011;
			_delay_ms(19);
			PORTB=PORTB|0b00111100;
			_delay_us(960);
			PORTB=PORTB&0b11000011;
		}
    }
}

