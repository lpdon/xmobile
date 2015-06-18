/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : Inhr2.h
**     Project   : Project
**     Processor : MC9S12C32MFA25
**     Component : BitIO
**     Version   : Component 02.075, Driver 03.15, CPU db: 2.87.402
**     Compiler  : CodeWarrior HC12 C Compiler
**     Date/Time : 18.06.2015, 19:31
**     Abstract  :
**         This component "BitIO" implements an one-bit input/output.
**         It uses one bit/pin of a port.
**         Note: This component is not set in the "Safe mode". See help.
**         Methods of this component are mostly implemented as a macros
**         (if supported by target language and compiler).
**     Settings  :
**         Used pin                    :
**             ----------------------------------------------------
**                Number (on package)  |    Name
**             ----------------------------------------------------
**                       2             |  PT1_PWM1_IOC1
**             ----------------------------------------------------
**
**         Port name                   : T
**
**         Bit number (in port)        : 1
**         Bit mask of the port        : $0002
**
**         Initial direction           : Input (direction can be changed)
**         Safe mode                   : no
**         Initial output value        : 1
**         Initial pull option         : up
**
**         Port data register          : PTT       [$0240]
**         Port control register       : DDRT      [$0242]
**
**         Optimization for            : speed
**     Contents  :
**         SetDir - void Inhr2_SetDir(bool Dir);
**         GetVal - bool Inhr2_GetVal(void);
**         PutVal - void Inhr2_PutVal(bool Val);
**         ClrVal - void Inhr2_ClrVal(void);
**         SetVal - void Inhr2_SetVal(void);
**         NegVal - void Inhr2_NegVal(void);
**
**     Copyright : 1997 - 2010 Freescale Semiconductor, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/

#ifndef Inhr2_H_
#define Inhr2_H_

/* MODULE Inhr2. */

  /* Including shared modules, which are used in the whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "Cpu.h"

#pragma CODE_SEG Inhr2_CODE
/*
** ===================================================================
**     Method      :  Inhr2_GetVal (component BitIO)
**
**     Description :
**         This method returns an input value.
**           a) direction = Input  : reads the input value from the
**                                   pin and returns it
**           b) direction = Output : returns the last written value
**     Parameters  : None
**     Returns     :
**         ---             - Input value. Possible values:
**                           FALSE - logical "0" (Low level)
**                           TRUE - logical "1" (High level)

** ===================================================================
*/
#define Inhr2_GetVal() ( \
    (bool)((getReg8(PTT) & 0x02U))     /* Return port data */ \
  )

/*
** ===================================================================
**     Method      :  Inhr2_PutVal (component BitIO)
**
**     Description :
**         This method writes the new output value.
**         Note: If direction is set to input the method code may not
**               work properly.
**     Parameters  :
**         NAME       - DESCRIPTION
**         Val             - Output value. Possible values:
**                           FALSE - logical "0" (Low level)
**                           TRUE - logical "1" (High level)
**     Returns     : Nothing
** ===================================================================
*/
void Inhr2_PutVal(bool Val);

/*
** ===================================================================
**     Method      :  Inhr2_ClrVal (component BitIO)
**
**     Description :
**         This method clears (sets to zero) the output value.
**         Note: If direction is set to input the method code may not
**               work properly.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
#define Inhr2_ClrVal() ( \
    (void)clrReg8Bits(PTT, 0x02U)      /* PTT1=0x00U */ \
  )

/*
** ===================================================================
**     Method      :  Inhr2_SetVal (component BitIO)
**
**     Description :
**         This method sets (sets to one) the output value.
**         Note: If direction is set to input the method code may not
**               work properly.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
#define Inhr2_SetVal() ( \
    (void)setReg8Bits(PTT, 0x02U)      /* PTT1=0x01U */ \
  )

/*
** ===================================================================
**     Method      :  Inhr2_NegVal (component BitIO)
**
**     Description :
**         This method negates (inverts) the output value.
**         Note: If direction is set to input the method code may not
**               work properly.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
#define Inhr2_NegVal() ( \
    (void)invertReg8Bits(PTT, 0x02U)   /* PTT1=invert */ \
  )

/*
** ===================================================================
**     Method      :  Inhr2_SetDir (component BitIO)
**
**     Description :
**         This method sets direction of the component.
**     Parameters  :
**         NAME       - DESCRIPTION
**         Dir        - Direction to set (FALSE or TRUE)
**                      FALSE = Input, TRUE = Output
**     Returns     : Nothing
** ===================================================================
*/
void Inhr2_SetDir(bool Dir);

#pragma CODE_SEG DEFAULT

/* END Inhr2. */
#endif /* #ifndef __Inhr2_H_ */
/*
** ###################################################################
**
**     This file was created by Processor Expert 3.02 [04.44]
**     for the Freescale HCS12 series of microcontrollers.
**
** ###################################################################
*/
