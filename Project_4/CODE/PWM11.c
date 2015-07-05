/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : PWM11.c
**     Project   : Project_4
**     Processor : MC9S12C32CFU16
**     Component : PWM
**     Version   : Component 02.231, Driver 01.16, CPU db: 2.87.410
**     Compiler  : CodeWarrior HC12 C Compiler
**     Date/Time : 7/5/2015, 6:31 PM
**     Abstract  :
**         This component implements a pulse-width modulation generator
**         that generates signal with variable duty and fixed cycle. 
**     Settings  :
**         Used output pin             : 
**             ----------------------------------------------------
**                Number (on package)  |    Name
**             ----------------------------------------------------
**                       79            |  PP5_PWM5_KWP5
**             ----------------------------------------------------
**
**         Timer name                  : PWM5 [8-bit]
**         Counter                     : PWMCNT5   [$00F1]
**         Mode register               : PWMCTL    [$00E5]
**         Run register                : PWME      [$00E0]
**         Prescaler                   : PWMPRCLK  [$00E3]
**         Compare 1 register          : PWMPER5   [$00F7]
**         Compare 2 register          : PWMDTY5   [$00FD]
**         Flip-flop 1 register        : PWMPOL    [$00E1]
**
**         User handling procedure     : not specified
**
**         Output pin
**
**         Port name                   : P
**         Bit number (in port)        : 5
**         Bit mask of the port        : $0020
**         Port data register          : PTP       [$0258]
**         Port control register       : DDRP      [$025A]
**
**         Runtime setting period      : none
**         Runtime setting ratio       : calculated
**         Initialization:
**              Aligned                : Left
**              Output level           : low
**              Timer                  : Enabled
**              Event                  : Enabled
**         High speed mode
**             Prescaler               : divide-by-1
**             Clock                   : 53333 Hz
**           Initial value of            period        pulse width (ratio 0.392%)
**             Xtal ticks              : 57375         225
**             microseconds            : 4781          19
**             milliseconds            : 5             0
**             seconds (real)          : 0.00478125    0.00001875
**
**     Contents  :
**         SetRatio16 - byte PWM11_SetRatio16(word Ratio);
**         SetDutyUS  - byte PWM11_SetDutyUS(word Time);
**         SetDutyMS  - byte PWM11_SetDutyMS(word Time);
**
**     Copyright : 1997 - 2010 Freescale Semiconductor, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/


/* MODULE PWM11. */

#include "PWM11.h"

#pragma DATA_SEG PWM11_DATA            /* Select data segment "PWM11_DATA" */

#pragma CODE_SEG PWM11_CODE

static word RatioStore;                /* Ratio of L-level to H-level */


/*
** ===================================================================
**     Method      :  SetRatio (component PWM)
**
**     Description :
**         The method reconfigures the compare and modulo registers of 
**         the peripheral(s) when the speed mode changes. The method is 
**         called automatically as a part of the component 
**         SetHigh/SetLow/SetSlow methods.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
#pragma MESSAGE DISABLE C12056         /* Disable WARNING C12056: SP debug info incorrect because of optimization */
static void SetRatio(void)
{
  PWMDTY5 = (byte)(((0xFFUL * (dword)RatioStore) + 0x8000UL) >> 0x10U); /* Calculate new value according to the given ratio */
}
#pragma MESSAGE DEFAULT C12056         /* Re-enable WARNING C12056 */

/*
** ===================================================================
**     Method      :  PWM11_SetRatio16 (component PWM)
**
**     Description :
**         This method sets a new duty-cycle ratio. Ratio is expressed
**         as a 16-bit unsigned integer number. Value 0 - 65535 value
**         is proportional to ratio 0 - 100%. 
**         Note: Calculated duty depends on the timer possibilities and
**         on the selected period.
**         The method is available only if method <SetPeriodMode> is
**         not selected.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Ratio           - Ratio to set. 0 - 65535 value is
**                           proportional to ratio 0 - 100%
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/
byte PWM11_SetRatio16(word Ratio)
{
  RatioStore = Ratio;                  /* Store new value of the ratio */
  SetRatio();                          /* Calculate and set up new appropriate values of the duty and period registers */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  PWM11_SetDutyUS (component PWM)
