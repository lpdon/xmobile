/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : AD1.h
**     Project   : Project_4
**     Processor : MC9S12C32CFU16
**     Component : ADC
**     Version   : Component 01.593, Driver 01.18, CPU db: 2.87.410
**     Compiler  : CodeWarrior HC12 C Compiler
**     Date/Time : 13.06.2015, 18:49
**     Abstract  :
**         This device "ADC" implements an A/D converter,
**         its control methods and interrupt/event handling procedure.
**     Settings  :
**         AD control register         : ATDCTL2     [$0082]
**         AD control register         : ATDCTL3     [$0083]
**         AD control register         : ATDCTL4     [$0084]
**         AD control register         : ATDCTL5     [$0085]
**         AD control register         : ATDSTAT0    [$0086]
**         AD control register         : ATDSTAT1    [$008B]
**         AD control register         : ATDCTL23    [$0082]
**         AD control register         : ATDCTL45    [$0084]
**         AD control register         : ATDTEST1    [$0089]
**         AD control register         : ATDDIEN     [$008D]
**         Interrupt name              : Vatd0
**         Interrupt enable reg.       : ATDCTL23    [$0082]
**         Priority                    : 1
**         User handling procedure     : AD1_OnEnd
**         Number of conversions       : 1
**         AD resolution               : 10-bit
**
**         Input pins
**
**              Port name              : AD0
**              Bit number (in port)   : 0
**              Bit mask of the port   : $0001
**              Port data register     : PTAD        [$0270]
**              Port control register  : DDRAD       [$0272]
**
**              Port name              : AD0
**              Bit number (in port)   : 1
**              Bit mask of the port   : $0002
**              Port data register     : PTAD        [$0270]
**              Port control register  : DDRAD       [$0272]
**
**              Port name              : AD0
**              Bit number (in port)   : 2
**              Bit mask of the port   : $0004
**              Port data register     : PTAD        [$0270]
**              Port control register  : DDRAD       [$0272]
**
**              Port name              : AD0
**              Bit number (in port)   : 3
**              Bit mask of the port   : $0008
**              Port data register     : PTAD        [$0270]
**              Port control register  : DDRAD       [$0272]
**
**         Initialization:
**              Conversion             : Enabled
**              Event                  : Enabled
**         High speed mode
**             Prescaler               : divide-by-4
**     Contents  :
**         Measure    - byte AD1_Measure(bool WaitForResult);
**         GetValue16 - byte AD1_GetValue16(word *Values);
**
**     Copyright : 1997 - 2010 Freescale Semiconductor, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/

#ifndef __AD1
#define __AD1

/* MODULE AD1. */

/*Include shared modules, which are used for whole project*/
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "Cpu.h"

#pragma DATA_SEG AD1_DATA              /* Select data segment "AD1_DATA" */
#pragma CODE_SEG AD1_CODE


#define AD1_EnableTrigger AD1_EnableExtChanTrigger
/*
** ===================================================================
** The EnableTrigger method has been renamed to
** the EnableExtChanTrigger.
** This macro is here to keep project backward compatibility.
** ===================================================================
*/

#define AD1_DisableTrigger AD1_Stop
/*
** ===================================================================
** Now, the Stop method is used for disabling the trigger mode
** instead of the DisableTrigger.
** This macro is here to keep project backward compatibility.
** ===================================================================
*/

#pragma CODE_SEG __NEAR_SEG NON_BANKED
__interrupt void AD1_Interrupt(void);
#pragma CODE_SEG AD1_CODE
/*
** ===================================================================
**     Method      :  AD1_Interrupt (component ADC)
**
**     Description :
**         The method services the interrupt of the selected peripheral(s)
**         and eventually invokes event(s) of the component.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/


byte AD1_Measure(bool WaitForResult);
/*
** ===================================================================
**     Method      :  AD1_Measure (component ADC)
**
**     Description :
**         This method performs one measurement on all channels that
**         are set in the component inspector. (Note: If the <number of
**         conversions> is more than one the conversion of A/D channels
**         is performed specified number of times.)
**     Parameters  :
**         NAME            - DESCRIPTION
**         WaitForResult   - Wait for a result of a
**                           conversion. If <interrupt service> is
**                           disabled, A/D peripheral doesn't support
**                           measuring all channels at once or Autoscan
**                           mode property isn't enabled and at the same
**                           time the <number of channel> is greater
**                           than 1, then the WaitForResult parameter is
**                           ignored and the method waits for each
**                           result every time. If the <interrupt
**                           service> is disabled and a <number of
**                           conversions> is greater than 1, the
**                           parameter is ignored and the method also
**                           waits for each result every time.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_DISABLED - Device is disabled
**                           ERR_BUSY - A conversion is already running
** ===================================================================
*/

byte AD1_GetValue16(word *Values);
/*
** ===================================================================
**     Method      :  AD1_GetValue16 (component ADC)
**
**     Description :
**         This method returns the last measured values of all channels
**         justified to the left. Compared with <GetValue> method this
**         method returns more accurate result if the <number of
**         conversions> is greater than 1 and <AD resolution> is less
**         than 16 bits. In addition, the user code dependency on <AD
**         resolution> is eliminated.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * Values          - Pointer to the array that contains
**                           the measured data.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_NOTAVAIL - Requested value not
**                           available
**                           ERR_OVERRUN - External trigger overrun flag
**                           was detected after the last value(s) was
**                           obtained (for example by GetValue). This
**                           error may not be supported on some CPUs
**                           (see generated code).
** ===================================================================
*/

void AD1_Init(void);
/*
** ===================================================================
**     Method      :  AD1_Init (component ADC)
**
**     Description :
**         Initializes the associated peripheral(s) and the component's 
**         internal variables. The method is called automatically as a 
**         part of the application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

#pragma DATA_SEG DEFAULT               /* Select data segment "DEFAULT" */
#pragma CODE_SEG DEFAULT

/* END AD1. */

#endif /* ifndef __AD1 */
/*
** ###################################################################
**
**     This file was created by Processor Expert 3.02 [04.44]
**     for the Freescale HCS12 series of microcontrollers.
**
** ###################################################################
*/
