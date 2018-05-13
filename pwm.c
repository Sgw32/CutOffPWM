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

    TCCR1A = 0x00; // none
    TCCR1B = ( 1 << ICES1 ) | ( 1 << CS11) | ( 1 << CS10); // use rising edge as trigger, prescale_clk/8
    TIMSK = ( 1 << TICIE1 ); // allow input capture interrupts

    // Clear timer 5
    TCNT1H = 0x00;
    TCNT1L = 0x00;
}

// Interrupt service routine for reading PPM values from the radio receiver.
ISR( TIMER1_CAPT_vect )
{
    // Count duration of the high pulse
    uint16_t high_cnt; 
    high_cnt = (unsigned int)ICR1L;
    high_cnt += (unsigned int)ICR1H * 256;

    /* If the duration is greater than 5000 counts then this is the end of the PPM signal 
     * and the next signal being addressed will be Ch0
     */
    if ( high_cnt < 5000 )
    {
        // Added for security of the array
        if ( ch_index > 5 )
        {
            ch_index = 5;
        }

        ppm.ch[ch_index] = high_cnt; // Write channel value to array

        ch_index++; // increment channel index
    }
    else
    {
        ch_index = 0; // reset channel index
    }

    // Reset counter
    TCNT1H = 0;
    TCNT1L = 0;

    TIFR = ( 1 << ICF1 ); // clear input capture flag
}