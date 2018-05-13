// Libraries included
#include <avr/io.h>
#include <stdint.h>
#include "pwm.h"

/*      PWM INPUT
 *      ---
 *      ICP1    Pin48 on Arduino Mega
 */
void ppm_input_init(void)
{
    DDRB &= ( 0 << PINB0 ); // set ICP5 as an input

    TCCR1A = 0x00; // none
    TCCR1B = ( 1 << ICES5 ) | ( 1 << CS51); // use rising edge as trigger, prescale_clk/8
    TIMSK = ( 1 << ICIE5 ); // allow input capture interrupts

    // Clear timer 5
    TCNT5H = 0x00;
    TCNT5L = 0x00;
}

// Interrupt service routine for reading PPM values from the radio receiver.
ISR( TIMER5_CAPT_vect )
{
    // Count duration of the high pulse
    uint16_t high_cnt; 
    high_cnt = (unsigned int)ICR5L;
    high_cnt += (unsigned int)ICR5H * 256;

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
    TCNT5H = 0;
    TCNT5L = 0;

    TIFR5 = ( 1 << ICF5 ); // clear input capture flag
}