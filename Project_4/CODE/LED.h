/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : LED.h
**     Project   : Project_4
**     Processor : MC9S12C32CFU16
**     Component : BitIO
**     Version   : Component 02.075, Driver 03.15, CPU db: 2.87.410
**     Compiler  : CodeWarrior HC12 C Compiler
**     Date/Time : 7/20/2015, 12:44 AM
**     Abstract  :
**         This component "BitIO" implements an one-bit input/output.
**         It uses one bit/pin of a port.
**         Note: This component is set to work in Output direction only.
**         Methods of this component are mostly implemented as a macros
**         (if supported by target language and compiler).
**     Settings  :
**         Used pin                    :
**             ----------------------------------------------------
**                Number (on package)  |    Name
**             ----------------------------------------------------
**                       41            |  PA0_ADDR8_DATA8
**             ----------------------------------------------------
**
**         Port name                   : A
**
**         Bit number (in port)        : 0
**         Bit mask of the port        : $0001
**
**         Initial direction           : Output (direction cannot be changed)
**         Initial output value        : 0
**         Initial pull option         : off
**
**         Port data register          : PORTA     [$0000]
**         Port control register       : DDRA      [$0002]
**
**         Optimization for            : speed
**     Contents  :
**         GetVal - bool LED_GetVal(void);
**         PutVal - void LED_PutVal(bool Val);
**         ClrVal - void LED_ClrVal(void);
**         SetVal - void LED_SetVal(void);
**
**     Copyright : 1997 - 2010 Freescale Semiconductor, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/

#ifndef LED_H_
#define LED_H_

/* MODULE LED. */

  /* Including shared modules, which are used in the whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "PE_Timer.h"
#include "Cpu.h"

#pragma CODE_SEG LED_CODE
/*
** ===================================================================
**     Method      :  LED_GetVal (component BitIO)
**
**     Description :
**         This method returns an input value.
**           a) direction = Input  : reads the input value from the
**                                   pin and returns it
**           b) direction = Output : returns the last written value
**         Note: This component is set to work in Output direction only.
**     Parameters  : None
**     Returns     :
**         ---             - Input value. Possible values:
**                           FALSE - logical "0" (Low level)
**                           TRUE - logical "1" (High level)

** ===================================================================
*/
#define LED_GetVal() ( \
    (bool)((getReg8(PORTA) & 0x01U))   /* Return port data */ \
  )

/*
** ===================================================================
**     Method      :  LED_PutVal (component BitIO)
**
**     Description :
**         This method writes the new output value.
**     Parameters  :
**         NAME       - DESCRIPTION
**         Val             - Output value. Possible values:
**                           FALSE - logical "0" (Low level)
**                           TRUE - logical "1" (High level)
**     Returns     : Nothing
** ===================================================================
*/
void LED_PutVal(bool Val);

/*
** ===================================================================
**     Method      :  LED_ClrVal (component BitIO)
**
**     Description :
**         This method clears (sets to zero) the output value.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
#define LED_ClrVal() ( \
    (void)clrReg8Bits(PORTA, 0x01U)    /* BIT0=0x00U */ \
  )

/*
** ===================================================================
**     Method      :  LED_SetVal (component BitIO)
**
**     Description :
**         This method sets (sets to one) the output value.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
#define LED_SetVal() ( \
    (void)setReg8Bits(PORTA, 0x01U)    /* BIT0=0x01U */ \
  )

#pragma CODE_SEG DEFAULT

/* END LED. */
#endif /* #ifndef __LED_H_ */
/*
** ###################################################################
**
**     This file was created by Processor Expert 3.02 [04.44]
**     for the Freescale HCS12 series of microcontrollers.
**
** ###################################################################
*/