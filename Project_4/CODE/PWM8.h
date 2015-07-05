/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : PWM8.h
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
**                       7             |  PT2_PWM2_IOC2
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
**             Clock                   : 53333 Hz
**           Initial value of            period        pulse width (ratio 0.392%)
**             Xtal ticks              : 57375         225
**             microseconds            : 4781          19
**             milliseconds            : 5             0
**             seconds (real)          : 0.00478125    0.00001875
**
**     Contents  :
**         SetRatio16 - byte PWM8_SetRatio16(word Ratio);
**         SetDutyUS  - byte PWM8_SetDutyUS(word Time);
**         SetDutyMS  - byte PWM8_SetDutyMS(word Time);
**
**     Copyright : 1997 - 2010 Freescale Semiconductor, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/

#ifndef __PWM8
#define __PWM8

/* MODULE PWM8. */

#include "Cpu.h"

#pragma CODE_SEG PWM8_CODE

#define PWM8_PERIOD_VALUE    0xFFUL    /* Initial period value in ticks of the timer */
#define PWM8_PERIOD_VALUE_HIGH 0xFFUL  /* Initial period value in ticks of the timer in high speed mode */

byte PWM8_SetRatio16(word Ratio);
/*
** ===================================================================
**     Method      :  PWM8_SetRatio16 (component PWM)
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

byte PWM8_SetDutyUS(word Time);
/*
** ===================================================================
**     Method      :  PWM8_SetDutyUS (component PWM)
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

byte PWM8_SetDutyMS(word Time);
/*
** ===================================================================
**     Method      :  PWM8_SetDutyMS (component PWM)
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

void PWM8_Init(void);
/*
** ===================================================================
**     Method      :  PWM8_Init (component PWM)
**
**     Description :
**         Initializes the associated peripheral(s) and the component's 
**         internal variables. The method is called automatically as a 
**         part of the application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

#pragma CODE_SEG DEFAULT

/* END PWM8. */

#endif /* ifndef __PWM8 */
/*
** ###################################################################
**
**     This file was created by Processor Expert 3.02 [04.44]
**     for the Freescale HCS12 series of microcontrollers.
**
** ###################################################################
*/
