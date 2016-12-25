/* Header file for LED module for Final Project
 *
 * Author: Conor Lindahl & Makeila Sorensen
 */
#include <stdint.h>

void ledInit(void);

void ledMask(uint32_t mask);

int ledTestMain(void);
