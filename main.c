#include "../inc/tm4c123gh6pm.h"
#include <stdlib.h>
#include "PLL.h"
#include "led.h"
#include "speaker.h"
#include "keyboard.h"

chord keyboardMap[256]; // 8 keys -> 256 combinations
chord gameboardMap[256]; // Specifically for the game
//int16_t sharpMap[] = {2*Gb_4, 2*Ab_4, 2*Bb_4, 4*C_4, 4*Db_4, 4*Eb_4, 4*F_4, 0};
//int16_t noteMap[] = {2*F_4, 2*G_4, 2*A_4, 2*B_4, 4*C_4, 4*D_4, 4*E_4, 0};

int16_t sharpMap[] = { 4*F_4, 4*Eb_4, 4*Db_4, 4*C_4, 2*Bb_4, 2*Ab_4, 2*Gb_4, 0 };
int16_t noteMap[] = {4*E_4, 4*D_4, 4*C_4, 2*B_4, 2*A_4, 2*G_4, 2*F_4, 0 };

/* Initialized the key mapping in order to quickly set the frequency of the
 * speaker after getting the keyboard activation. Currently only maps one
 * note to each configuration. Will likely expand it to two keys later.
 * Maps the note of the highest activated key.
 */
void keyboardMappingInit(void) {
	int keymap=0;
	int notes[2];
	int numNotes;
	int keyMask;
	for(; keymap<256; keymap++) {
		notes[0] = 7;
		notes[1] = 7;
		numNotes = 0;
		keyMask = 0x00000040;
		
		for(int j=0; j<7; j++) {
			if(keymap&keyMask) { // This bit is active
				notes[numNotes] = j;
				numNotes+=1;
			}
			if(numNotes==2) { // Max chords
				break;
			}
			keyMask>>=1; // Shift mask to check next bits
		}
		
		/* Regular keyboard mappings */
		if(keymap&0x00000080) {
			keyboardMap[keymap].freq1 = sharpMap[notes[0]];
			keyboardMap[keymap].freq2 = sharpMap[notes[1]];
		} else {
			keyboardMap[keymap].freq1 = noteMap[notes[0]];
			keyboardMap[keymap].freq2 = noteMap[notes[1]];
		}

		/* Game mode mappings */
		if(keymap & 0x01) {
			gameboardMap[keymap] = (chord){2*F_4, 0};
		} else if ( keymap & 0x02 ) {
			gameboardMap[keymap] = (chord){2*G_4, 0};
		} else if ( keymap & 0x04 ) {
			gameboardMap[keymap] = (chord){2*A_4, 0};
		} else if ( keymap & 0x08 ) {
			gameboardMap[keymap] = (chord){2*B_4, 0};
		} else if ( keymap & 0x10 ) {
			gameboardMap[keymap] = (chord){4*C_4, 0};
		} else if ( keymap & 0x20 ) {
			gameboardMap[keymap] = (chord){4*D_4, 0};
		} else if ( keymap & 0x40 ) {
			gameboardMap[keymap] = (chord){4*E_4, 0};
		} else if ( keymap & 0x80 ) {
			gameboardMap[keymap] = (chord){4*F_4, 0};
		} else {
			/* No key pressed */
			gameboardMap[keymap] = (chord){0, 0};
		}
	}
}

void randInit(void) {
	int32_t sum=0;
	ADC0_PSSI_R = 0x0001;
	for( int i=0; i<8; i++) { // Record the samples
		while((ADC0_SSFSTAT0_R&0x00000100)) {} // wait for value in FIFO

		sum += ADC0_SSFIFO0_R;
	}

	srand(sum);
}

/* TIMER1A will be a one-shot countdown timer for general purpose
*/
void gameInit(void) {
	/* Initialize Timer1A to control computer turn */
	SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1 TODO: make sure this right
	TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
	TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
	TIMER1_TAMR_R = 0x00000001;   // 3) configure for one-shot mode, default down-count settings
	TIMER1_TAPR_R = 0;            // 5) bus clock resolution
	TIMER1_ICR_R = 0x00000001;    // 6) clear TIMER1A timeout flag
	//TIMER1_IMR_R = 0x00000001;    // 7) arm timeout interrupt

	SYSCTL_RCGCTIMER_R |= 0x08;   // 0) activate TIMER3 TODO: make sure this right
	TIMER3_CTL_R = 0x00000000;    // 1) disable TIMER3A during setup
	TIMER3_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
	TIMER3_TAMR_R = 0x00000001;   // 3) configure for one-shot mode, default down-count settings
	TIMER3_TAPR_R = 0;            // 5) bus clock resolution
	TIMER3_ICR_R = 0x00000001;    // 6) clear TIMER3A timeout flag

	/* TODO: Correct interrupts 
	   NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x20000000; // 8) priority 1
	// interrupts enabled in the main program after all devices initialized
	// vector number 35, interrupt number 19
	NVIC_EN0_R = 1<<19;           // 9) enable IRQ 19 in NVIC
	*/

	randInit();
}

