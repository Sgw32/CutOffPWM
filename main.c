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
	sei();
	
    while (1) 
    {
		
		if(ppm.ch[5]>0)  	
		{
			PORTD|=(1<<PORTD7);
			_delay_ms(10);
			if (ppm.ch[5]>700)
			{
				enabled=0;
				PORTD&=~(1<<PORTD7);
			}
			else
			{
				enabled=1;	
				//PORTD|=(1<<PORTD7);
			}
			ppm.ch[5]=0;
		}
		_delay_ms(190);
		PORTD&=~(1<<PORTD7);
		_delay_ms(200);
    }
}

