/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           lib_timer.h
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        atomic functions to be used by higher sw levels
** Correlated files:    lib_timer.c, funct_timer.c, IRQ_timer.c
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "timer.h"

/******************************************************************************
** Function name:		enable_timer
**
** Descriptions:		Enable timer
**
** parameters:			timer number: 0 or 1 or 2
** Returned value:		None
**
******************************************************************************/
void enable_timer(uint8_t timer_num)
{
	if (timer_num == 0)
	{
		LPC_TIM0->TCR = 1;
	}
	else if (timer_num == 1)
	{
		LPC_TIM1->TCR = 1;
	}
	else if (timer_num == 2)
	{
		LPC_TIM2->TCR = 1;
	}
	return;
}

/******************************************************************************
** Function name:		disable_timer
**
** Descriptions:		Disable timer
**
** parameters:			timer number: 0 or 1 or 2
** Returned value:		None
**
******************************************************************************/
void disable_timer(uint8_t timer_num)
{
	if (timer_num == 0)
	{
		LPC_TIM0->TCR = 0;
	}
	else if (timer_num == 1)
	{
		LPC_TIM1->TCR = 0;
	}
	else if (timer_num == 2)
	{
		LPC_TIM2->TCR = 0;
	}
	return;
}

/******************************************************************************
** Function name:		reset_timer
**
** Descriptions:		Reset timer
**
** parameters:			timer number: 0 or 1 or 2
** Returned value:		None
**
******************************************************************************/
void reset_timer(uint8_t timer_num)
{
	uint32_t regVal;

	if (timer_num == 0)
	{
		regVal = LPC_TIM0->TCR;
		regVal |= 0x02;
		LPC_TIM0->TCR = regVal;
	}
	else if (timer_num == 1)
	{
		regVal = LPC_TIM1->TCR;
		regVal |= 0x02;
		LPC_TIM1->TCR = regVal;
	}
	else if (timer_num == 2)
	{
		regVal = LPC_TIM2->TCR;
		regVal |= 0x02;
		LPC_TIM2->TCR = regVal;
	}
	return;
}

// XXX funzione estesa
uint32_t init_timer(uint8_t timer_num, uint32_t Prescalar, uint8_t MatchReg, uint8_t SRImatchReg, uint32_t TimerInterval)
{
	if (timer_num == 0)
	{
		// XXX assegno al Timer0 il valore del Prescaler
		LPC_TIM0->PR = Prescalar;

		// XXX se viene passato il MatchRegister 0 assegno al suo registro
		// l'intervallo di tempo passato dall'utente
		if (MatchReg == 0)
		{
			LPC_TIM0->MR0 = TimerInterval;
			// devo shiftare il settaggio del Stop-Reset-Interrupt a
			// seconda del MR che sto considerando (vedi slide Esercizio1)
			LPC_TIM0->MCR |= SRImatchReg << 3 * MatchReg;

		} // e cos� via a seconda del valore del Match Register
		else if (MatchReg == 1)
		{
			LPC_TIM0->MR1 = TimerInterval;
			LPC_TIM0->MCR |= SRImatchReg << 3 * MatchReg;
		}
		else if (MatchReg == 2)
		{
			LPC_TIM0->MR2 = TimerInterval;
			LPC_TIM0->MCR |= SRImatchReg << 3 * MatchReg;
		}
		else if (MatchReg == 3)
		{
			LPC_TIM0->MR3 = TimerInterval;
			LPC_TIM0->MCR |= SRImatchReg << 3 * MatchReg;
		}
		// XXX abilito l'interruzione del timer in questione
		//  in questo caso lo 0 e ne setto la priorit�
		// LPC_TIM0->PR = 20;
		NVIC_EnableIRQ(TIMER0_IRQn);
		// NVIC_SetPriority(TIMER0_IRQn, 4);		/* less priority than buttons */
		NVIC_SetPriority(TIMER0_IRQn, 1); /* more priority than buttons */
		return (1);
	}
	// XXX ripetiamo quanto sopra abche per il timer 1
	else if (timer_num == 1)
	{
		LPC_TIM1->PR = Prescalar;

		if (MatchReg == 0)
		{
			LPC_TIM1->MR0 = TimerInterval;
			LPC_TIM1->MCR |= SRImatchReg << 3 * MatchReg;
		}
		else if (MatchReg == 1)
		{
			LPC_TIM1->MR1 = TimerInterval;
			LPC_TIM1->MCR |= SRImatchReg << 3 * MatchReg;
		}
		else if (MatchReg == 2)
		{
			LPC_TIM1->MR2 = TimerInterval;
			LPC_TIM1->MCR |= SRImatchReg << 3 * MatchReg;
		}
		else if (MatchReg == 3)
		{
			LPC_TIM1->MR3 = TimerInterval;
			LPC_TIM1->MCR |= SRImatchReg << 3 * MatchReg;
		}

		NVIC_EnableIRQ(TIMER1_IRQn);
		NVIC_SetPriority(TIMER1_IRQn, 1); /* less priority than buttons and timer0*/
		return (1);
	}
	else if (timer_num == 2)
	{
		LPC_SC->PCONP |= (1 << 22); // XXX accendo il timer 2

		// XXX assegno al Timer2 il valore del Prescaler
		LPC_TIM2->PR = Prescalar;

		// XXX se viene passato il MatchRegister 0 assegno al suo registro
		// l'intervallo di tempo passato dall'utente
		if (MatchReg == 0)
		{
			LPC_TIM2->MR0 = TimerInterval;
			// devo shiftare il settaggio del Stop-Reset-Interrupt a
			// seconda del MR che sto considerando
			LPC_TIM2->MCR |= SRImatchReg << 3 * MatchReg;
		} // e cos� via a seconda del valore del Match Register
		else if (MatchReg == 1)
		{
			LPC_TIM2->MR1 = TimerInterval;
			LPC_TIM2->MCR |= SRImatchReg << 3 * MatchReg;
		}
		// XXX abilito l'interruzione del timer in questione
		//  in questo caso il 2 e ne setto la priorit�
		// LPC_TIM0->PR = 20;
		NVIC_EnableIRQ(TIMER2_IRQn);
		// NVIC_SetPriority(TIMER0_IRQn, 4);		/* less priority than buttons */
		NVIC_SetPriority(TIMER2_IRQn, 0); /* more priority than buttons */
		return (1);
	}
	return (0);
}

/******************************************************************************
**                            End Of File
******************************************************************************/