void switchInit(void) {
	SYSCTL_RCGCGPIO_R |= 0x02;	// 1) activate clock for Port B
	while((SYSCTL_RCGCGPIO_R&0x02) == 0x00){};
	GPIO_PORTB_PCTL_R &= ~0x0000000F; // Enable GPIO for PB0
	GPIO_PORTB_DIR_R &= ~0x01;      // 2) make PB0 input
	GPIO_PORTB_DEN_R |= 0x01;      // 4) enable digital I/O on PB0
	GPIO_PORTB_AFSEL_R &= ~0x01;     // 3) disable alt function on PB0
	GPIO_PORTB_AMSEL_R &= ~0x01;     // 5) disable analog functionality on PB0

	// Pull up resistor?
}

/* Returns the status of the game switch on the board 
 *   0 for off, 1 for on
 */
int switchPressed(void) {
	return GPIO_PORTB_DATA_R&0x01;
}

#define MAX_SEQ 256
static int32_t sequence[MAX_SEQ];

/* block the calling program until Timer1 counts down d cycles */
void dutyWait(int32_t d) {
	TIMER1_TAILR_R = d;
	TIMER1_ICR_R = 0x00000001; // Clear timeout flag
	TIMER1_CTL_R = 0x00000001;    // begin Timer1 countdown

	/* Wait for the countdown to reach 0 */
	while( !(TIMER1_RIS_R & 0x00000001) ) { }
}

/* Begin Timer1 countdown w/ clockcycles d */
void dutyRun(int32_t d) {
	TIMER1_TAILR_R = d;
	TIMER1_ICR_R = 0x00000001; // Clear timeout flag
	TIMER1_CTL_R = 0x00000001;    // begin Timer1 countdown
}

/* Returns 1 on completion, 0 on incomplete */
int checkTimer1() {
	return TIMER1_RIS_R & 0x00000001;
}

void timer3Run(int32_t d) {
	TIMER3_TAILR_R = d;
	TIMER3_ICR_R = 0x00000001; // Clear timeout flag
	TIMER3_CTL_R = 0x00000001;    // begin Timer1 countdown
}

int checkTimer3() {
	return TIMER3_RIS_R & 0x00000001;
}

/* Flashes LEDs w/ 50 percent duty cycle at period 2*p */
void tripleFlash(int32_t p) {
	for( int i=0; i<3; i++) {
		/* Flash Lights 3 times */
		ledMask(255); // All lights

		/* Wait for 25% of second */
		dutyWait(p); 

		/* Turn off LED & sound for note */
		ledMask(0);

		/* Wait for 25% of second */
		dutyWait(p); /* Wait for 25% of period */
	}
}

/* WAH WAH WAH WAAAAAAAAAH + 1/2 second wait */
void loseSound(void) {
	changeSoundFrequency((chord){2*F_4,0}); // F_5
	startSound();
	dutyWait(56000000);
	stopSound();
	dutyWait(24000000);
	
	changeSoundFrequency((chord){2*E_4,0}); // E_5
	startSound();
	dutyWait(56000000);
	stopSound();
	dutyWait(24000000);
	
	changeSoundFrequency((chord){2*Eb_4,0}); // Eb_5
	startSound();
	dutyWait(56000000);
	stopSound();
	dutyWait(24000000);
	
	
	changeSoundFrequency((chord){2*D_4,0}); // D_5
	startSound();
	dutyWait(160000000);
	stopSound();
	dutyWait(40000000);
}

/* Close encounters of the 3rd kind theme */
void startGameSound(void) {
	ledMask(0); // Turn off current lights
	changeSoundFrequency((chord){2*G_4,0}); // G_5
	startSound();
	ledMask(0x20);
	dutyWait(40000000);
	
	changeSoundFrequency((chord){2*A_4,0}); // A_5
	startSound();
	ledMask(0x40);
	dutyWait(40000000);
	
	changeSoundFrequency((chord){2*F_4,0}); // F_5
	startSound();
	ledMask(0x08);
	dutyWait(40000000);
	
	changeSoundFrequency((chord){F_4,0}); // F_4
	startSound();
	ledMask(0x01);
	dutyWait(40000000);
	
	changeSoundFrequency((chord){2*C_4,0}); // C_5
	startSound();
	ledMask(0x04);
	dutyWait(80000000);
	stopSound();
}

