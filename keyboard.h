/* Header file for Keyboard module of Final Project */

#define VOLUME 1536

#define C_4 262
#define Db_4 277
#define D_4 294
#define Eb_4 311
#define E_4 330
#define F_4 349
#define Gb_4 370
#define G_4 392
#define Ab_4 415
#define A_4 440
#define Bb_4 466
#define B_4 494
#define C_5 523
#define NONE 0

/* Initialized the keyboard to read from ADC pins */
void keyboardInit(void);

/* Returns an int where each set bit represents a corresponding key that is
 * pressed. I.e. 00010010 means the 4th and 7th keys are pressed. 
 */
int getPressedKeys(void);

int keyboardTestMain(void);
