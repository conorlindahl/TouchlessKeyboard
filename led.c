#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

void ledInit(void) {
	//Port A
	SYSCTL_RCGCGPIO_R |= 0x00000001; //Activate clock
	while((SYSCTL_RCGCGPIO_R & 0x00000001) == 0) {}; //Delay
	// Init all pins
	GPIO_PORTA_AFSEL_R &= ~0x000000FF; //Enable regular
	GPIO_PORTA_AMSEL_R &= ~0x000000FF; //Disable analog
	GPIO_PORTA_PCTL_R &= ~0xFFFFFFFF; //Enable GPIO
	GPIO_PORTA_DEN_R |= 0x000000FF; //Enable digital
	GPIO_PORTA_DIR_R |= 0x000000FF; // Output
}

/* Changes the current LED set to the one specified by mask, where each bit in
 * the least significant byte represents the value to be put on the bits
 */
void ledMask(uint32_t mask) {
	GPIO_PORTA_DATA_R = mask;
}