**
**     Description :
**         This method sets the new duty value of the output signal. The
**         duty is expressed in microseconds as a 16-bit unsigned integer
**         number.
**     Parameters  :
**         NAME       - DESCRIPTION
**         Time       - Duty to set [in microseconds]
**                      (0 to 4781 us in high speed mode)
**     Returns     :
**         ---        - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_MATH - Overflow during evaluation
**                           ERR_RANGE - Parameter out of range
** ===================================================================
*/
byte PWM11_SetDutyUS(word Time)
{
  dlong rtval;                         /* Result of two 32-bit numbers multiplication */

  if (Time > 0x12ADU) {                /* Is the given value out of range? */
    return ERR_RANGE;                  /* If yes then error */
  }
  PE_Timer_LngMul((dword)Time, (dword)229963216, &rtval); /* Multiply given value and high speed CPU mode coefficient */
  if (PE_Timer_LngHi3(rtval[0], rtval[1], &RatioStore)) { /* Is the result greater or equal than 65536 ? */
    RatioStore = 0xFFFFU;              /* If yes then use maximal possible value */
  }
  SetRatio();                          /* Calculate and set up new appropriate values of the duty and period registers */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  PWM11_SetDutyMS (component PWM)
**
**     Description :
**         This method sets the new duty value of the output signal. The
**         duty is expressed in milliseconds as a 16-bit unsigned integer
**         number.
**     Parameters  :
**         NAME       - DESCRIPTION
**         Time       - Duty to set [in milliseconds]
**                      (0 to 5 ms in high speed mode)
**     Returns     :
**         ---        - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_MATH - Overflow during evaluation
**                           ERR_RANGE - Parameter out of range
** ===================================================================
*/
byte PWM11_SetDutyMS(word Time)
{
  dlong rtval;                         /* Result of two 32-bit numbers multiplication */

  if (Time > 0x05U) {                  /* Is the given value out of range? */
    return ERR_RANGE;                  /* If yes then error */
  }
  PE_Timer_LngMul((dword)Time, (dword)898293814, &rtval); /* Multiply given value and high speed CPU mode coefficient */
  if (PE_Timer_LngHi2(rtval[0], rtval[1], &RatioStore)) { /* Is the result greater or equal than 65536 ? */
    RatioStore = 0xFFFFU;              /* If yes then use maximal possible value */
  }
  SetRatio();                          /* Calculate and set up new appropriate values of the duty and period registers */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  PWM11_Init (component PWM)
**
**     Description :
**         Initializes the associated peripheral(s) and the component's 
**         internal variables. The method is called automatically as a 
**         part of the application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void PWM11_Init(void)
{
  /* PWMCNT5: BIT7=0,BIT6=0,BIT5=0,BIT4=0,BIT3=0,BIT2=0,BIT1=0,BIT0=0 */
  setReg8(PWMCNT5, 0x00U);             /* Reset Counter */ 
  /* PWMSDN: PWMIF=1,PWMIE=0,PWMRSTRT=0,PWMLVL=0,??=0,PWM5IN=0,PWM5INL=0,PWM5ENA=0 */
  setReg8(PWMSDN, 0x80U);              /* Emergency shutdown feature settings */ 
  RatioStore = 0x0101U;                /* Store initial value of the ratio */
  /* PWMDTY5: BIT7=0,BIT6=0,BIT5=0,BIT4=0,BIT3=0,BIT2=0,BIT1=0,BIT0=1 */
  setReg8(PWMDTY5, 0x01U);             /* Store initial value to the duty-compare register */ 
  /* PWMPER5: BIT7=1,BIT6=1,BIT5=1,BIT4=1,BIT3=1,BIT2=1,BIT1=1,BIT0=1 */
  setReg8(PWMPER5, 0xFFU);             /* and to the period register */ 
  /* PWMCLK: PCLK5=1 */
  setReg8Bits(PWMCLK, 0x20U);          /* Select clock source */ 
  /* PWME: PWME5=1 */
  setReg8Bits(PWME, 0x20U);            /* Run counter */ 
}

/* END PWM11. */

/*
** ###################################################################
**
**     This file was created by Processor Expert 3.02 [04.44]
**     for the Freescale HCS12 series of microcontrollers.
**
** ###################################################################
*/
