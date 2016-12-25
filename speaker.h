/* Header file for speaker module for final project
 */

/* Initializes a DAC port for controling the speaker interface */
void speakerInit(void);

/* Changes the frequency of the sound produced by the speaker. Must be a 
 * number greater than 0, or else this function will crash. 
 */
void changeSoundFrequency(int16_t);

/* Begins playing a sound from the speaker */
void startSound(void);

/* Stops playing a sound from the speaker */
void stopSound(void);

void speakerTestMain(void);
