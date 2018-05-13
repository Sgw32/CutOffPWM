#ifndef _PWM_H_
#define _PWM_H_

// Libraries included
#include <stdint.h>
#include <avr/interrupt.h>

struct orangeRX_pwm {
	uint16_t ch[6];
};
volatile unsigned char ch_index;
struct orangeRX_pwm ppm;

/* Functions */
void ppm_input_init(void); // Initialise the PPM Input to CTC mode
ISR( TIMER5_CAPT_vect ); // Use ISR to handle CTC interrupt and decode PPM

#endif /* _PWM_H_ */