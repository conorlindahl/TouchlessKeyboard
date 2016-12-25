#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>
#include "keyboard.h"
#include "PLL.h"

int keyboardTestMain(void) {
	PLL_Init(Bus80MHz);
	keyboardInit();

	uint32_t val;
	while(1) {
		val = getPressedKeys();
		int i;
		for(i=0; i<80000000; i++) {
		}
		val = val;
	}
}
