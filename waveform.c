/* Embedded Systems Lab05
 *
 * Authors: Conor Lindahl 		- cl34376
 * 			Makeila Sorensen 	- ms62224
 *
 */

/*
 *  WAVEFORM
 *
 * 	GLOBALS:
 * 		Array that contains the information for a waveform 
 *
 * 	FUNCTIONS:
 * 		Defines a function that creates a waveform from a forrier series
 * 		Defines a function that creates a note from a waveform identifier
 * 			and a frequency
 */
#include "waveform.h"
#include <math.h>
#include <stdint.h>

#define PI 3.14159265359
#define abs16(x) x<0?-x:x;

/*
int16_t abs16(int16_t i) {
	return i < 0 ? -i : i;
}
*/


void buildWaveform(waveform w, int16_t low, int16_t high,
		int16_t *fs, int16_t n) {

	int16_t offset = (high - low)/2;
	int16_t summa = 0;

	/* Clear array */
	for(int j=0; j<SAMPLES; j++) {
		w[j] = 0;
	}

	/* Determine total amplitude of fourrier series */
	for(int i=0; i<n; i++) {
		int16_t coef = fs[i];
		summa += abs16(coef);
	}

	/* Iterate through each sin function, adding it's effects to the w */
	for(int i=0; i<n; i++) {

		for(int j=0; j<SAMPLES; j++) {
			int16_t a = (int16_t) offset*sin((i+1)*2*PI*j/SAMPLES); // i+1 to get correct overtone
			a += offset;
			/* Add overtone's effect to w including scaling */
			w[j] += (a/summa)*fs[i];
		}

	}
}

