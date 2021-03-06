/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : AD1.c
**     Project   : Project_4
**     Processor : MC9S12C32CFU16
**     Component : ADC
**     Version   : Component 01.593, Driver 01.18, CPU db: 2.87.410
**     Compiler  : CodeWarrior HC12 C Compiler
**     Date/Time : 7/20/2015, 9:48 PM
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
**             Prescaler               : divide-by-6
**     Contents  :
**         Enable         - byte AD1_Enable(void);
**         Start          - byte AD1_Start(void);
**         Measure        - byte AD1_Measure(bool WaitForResult);
**         GetValue8      - byte AD1_GetValue8(byte *Values);
**         GetChanValue8  - byte AD1_GetChanValue8(byte Channel, byte *Value);
**         GetValue16     - byte AD1_GetValue16(word *Values);
**         GetChanValue16 - byte AD1_GetChanValue16(byte Channel, word *Value);
**
**     Copyright : 1997 - 2010 Freescale Semiconductor, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/


/* MODULE AD1. */

#pragma MESSAGE DISABLE C5703          /* Disable warning C5703 "Parameter is not referenced" */
#pragma MESSAGE DISABLE C4002          /* Disable warning C4002 "Result not used is ignored" */
#pragma MESSAGE DISABLE C12056         /* Disable warning C12056 "SP debug info incorrect because of optimization or inline assembler" */

#include "Events.h"
#include "AD1.h"

#pragma DATA_SEG AD1_DATA              /* Select data segment "AD1_DATA" */
#pragma CODE_SEG AD1_CODE
#pragma CONST_SEG AD1_CONST            /* Constant section for this module */

#define STOP            0U             /* STOP state           */
#define MEASURE         1U             /* MESURE state         */
#define CONTINUOUS      2U             /* CONTINUOUS state      */
#define SINGLE          3U             /* SINGLE state         */

static const byte Channels[] = {       /* Contents for the device control register */
0x80U,0x81U,0x82U,0x83U};
static bool OutFlg;                    /* Measurement finish flag */
static byte SumChan;                   /* Number of measured channels */
volatile static byte ModeFlg;          /* Current state of device */
static word AD1_OutV[4];               /* Array of measured values */
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
#pragma CODE_SEG __NEAR_SEG NON_BANKED
ISR(AD1_Interrupt)
{
  if (ModeFlg == STOP) {               /* Test if the device is in an allowed mode */
    (void) ATDDR0;                     /* Dummy read to clear the SCF interrupt flag */
    return;                            /* if not, return from the interrupt */
  }
  AD1_OutV[SumChan] = ATDDR0;          /* Save measured value */
  SumChan++;                           /* Number of measurement */
  if (SumChan == 4U) {                 /* Is number of measurement equal to the number of channels? */
    SumChan = 0U;                      /* If yes then set the number of measurement to 0 */
    OutFlg = TRUE;                     /* Measured values are available */
    AD1_OnEnd();                       /* Invoke user event */
    if (ModeFlg == MEASURE) {          /* Is the device in the measure state? */
      ModeFlg = STOP;                  /* Set the device to the stop mode */
      return;                          /* Return from interrupt */
    }
  }
  ATDCTL5 = Channels[SumChan];         /* Start measurement of next channel */
}

#pragma CODE_SEG AD1_CODE
/*
** ===================================================================
**     Method      :  HWEnDi (component ADC)
**
**     Description :
**         Enables or disables the peripheral(s) associated with the 
**         component. The method is called automatically as a part of the 
**         Enable and Disable methods and several internal methods.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void HWEnDi(void)
{
  if (ModeFlg) {                       /* Start or stop measurement? */
    OutFlg = FALSE;                    /* Output values aren't available */
    SumChan = 0U;                      /* Set the number of measured channels to 0 */
    /* ATDCTL5: DJM=1,DSGN=0,SCAN=0,MULT=0,??=0,CC=0,CB=0,CA=0 */
    ATDCTL5 = 0x80U;                   /* Start the conversion */
  }
  else {
    /* ATDCTL3: ??=0,S8C=0,S4C=0,S2C=0,S1C=1,FIFO=0,FRZ1=0,FRZ0=0 */
    ATDCTL3 = 0x08U;                   /* Abort current measurement */
  }
}

/*
** ===================================================================
**     Method      :  AD1_Enable (component ADC)
**
**     Description :
**         Enables A/D converter component. <Events> may be generated
**         (<DisableEvent>/<EnableEvent>). If possible, this method
**         switches on A/D converter device, voltage reference, etc.
**     Parameters  : None
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/
byte AD1_Enable(void)
{
  HWEnDi();                            /* Enable the device */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  AD1_Start (component ADC)
