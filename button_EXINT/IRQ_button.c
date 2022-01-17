#include "button.h"
#include "lpc17xx.h"

extern int key1_down;
extern int key2_down;
extern int key0_down;
extern int keyReset_down;

/**
	RESET THE GAME
**/
void EINT0_IRQHandler(void) /* INT0														 */
{
	NVIC_DisableIRQ(EINT0_IRQn);	   /* disable Button interrupts			 */
	LPC_PINCON->PINSEL4 &= ~(1 << 20); /* GPIO pin selection */
	key0_down = 1;
	LPC_SC->EXTINT &= (1 << 0); /* clear pending interrupt         */
}

/**
	START THE GAME
**/
void EINT1_IRQHandler(void) /* KEY1														 */
{

	NVIC_DisableIRQ(EINT1_IRQn);	   /* disable Button interrupts			 */
	LPC_PINCON->PINSEL4 &= ~(1 << 22); /* GPIO pin selection */
	key1_down = 1;
	LPC_SC->EXTINT &= (1 << 1); /* clear pending interrupt         */
}

/**
	PAUSE-RESUME THE GAME
**/
void EINT2_IRQHandler(void) /* KEY2														 */
{
	NVIC_DisableIRQ(EINT2_IRQn);	   /* disable Button interrupts			 */
	LPC_PINCON->PINSEL4 &= ~(1 << 24); /* GPIO pin selection */
	key2_down = 1;
	LPC_SC->EXTINT &= (1 << 2); /* clear pending interrupt         */
}
