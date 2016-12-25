#include "../inc/tm4c123gh6pm.h"
#include "led.h"
#include "PLL.h"

#define led_PA0 0x00000001
#define led_PA1 0x00000002
#define led_PA2 0x00000004
#define led_PA3 0x00000008
#define led_PA4 0x00000010
#define led_PA5 0x00000020
#define led_PA6 0x00000040
#define led_PA7 0x00000080

int ledTestMain(void) {
	PLL_Init(Bus80MHz);
	ledInit();

	ledMask(led_PA2);

	while(1) { }
}
