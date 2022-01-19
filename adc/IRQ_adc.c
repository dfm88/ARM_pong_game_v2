/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_adc.c
** Last modified Date:  20184-12-30
** Last Version:        V1.00
** Descriptions:        functions to manage A/D interrupts
** Correlated files:    adc.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/

#include "lpc17xx.h"
#include "adc.h"
#include "../GLCD/GLCD.h"
#include "../game/game.h"

/*----------------------------------------------------------------------------
  A/D IRQ: Executed when A/D Conversion is ready (signal from ADC peripheral)
 *----------------------------------------------------------------------------*/

unsigned short AD_current;
unsigned short AD_last = 0xFF; /* Last converted value               */

void ADC_IRQHandler(void)
{
  unsigned short mov = 0;
  AD_current = ((LPC_ADC->ADGDR >> 4) & 0xFFF); /* Read Conversion Result             */
  mov = AD_current * 8 / 0xFFF;
  move_paddle(mov, &paddle1, 0);
}
