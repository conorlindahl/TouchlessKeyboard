#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

int16_t sensitivity[] = {0, 0, 0, 0, 0, 0, 0, 0};

/* Runs a sample to set up sensitivities for the keys based on what is
 * supposedly ambient lighting
 */
void setSensitivities(void) {
	ADC0_PSSI_R = 0x0001;
	for( int i=0; i<8; i++) { // Record the samples
		while((ADC0_SSFSTAT0_R&0x00000100)) {} // wait for value in FIFO

		sensitivity[i] = ADC0_SSFIFO0_R - 200;
	}
}

void keyboardInit(void){ 
  SYSCTL_RCGCADC_R |= 0x0001;   // 7) activate ADC0 
  /* Port E initialization */
  SYSCTL_RCGCGPIO_R |= 0x10;	// 1) activate clock for Port E
  while((SYSCTL_RCGCGPIO_R&0x10) == 0x0){};
  GPIO_PORTE_DIR_R &= ~0x3F;      // 2) make PE0-PE5 input
  GPIO_PORTE_AFSEL_R |= 0x3F;     // 3) enable alternate function on PE0-PE5
  GPIO_PORTE_DEN_R &= ~0x3F;      // 4) disable digital I/O on PE0-PE5
  GPIO_PORTE_AMSEL_R |= 0x3F;     // 5) enable analog functionality on PE0-PE5
    
  /* Port B initialization */
  SYSCTL_RCGCGPIO_R |= 0x02;	// 1) activate clock for Port B
  while((SYSCTL_RCGCGPIO_R&0x02) == 0x00){};
  GPIO_PORTB_DIR_R &= ~0x30;      // 2) make PB4-PB5 input
  GPIO_PORTB_AFSEL_R |= 0x30;     // 3) enable alternate function on PB4-PB5
  GPIO_PORTB_DEN_R &= ~0x30;      // 4) disable digital I/O on PB4-PB5
  GPIO_PORTB_AMSEL_R |= 0x30;     // 5) enable analog functionality on PB4-PB5
  

  /* ADC Initialization */
  ADC0_ACTSS_R &= ~0x0001;        // 9) disable sample sequencer 0
  ADC0_SSCTL0_R = 0x20000000;	  // Take 8 samples per sequence
  ADC0_PC_R &= ~0xF;              // 7) clear max sample rate field
  ADC0_PC_R |= 0x3;               //    configure for 125K samples/sec
  ADC0_SSMUX0_R = 0xBA890123;       // 11) Set sequencing order
  ADC0_ACTSS_R |= 0x0001;         // 14) enable sample sequencer 0

  setSensitivities();
}

/* Returns the current state of the keys on the keyboard. Each bit in the
 * least significant byte represents a different key. A 1 represents a pressed
 * key, a 0 represents a non-pressed key
 */
int getPressedKeys(void) {
  int pin = 0x01;
  int result = 0;
	int activeKeys[] = {0, 0, 0, 0, 0, 0, 0, 0};
	
	int samples = 0;
	while( samples < 10 ) {
		ADC0_PSSI_R = 0x0001;
		for( int i=0; i<8; i++) { // Record the samples
			while((ADC0_SSFSTAT0_R&0x00000100)) {} // wait for value in FIFO

			if( (ADC0_SSFIFO0_R&0xFFF) < sensitivity[i] ) {
				activeKeys[i] += 1;
			}
		}
		samples++;
	}
	
	for(int i=0; i<8; i++) {
		if(activeKeys[i] >= 10) {
			result |= pin;
		}
		pin<<=1;
	}

  return result;
}