**
**     Description :
**         This method starts continuous conversion on all channels
**         that are set in the component inspector. When each
**         measurement on all channels has finished the <OnEnd > event
**         may be invoked. This method is not available if the
**         <interrupt service> is disabled and the device doesn't
**         support the continuous mode. Note: If time of measurement is
**         too short and the instruction clock is too slow then the
**         conversion complete interrupt and its handler may cause a
**         system overflow.
**     Parameters  : None
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_DISABLED - Device is disabled
**                           ERR_BUSY - A conversion is already running
** ===================================================================
*/
byte AD1_Start(void)
{
  if (ModeFlg != STOP) {               /* Is the device in running mode? */
    return ERR_BUSY;                   /* If yes then error */
  }
  ModeFlg = CONTINUOUS;                /* Set state of device to the continuos mode */
  HWEnDi();                            /* Start measurement */
  return ERR_OK;                       /* OK */
}

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
byte AD1_Measure(bool WaitForResult)
{
  if (ModeFlg != STOP) {               /* Is the device in different mode than "stop"? */
    return ERR_BUSY;                   /* If yes then error */
  }
  ModeFlg = MEASURE;                   /* Set state of device to the measure mode */
  HWEnDi();                            /* Enable the device */
  if (WaitForResult) {                 /* Is WaitForResult TRUE? */
    while (ModeFlg == MEASURE) {}      /* If yes then wait for end of measurement */
  }
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  AD1_GetValue8 (component ADC)
**
**     Description :
**         This method returns the last measured values of all channels
**         justified to the left. Compared with <GetValue> method this
**         method returns more accurate result if the <number of
**         conversions> is greater than 1 and <AD resolution> is less
**         than 8 bits. In addition, the user code dependency on <AD
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
byte AD1_GetValue8(byte *Values)
{
  if (!OutFlg) {                       /* Is measured value(s) available? */
    return ERR_NOTAVAIL;               /* If no then error */
  }
  /* Note: Next 4 lines are speed optimized */
  *Values++ = (byte)(AD1_OutV[0] >> 2U); /* Save measured values to the output buffer */
  *Values++ = (byte)(AD1_OutV[1] >> 2U); /* Save measured values to the output buffer */
  *Values++ = (byte)(AD1_OutV[2] >> 2U); /* Save measured values to the output buffer */
  *Values = (byte)(AD1_OutV[3] >> 2U); /* Save measured values to the output buffer */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  AD1_GetChanValue8 (component ADC)
**
**     Description :
**         This method returns the last measured value of required
**         channel justified to the left. Compared with <GetChanValue>
**         method this method returns more accurate result if the
**         <number of conversions> is greater than 1 and <AD resolution>
**         is less than 8 bits. In addition, the user code dependency
**         on <AD resolution> is eliminated.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Channel         - Channel number. If only one
**                           channel in the component is set then this
**                           parameter is ignored.
**       * Value           - Pointer to the measured value.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_NOTAVAIL - Requested value not
**                           available
**                           ERR_RANGE - Parameter "Channel" out of
**                           range
**                           ERR_OVERRUN - External trigger overrun flag
**                           was detected after the last value(s) was
**                           obtained (for example by GetValue). This
**                           error may not be supported on some CPUs
**                           (see generated code).
** ===================================================================
*/
byte AD1_GetChanValue8(byte Channel,byte *Value)
{
  if (Channel >= 4U) {                 /* Is channel number greater than or equal to 4 */
    return ERR_RANGE;                  /* If yes then error */
  }
  if (!OutFlg) {                       /* Is measured value(s) available? */
    return ERR_NOTAVAIL;
  }
  *Value = (byte)(AD1_OutV[Channel] >> 2U); /* Save measured values to the output buffer */
  return ERR_OK;                       /* OK */
}

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
byte AD1_GetValue16(word *Values)
{
  if (!OutFlg) {                       /* Is measured value(s) available? */
    return ERR_NOTAVAIL;               /* If no then error */
  }
  /* Note: Next 4 lines are speed optimized */
  *Values++ = AD1_OutV[0] << 6U;       /* Save measured values to the output buffer */
  *Values++ = AD1_OutV[1] << 6U;       /* Save measured values to the output buffer */
  *Values++ = AD1_OutV[2] << 6U;       /* Save measured values to the output buffer */
  *Values = AD1_OutV[3] << 6U;         /* Save measured values to the output buffer */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  AD1_GetChanValue16 (component ADC)
**
**     Description :
**         This method returns the last measured value of the required
**         channel justified to the left. Compared with <GetChanValue>
**         method this method returns more accurate result if the
**         <number of conversions> is greater than 1 and <AD resolution>
**         is less than 16 bits. In addition, the user code dependency
**         on <AD resolution> is eliminated.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Channel         - Channel number. If only one
**                           channel in the component is set then this
**                           parameter is ignored.
**       * Value           - Pointer to the measured value.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_NOTAVAIL - Requested value not
**                           available
**                           ERR_RANGE - Parameter "Channel" out of
**                           range
**                           ERR_OVERRUN - External trigger overrun flag
**                           was detected after the last value(s) was
**                           obtained (for example by GetValue). This
**                           error may not be supported on some CPUs
**                           (see generated code).
** ===================================================================
*/
byte AD1_GetChanValue16(byte Channel,word *Value)
{
  if (Channel >= 4U) {                 /* Is channel number greater than or equal to 4 */
    return ERR_RANGE;                  /* If yes then error */
  }
  if (!OutFlg) {                       /* Is measured value(s) available? */
    return ERR_NOTAVAIL;
  }
  *Value = AD1_OutV[Channel] << 6U;    /* Save measured values to the output buffer */
  return ERR_OK;                       /* OK */
}

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
void AD1_Init(void)
{
  OutFlg = FALSE;                      /* No measured value */
  ModeFlg = STOP;                      /* Device isn't running */
  /* ATDCTL4: SRES8=0,SMP1=1,SMP0=1,PRS4=0,PRS3=0,PRS2=0,PRS1=1,PRS0=0 */
  ATDCTL4 = 0x62U;                     /* Set resolution, sample time and prescaler */
  /* ATDCTL3: ??=0,S8C=0,S4C=0,S2C=0,S1C=1,FIFO=0,FRZ1=0,FRZ0=0 */
  ATDCTL3 = 0x08U;                     /* Set ATD control register 3 */
  /* ATDCTL2: ADPU=1,AFFC=1,AWAI=0,ETRIGLE=0,ETRIGP=0,ETRIGE=0,ASCIE=1,ASCIF=0 */
  ATDCTL2 = 0xC2U;                     /* Set ATD control register 2 */
}


/* END AD1. */

/*
** ###################################################################
**
**     This file was created by Processor Expert 3.02 [04.44]
**     for the Freescale HCS12 series of microcontrollers.
**
** ###################################################################
*/