int playGame(void) {
	int32_t compPeriod;
	int32_t seqLength;
	int32_t duty;
	int i; // index for for's & while's

	/* Indicate that the Game is starting */
	startGameSound();
	tripleFlash(20000000);

	/* Initialize the Game */
	seqLength = 0; // Restart the sequence 
	compPeriod = 80000000; // Game begins at 1 Hz
	duty = compPeriod - compPeriod/4; // 75% duty cycle

	do {
		int key = rand() % 8; // Press switch 0-7
		sequence[seqLength] = 0x01 << (key);
		seqLength++;

		/* Start the computer turn */
		for( i = 0; i < seqLength; i++ ) {
			/* Show & turn on sound for note */
			ledMask(sequence[i]);
			changeSoundFrequency(gameboardMap[sequence[i]]);
			startSound();

			/* Wait for 75% of period */
			dutyWait(duty);

			/* Turn off LED & sound for note */
			ledMask(0);
			stopSound();

			/* Wait for 25% of period */
			dutyWait(compPeriod - duty); /* Wait for 25% of period */
		}

		/* Show that it is the human's turn */
		ledMask(255); // All lights
		dutyWait(20000000); // Wait for 25% of second 
		ledMask(0); // Turn off LED
		dutyWait(20000000); // Wait for 25% of period
		
		/* Start the human turn */
		i = 0;
		while( i < seqLength ) {
			int32_t key = sequence[i];
			int32_t guess = 0;
			int32_t guessLock = 1;

			dutyRun(240000000); // User has 3 seconds to lock in answer

			/* Wait for User input */
			while((!checkTimer1()) && guessLock) { // until timeout or guess
				guess = getPressedKeys();
				ledMask(guess);
				
				if( guess != 0 ) {
				/* Start lock timer */
					timer3Run(30000000); // 3/8 second 
					while( guess == getPressedKeys() ) { // Make sure user doesn't change mind
						if ( checkTimer3() ) { // If the timer finishes, lock in the guess
							guessLock = 0;
							break;
						}
					}
				}
			}

			/* Check that input is correct */
			if ( guess == key ) {
				/* display & play user guess note */
				ledMask(guess);
				changeSoundFrequency(gameboardMap[guess]);
				startSound();
				
				/* Wait for 1/2 of a second */
				dutyWait(40000000);

				/* Turn off LED & sound for note */
				ledMask(0);
				stopSound();

				/* Wait for 1/8 of a second */
				dutyWait(10000000);
			} else {
				break; // User lost, turn is over
			}
			
			dutyWait(10000000); // Slight delay before next turn
			i++;
		}

		/* Check if user guessed all the right keys */
		if ( i != seqLength ) { 
			/* Play a bad sound Show the end of the game */
			loseSound();

			/* Indicate that the Game is over */
			tripleFlash(20000000);

			return 0;
		} else {
			/* They survived another round!!! */
			changeSoundFrequency((chord){C_5, 0});
			startSound();
			dutyWait(8000000);
			changeSoundFrequency((chord){2*E_4, 0}); // E_5
			dutyWait(8000000);
			stopSound();
			dutyWait(20000000);
		}

		compPeriod -= 9*(compPeriod/100); // Sub 9% from period = increase freq by 10%
		duty = compPeriod - compPeriod/4; // 75% duty cycle for computer
	} while ( seqLength < MAX_SEQ );

	/* HOLY SHIT THEY WON */
	return 0;
}

int main(void) {
	PLL_Init(Bus80MHz);
	keyboardMappingInit();
	ledInit();
	keyboardInit();
	switchInit();

	speakerInit();
	
	gameInit();

	int prevKeyboard = 0;
	chord prevChord = (chord){0,0};
	while(1) {
		if(switchPressed()) {
			playGame();
		}

		int curKeyboard = getPressedKeys();
		chord curChord = keyboardMap[curKeyboard];
		if( curKeyboard != prevKeyboard ) {
			ledMask(curKeyboard); // Always have led reflect keys

			/* Set speaker according to key press */
			if( (curChord.freq1 != prevChord.freq1) || (curChord.freq2 != prevChord.freq2) ) {
				if ( curKeyboard > 0 ) {
					changeSoundFrequency(curChord);
					startSound();
				} else {
					/* curKeyboard == 0 */
					stopSound();
				}
				prevChord = curChord;
			}
			
			prevKeyboard = curKeyboard;
			dutyWait(800000); // Poll 100 times a second
		}
	}
}
