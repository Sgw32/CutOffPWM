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
	
	/*OCR2 = 128;

	TCCR2 |= (1 << WGM21);
	// Set to CTC Mode

	TIMSK |= (1 << OCIE2);
	//Set interrupt on compare match

	TCCR2 |= (1 << CS21);
	// div8*/

	sei();
	
    while (1) 
    {
		TCCR1A = 0;
		TCNT1=0;
		TIFR = (1<<ICF1);  	/* Clear ICF (Input Capture flag) flag */
		
		TCCR1B = 0x41;  	/* Rising edge, no prescaler */
		while ((TIFR&(1<<ICF1)) == 0);
		a = ICR1;  		/* Take value of capture register */
		TIFR = (1<<ICF1);  	/* Clear ICF flag */
		
		TCCR1B = 0x01;  	/* Falling edge, no prescaler */
		while ((TIFR&(1<<ICF1)) == 0);
		b = ICR1;  		/* Take value of capture register */
		TIFR = (1<<ICF1);  	/* Clear ICF flag */
		
		TCCR1B = 0x41;  	/* Rising edge, no prescaler */
		while ((TIFR&(1<<ICF1)) == 0);
		c = ICR1;  		/* Take value of capture register */
		TIFR = (1<<ICF1);  	/* Clear ICF flag */
		
		
		if(a<b && b<c)  	/* Check for valid condition, 
					to avoid timer overflow reading */
		{
			PORTD|=(1<<PORTD7);
			high=b-a;
			period=c-a;
			long freq= F_CPU/period;/* Calculate frequency */

						/* Calculate duty cycle */
			float duty_cycle =((float) high /(float)period)*100;	
			float sec = (1.0f/(float)freq);
			sec*=duty_cycle; //Period=20ms, then 1-2 ms;
			if (sec>0.0015)
			{
				enabled=0;
				PORTD&=~(1<<PORTD7);
			}
			else
			{
				enabled=1;	
				//PORTD|=(1<<PORTD7);
			}
				
		}
		_delay_ms(200);
		PORTD&=~(1<<PORTD7);
		_delay_ms(200);
    }
}

