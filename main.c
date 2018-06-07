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
unsigned long relay_cnt;

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

	PORTC = 0x00;
	DDRC = 0b11000011; //0b11110000, some input(PWM)
	
	enabled=1;
	ppm_input_init();
	ppm.ch[0]=0;
	relay_cnt=3686400;
	sei();
	
    while (1) 
    {
		
		if(ppm.ch[0]>0)  	
		{
			enabled = (ppm.ch[0]>700) ? 0 : 1;
			ppm.ch[0]=0;
		}
		
		if (relay_cnt==1000) 
		{
			//После выключения реле передаём данные с порта.
			PORTB=(PORTB&0b11000011)|(PIND&(0b00111100));
			relay_cnt--;
		}
		if (relay_cnt==0)
		{
			//Подождали пока ESC выключится, и включили торможение через реле.
			PORTD|=(1<<PORTD7);
			relay_cnt++;
		}
		
		if (enabled)
		{
			//Сначала реле выключается, потом включается ESC
			PORTD&=~(1<<PORTD7);
			relay_cnt++;
		}
		else
		{
			//Сначала ESC выключается, потом реле включается.
			PORTB=(PORTB&0b11000011)|(PINC&(0b00111100));
			relay_cnt--;
		}
		
		
		
		
    }
}

