#ifndef _PPM_H_
#define _PPM_H_

// Libraries included
#include <stdint.h>
#include <avr/interrupt.h>

struct orangeRX_ppm {
	uint16_t ch[6];
};
volatile unsigned char ch_index;
struct orangeRX_ppm ppm;

/* Functions */
void ppm_input_init(void); // Initialise the PPM Input to CTC mode
ISR( TIMER1_CAPT_vect ); // Use ISR to handle CTC interrupt and decode PPM

#endif /* _PPM_H_ */