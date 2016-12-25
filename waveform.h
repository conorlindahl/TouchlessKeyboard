/* Embedded System Lab05
 *
 * Authors: Conor Lindahl		cl34376
 * 			Makeila Soresen		ms62224
 *
 */

/* Header File for waveform.c
 *
 * Defintions:
 * SAMPLES - Number of samples that each sound period will get, equivalent to
 * 		the size of the waveform array
 * waveform - Typedef of an integer array of fixed size
 *
 * Declarations:
 * buildWaveform - turns a fourrier series into a waveform
 */
#include <stdint.h>

#define SAMPLES 396

typedef int16_t waveform[SAMPLES];

/* FUNCTION: buildWaveform
 *
 * Fills the waveform w with values in by taking SAMPLES discrete
 * values from the wave created from the fourrier series of fs.
 *
 * fs is an int array giving consecutive overtone values of sinusoids. 
 * 	Ex: [2, 0, 5, 4] corresponds to 2sin(x) + 0sin(2x) + 5sin(3x) + 4sin(4x)
 * n gives how many overtones are in fs
 *
 * After building the waveform, it will be scaled to [low, high)
 */
void buildWaveform(waveform w, int16_t high, int16_t low, 
		int16_t *fs, int16_t n);
