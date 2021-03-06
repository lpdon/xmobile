/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : PWM6.c
**     Project   : Project
**     Processor : MC9S12C32MFA25
**     Component : PWM
**     Version   : Component 02.231, Driver 01.16, CPU db: 2.87.402
**     Compiler  : CodeWarrior HC12 C Compiler
**     Date/Time : 18.06.2015, 19:31
**     Abstract  :
**         This component implements a pulse-width modulation generator
**         that generates signal with variable duty and fixed cycle. 
**     Settings  :
**         Used output pin             : 
**             ----------------------------------------------------
**                Number (on package)  |    Name
**             ----------------------------------------------------
**                       3             |  PT2_PWM2_IOC2
**             ----------------------------------------------------
**
**         Timer name                  : PWM2 [8-bit]
**         Counter                     : PWMCNT2   [$00EE]
**         Mode register               : PWMCTL    [$00E5]
**         Run register                : PWME      [$00E0]
**         Prescaler                   : PWMPRCLK  [$00E3]
**         Compare 1 register          : PWMPER2   [$00F4]
**         Compare 2 register          : PWMDTY2   [$00FA]
**         Flip-flop 1 register        : PWMPOL    [$00E1]
**
**         User handling procedure     : not specified
**
**         Output pin
**
**         Port name                   : T
**         Bit number (in port)        : 2
**         Bit mask of the port        : $0004
**         Port data register          : PTT       [$0240]
**         Port control register       : DDRT      [$0242]
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
**             Clock                   : 133333 Hz
**           Initial value of            period        pulse width (ratio 1.568%)
**             Xtal ticks              : 30600         480
**             microseconds            : 1912          30
**             milliseconds            : 2             0
**             seconds (real)          : 0.0019125     0.00003
**
**     Contents  :
**         SetRatio16 - byte PWM6_SetRatio16(word Ratio);
**         SetDutyUS  - byte PWM6_SetDutyUS(word Time);
**         SetDutyMS  - byte PWM6_SetDutyMS(word Time);
**
**     Copyright : 1997 - 2010 Freescale Semiconductor, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/


/* MODULE PWM6. */

#include "PWM6.h"

#pragma DATA_SEG PWM6_DATA             /* Select data segment "PWM6_DATA" */

#pragma CODE_SEG PWM6_CODE

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
  PWMDTY2 = (byte)(((0xFFUL * (dword)RatioStore) + 0x8000UL) >> 0x10U); /* Calculate new value according to the given ratio */
}
#pragma MESSAGE DEFAULT C12056         /* Re-enable WARNING C12056 */

/*
** ===================================================================
**     Method      :  PWM6_SetRatio16 (component PWM)
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
byte PWM6_SetRatio16(word Ratio)
{
  RatioStore = Ratio;                  /* Store new value of the ratio */
  SetRatio();                          /* Calculate and set up new appropriate values of the duty and period registers */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  PWM6_SetDutyUS (component PWM)
**
**     Description :
**         This method sets the new duty value of the output signal. The
**         duty is expressed in microseconds as a 16-bit unsigned integer
**         number.
**     Parameters  :
**         NAME       - DESCRIPTION
**         Time       - Duty to set [in microseconds]
**                      (0 to 1912 us in high speed mode)
**     Returns     :
**         ---        - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_MATH - Overflow during evaluation
**                           ERR_RANGE - Parameter out of range
** ===================================================================
*/
byte PWM6_SetDutyUS(word Time)
{
  dlong rtval;                         /* Result of two 32-bit numbers multiplication */

  if (Time > 0x0778U) {                /* Is the given value out of range? */
    return ERR_RANGE;                  /* If yes then error */
  }
  PE_Timer_LngMul((dword)Time, (dword)574908041, &rtval); /* Multiply given value and high speed CPU mode coefficient */
  if (PE_Timer_LngHi3(rtval[0], rtval[1], &RatioStore)) { /* Is the result greater or equal than 65536 ? */
    RatioStore = 0xFFFFU;              /* If yes then use maximal possible value */
  }
  SetRatio();                          /* Calculate and set up new appropriate values of the duty and period registers */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  PWM6_SetDutyMS (component PWM)
**
**     Description :
**         This method sets the new duty value of the output signal. The
**         duty is expressed in milliseconds as a 16-bit unsigned integer
**         number.
**     Parameters  :
**         NAME       - DESCRIPTION
**         Time       - Duty to set [in milliseconds]
**                      (0 to 2 ms in high speed mode)
**     Returns     :
**         ---        - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_MATH - Overflow during evaluation
**                           ERR_RANGE - Parameter out of range
** ===================================================================
*/
byte PWM6_SetDutyMS(word Time)
{
  dlong rtval;                         /* Result of two 32-bit numbers multiplication */

  if (Time > 0x02U) {                  /* Is the given value out of range? */
    return ERR_RANGE;                  /* If yes then error */
  }
  PE_Timer_LngMul((dword)Time, (dword)2245734534, &rtval); /* Multiply given value and high speed CPU mode coefficient */
  if (PE_Timer_LngHi2(rtval[0], rtval[1], &RatioStore)) { /* Is the result greater or equal than 65536 ? */
    RatioStore = 0xFFFFU;              /* If yes then use maximal possible value */
  }
  SetRatio();                          /* Calculate and set up new appropriate values of the duty and period registers */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  PWM6_Init (component PWM)
**
**     Description :
**         Initializes the associated peripheral(s) and the component's 
**         internal variables. The method is called automatically as a 
**         part of the application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void PWM6_Init(void)
{
  /* PWMCNT2: BIT7=0,BIT6=0,BIT5=0,BIT4=0,BIT3=0,BIT2=0,BIT1=0,BIT0=0 */
  setReg8(PWMCNT2, 0x00U);             /* Reset Counter */ 
  /* PWMSDN: PWMIF=1,PWMIE=0,PWMRSTRT=0,PWMLVL=0,??=0,PWM5IN=0,PWM5INL=0,PWM5ENA=0 */
  setReg8(PWMSDN, 0x80U);              /* Emergency shutdown feature settings */ 
  RatioStore = 0x0404U;                /* Store initial value of the ratio */
  /* PWMDTY2: BIT7=0,BIT6=0,BIT5=0,BIT4=0,BIT3=0,BIT2=1,BIT1=0,BIT0=0 */
  setReg8(PWMDTY2, 0x04U);             /* Store initial value to the duty-compare register */ 
  /* PWMPER2: BIT7=1,BIT6=1,BIT5=1,BIT4=1,BIT3=1,BIT2=1,BIT1=1,BIT0=1 */
  setReg8(PWMPER2, 0xFFU);             /* and to the period register */ 
  /* PWMPRCLK: ??=0,PCKB2=0,PCKB1=0,PCKB0=0,??=0,PCKA2=0,PCKA1=0,PCKA0=0 */
  setReg8(PWMPRCLK, 0x00U);            /* Set prescaler register */ 
  /* PWMSCLB: BIT7=0,BIT6=0,BIT5=0,BIT4=1,BIT3=1,BIT2=1,BIT1=1,BIT0=0 */
  setReg8(PWMSCLB, 0x1EU);             /* Set scale register */ 
  /* PWMCLK: PCLK2=1 */
  setReg8Bits(PWMCLK, 0x04U);          /* Select clock source */ 
  /* PWME: PWME2=1 */
  setReg8Bits(PWME, 0x04U);            /* Run counter */ 
}

/* END PWM6. */

/*
** ###################################################################
**
**     This file was created by Processor Expert 3.02 [04.44]
**     for the Freescale HCS12 series of microcontrollers.
**
** ###################################################################
*/
