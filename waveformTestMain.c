#include "waveform.h"
#include <stdint.h>
#include <stdio.h>

void waveformMain() {
	waveform w;
	//int16_t sin1[] = { 1 };
	int16_t sin2[] = { 2, 18, 8, 2, 1 };
	//int16_t sin3[] = { 0 };

	/*
	buildWaveform(w, 0, 4096, sin1, 1);
	for(int i=0; i<SAMPLES; i++) {
		printf("%d ", w[i]);
	}
	printf("\n");
	*/

	buildWaveform(w, 0, 4096, sin2, 5);
	for(int i=0; i<SAMPLES; i++) {
		printf("%d ", w[i]);
	}
	printf("\n");

	/*
	buildWaveform(w, 0, 4096, sin3, 1);
	for(int i=0; i<SAMPLES; i++) {
		printf("%d ", w[i]);
	}
	printf("\n");
	*/
	//return 0;
}
