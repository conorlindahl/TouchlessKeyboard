/* Embedded Systems Lab05
 *
 * Authors: Conor Lindahl		-cl34376	
 * 			Makeila Sorensen	-ms62224
 *
 */

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "speaker.h"
#include "waveform.h"

#define VOLUME 3000

static waveform wave;
static int playingChord; // 0 for no, 1 for yes
static int16_t fs[] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Fourier series of waveform 

/*Initialize the DAC
*/
void DAC_Init(void) {
	SYSCTL_RCGCSSI_R |= 0x8; //Activate SSI Module 3
	SYSCTL_RCGCGPIO_R  |= 0x08; //Activate GPIO Port D
	while((SYSCTL_RCGCGPIO_R & 0x08) == 0) {}; //Wait until GPIO Port D is activated
	GPIO_PORTD_AMSEL_R &= ~0x0F; //Disable analog
	GPIO_PORTD_AFSEL_R |= 0x0F; //Enable alternate for PD0, PD1, PD2, and PD3
	GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R & 0xFFFF0000) + 0x00001111; //Select SSI Module 3
	GPIO_PORTD_DEN_R |= 0x0F; //Enable digital
	SSI3_CR1_R &= ~0x02; //Disable SSI Module 3
	SSI3_CR1_R &= ~0x04; //Master operation
	SSI3_CPSR_R = 0x04; //20 MHz SSI clock
	SSI3_CR0_R &= ~0xFF00; //Clock Rate = 0 for max bit rate
	SSI3_CR0_R &= ~0x30; //Freescale SPI FF
	SSI3_CR0_R &= ~0x40; //Clock Polarity = 0
	SSI3_CR0_R |= 0x80; //Clock Phase = 1
	SSI3_CR0_R |= 0xF; //16-bit data size (4 control bits; 12 output bits)
	SSI3_CR1_R |= 0x02; //Enable SSI Module 3
}

void soundInit(void) {
  /* Initialize Timer0A to control sound */
  SYSCTL_RCGCTIMER_R |= 0x01;   // 0) activate TIMER0
  TIMER0_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  TIMER0_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER0_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER0_TAPR_R = 0;            // 5) bus clock resolution
  TIMER0_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  TIMER0_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x20000000; // 8) priority 1
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN0_R = 1<<19;           // 9) enable IRQ 19 in NVIC
	
	SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate TIMER2
  TIMER2_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
	
	/* TODO: This */
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x20000000; // 8) priority 1
// interrupts enabled in the main program after all devices initialized
// vector number 40, interrupt number 24
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 24 in NVIC
	
	
  /* Leave Timer off until music is supposed to start */
  buildWaveform(wave, 0, VOLUME, fs, 10);
}

void speakerInit(void) {
	DAC_Init();
	soundInit();
}

/*Output data to the DAC
*/
void DAC_Out(int16_t data) {
	while((SSI3_SR_R & 0x02) == 0) {} //Wait until SSI Module 3 Transmit FIFO is not full
	SSI3_DR_R = data; //Output data
}

static int16_t timer0Index;
static int16_t timer2Index;
static int16_t vOut;

/* Changes the sound frequency to freq */
void changeSoundFrequency(chord notes) {
  uint32_t timer0PrevState, timer2PrevState;
  uint32_t timer0period, timer2period;
  uint32_t timer0isrPeriod, timer2isrPeriod;

  /* Disable TIMER0 */
  timer0PrevState = TIMER0_CTL_R & 0x00000001; // Saving previous state
	timer2PrevState = TIMER2_CTL_R & 0x00000001; 
  TIMER0_CTL_R = 0x00000000;
	TIMER2_CTL_R = 0x00000000;

  /* Calculate new period */
	if (notes.freq1 != 0 ) { /* Valid frequency */
  timer0isrPeriod = notes.freq1 * SAMPLES;
  timer0period = (80000000) / timer0isrPeriod;
  TIMER0_TAILR_R = timer0period-1;
	} else {
		TIMER0_TAILR_R = 80000000; /* Slow interupt */
	}
	
	/* Timer2 slightly different */
	if (notes.freq2 != 0) { /* Valid frequency */
		playingChord = 1;
		timer2isrPeriod = notes.freq2 * SAMPLES;
		timer2period = (80000000) / timer2isrPeriod;
		TIMER2_TAILR_R = timer2period-1;
	} else {
		playingChord = 0;
		TIMER2_TAILR_R = 80000000; // Slowest interupt I can think of right now
	}

	/* Initialize wave */
	timer0Index = timer2Index = 0;
	if( playingChord ) {
		vOut = wave[timer0Index]/2 + wave[timer2Index]/2;
	} else {
		vOut = wave[timer0Index];
	}
	
  /* Reenable TIMERs */
  TIMER0_CTL_R = timer0PrevState;
	TIMER2_CTL_R = timer2PrevState;
}

/* TODO: Redesign this */
void Timer0A_Handler(void) {
  TIMER0_ICR_R = 0x00000001;    //clear TIMER0A timeout flag
	
	if( playingChord ) {
		vOut -= wave[timer0Index]/2;
		vOut += wave[(timer0Index+1)%SAMPLES]/2;
	} else {
		// Only one note playing, so full weight
		vOut = wave[timer0Index];
	}
	DAC_Out(vOut);
  timer0Index = (timer0Index+1)%SAMPLES; /* TODO: See if an if statement is faster */
}

void Timer2A_Handler(void) {
	TIMER2_ICR_R = 0x00000001; // clear Timer2 interupt flag
	
	if( playingChord ) {
		/* Only ever happens w/ 2 notes, so half amplitude */
		vOut -= wave[timer2Index]/2;
		vOut += wave[(timer2Index+1)%SAMPLES]/2;
		DAC_Out(vOut);
		timer2Index = (timer2Index+1)%SAMPLES;
	}
}

void startSound(void) {
  TIMER0_CTL_R = 0x00000001;    // enable TIMER0A
	TIMER2_CTL_R = 0x00000001;
}


void stopSound(void) {
  TIMER0_CTL_R = 0x00000000; // Disable TIMER0A
	TIMER2_CTL_R = 0x00000000; 
}
