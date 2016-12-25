#include "PLL.h"
#include "speaker.h"

void speakerTestMain() {
	PLL_Init(Bus80MHz);
	speakerInit();

	changeSoundFrequency(440);
	startSound();

	while(1) { }
}
