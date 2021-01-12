/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : BOARD_ID.c
**     Project   : Project_4
**     Processor : MC9S12C32CFU16
**     Component : BitsIO
**     Version   : Component 02.102, Driver 03.14, CPU db: 2.87.410
**     Compiler  : CodeWarrior HC12 C Compiler
**     Date/Time : 7/18/2015, 7:49 PM
**     Abstract  :
**         This component "BitsIO" implements a multi-bit input/output.
**         It uses selected pins of one 1-bit to 8-bit port.
**     Settings  :
**         Port name                   : AD0
**
**         Bit mask of the port        : $00E0
**         Number of bits/pins         : 3
**         Single bit numbers          : 0 to 2
**         Values range                : 0 to 7
**
**         Initial direction           : Input (direction can be changed)
**         Safe mode                   : yes
**         Initial output value        : 0 = 000H
**         Initial pull option         : off
**
**         Port data register          : PTAD      [$0270]
**         Port control register       : DDRAD     [$0272]
**
**             ----------------------------------------------------
**                   Bit     |   Pin   |   Name
**             ----------------------------------------------------
**                    0      |    56   |   PAD05_AN05
**                    1      |    57   |   PAD06_AN06
**                    2      |    58   |   PAD07_AN07
**             ----------------------------------------------------
**
**         Optimization for            : speed
**     Contents  :
**         GetDir - bool BOARD_ID_GetDir(void);
**         SetDir - void BOARD_ID_SetDir(bool Dir);
**         GetVal - byte BOARD_ID_GetVal(void);
**         PutVal - void BOARD_ID_PutVal(byte Val);
**         GetBit - bool BOARD_ID_GetBit(byte Bit);
**         PutBit - void BOARD_ID_PutBit(byte Bit, bool Val);
**         SetBit - void BOARD_ID_SetBit(byte Bit);
**         ClrBit - void BOARD_ID_ClrBit(byte Bit);
**         NegBit - void BOARD_ID_NegBit(byte Bit);
**
**     Copyright : 1997 - 2010 Freescale Semiconductor, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/

/* MODULE BOARD_ID. */

#include "BOARD_ID.h"
  /* Including shared modules, which are used in the whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "PE_Timer.h"
#include "Cpu.h"

#pragma DATA_SEG BOARD_ID_DATA         /* Select data segment "BOARD_ID_DATA" */
#pragma CODE_SEG BOARD_ID_CODE
#pragma CONST_SEG BOARD_ID_CONST       /* Constant section for this module */
/*
** ===================================================================
**     Method      :  BOARD_ID_GetMsk (component BitsIO)
**
**     Description :
**         The method returns a bit mask which corresponds to the 
**         required bit position.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static const byte BOARD_ID_Table[3U] = { /* Table of mask constants */
   0x20U, 0x40U, 0x80U
};

static byte BOARD_ID_GetMsk (byte PinIndex)
{
  return (byte)((PinIndex<3U) ? BOARD_ID_Table[PinIndex] : 0U); /* Check range and return appropriate bit mask */
}

/*
** ===================================================================
**     Method      :  BOARD_ID_GetVal (component BitsIO)
**
**     Description :
**         This method returns an input value.
**           a) direction = Input  : reads the input value from the
**                                   pins and returns it
**           b) direction = Output : returns the last written value
**     Parameters  : None
**     Returns     :
**         ---        - Input value (0 to 7)
** ===================================================================
*/
byte BOARD_ID_GetVal(void)
{
  return (byte)((getReg8(PTAD) & 0xE0U) >> 5U); /* Return port data */
}

/*
** ===================================================================
**     Method      :  BOARD_ID_PutVal (component BitsIO)
**
**     Description :
**         This method writes the new output value.
**           a) direction = Input  : sets the new output value;
**                                   this operation will be shown on
**                                   output after the direction has
**                                   been switched to output
**                                   (SetDir(TRUE);)
**           b) direction = Output : directly writes the value to the
**                                   appropriate pins
**     Parameters  :
**         NAME       - DESCRIPTION
**         Val        - Output value (0 to 7)
**     Returns     : Nothing
** ===================================================================
*/
void BOARD_ID_PutVal(byte Val)
{
  Val = (byte)((Val & 0x07U) << 5U);   /* Mask and shift output value */
  Shadow_AD0 = (byte)((Shadow_AD0 & 0x1FU) | Val); /* Set-up shadow variable */
  setReg8(PTAD, (getReg8(PTAD) & (byte)(~(byte)0xE0U)) | Val); /* Put masked value on port */
}

/*
** ===================================================================
**     Method      :  BOARD_ID_GetBit (component BitsIO)
**
**     Description :
**         This method returns the specified bit of the input value.
**           a) direction = Input  : reads the input value from pins
**                                   and returns the specified bit
**           b) direction = Output : returns the specified bit
**                                   of the last written value
**     Parameters  :
**         NAME       - DESCRIPTION
**         Bit        - Number of the bit to read (0 to 2)
**     Returns     :
**         ---        - Value of the specified bit (FALSE or TRUE)
**                      FALSE = "0" or "Low", TRUE = "1" or "High"
** ===================================================================
*/
bool BOARD_ID_GetBit(byte Bit)
{
  byte const Mask = BOARD_ID_GetMsk(Bit); /* Temporary variable - bit mask to test */
  return (bool)(((getReg8(PTAD) & Mask) == Mask)? 1U: 0U); /* Test if specified bit of port register is set */
}

/*
** ===================================================================
**     Method      :  BOARD_ID_PutBit (component BitsIO)
**
**     Description :
**         This method writes the new value to the specified bit
**         of the output value.
**           a) direction = Input  : sets the value of the specified
**                                   bit; this operation will be
**                                   shown on output after the
**                                   direction has been switched to
**                                   output (SetDir(TRUE);)
**           b) direction = Output : directly writes the value of the
**                                   bit to the appropriate pin
**     Parameters  :
**         NAME       - DESCRIPTION
**         Bit        - Number of the bit (0 to 2)
**         Val        - New value of the bit (FALSE or TRUE)
**                      FALSE = "0" or "Low", TRUE = "1" or "High"
**     Returns     : Nothing
** ===================================================================
*/
void BOARD_ID_PutBit(byte Bit, bool Val)
{
  byte const Mask = BOARD_ID_GetMsk(Bit); /* Temporary variable - put bit mask */
  if (Val) {
    setReg8Bits(PTAD, Mask);           /* [bit (Bit+5)]=0x01U */
    Shadow_AD0 |= Mask;                /* Set appropriate bit in shadow variable */
  } else { /* !Val */
    clrReg8Bits(PTAD, Mask);           /* [bit (Bit+5)]=0x00U */
    Shadow_AD0 &= (byte)~(byte)Mask;   /* Clear appropriate bit in shadow variable */
  } /* !Val */
}

/*
** ===================================================================
**     Method      :  BOARD_ID_ClrBit (component BitsIO)
**
**     Description :
**         This method clears (sets to zero) the specified bit
**         of the output value.
**         [ It is the same as "PutBit(Bit,FALSE);" ]
**           a) direction = Input  : sets the specified bit to "0";
**                                   this operation will be shown on
**                                   output after the direction has
**                                   beenswitched to output
**                                   (SetDir(TRUE);)
**           b) direction = Output : directly writes "0" to the
**                                   appropriate pin
**     Parameters  :
**         NAME       - DESCRIPTION
**         Bit        - Number of the bit to clear (0 to 2)
**     Returns     : Nothing
** ===================================================================
*/
void BOARD_ID_ClrBit(byte Bit)
{
  byte const Mask = BOARD_ID_GetMsk(Bit); /* Temporary variable - set bit mask */
  clrReg8Bits(PTAD, Mask);             /* [bit (Bit+5)]=0x00U */
  Shadow_AD0 &= (byte)~(byte)Mask;     /* Set appropriate bit in shadow variable */
}

/*
** ===================================================================
**     Method      :  BOARD_ID_SetBit (component BitsIO)
**
**     Description :
**         This method sets (sets to one) the specified bit of the
**         output value.
**         [ It is the same as "PutBit(Bit,TRUE);" ]
**           a) direction = Input  : sets the specified bit to "1";
**                                   this operation will be shown on
**                                   output after the direction has
**                                   been switched to output
**                                   (SetDir(TRUE);)
**           b) direction = Output : directly writes "1" to the
**                                   appropriate pin
**     Parameters  :
**         NAME       - DESCRIPTION
**         Bit        - Number of the bit to set (0 to 2)
**     Returns     : Nothing
** ===================================================================
*/
void BOARD_ID_SetBit(byte Bit)
{
  byte const Mask = BOARD_ID_GetMsk(Bit); /* Temporary variable - set bit mask */
  setReg8Bits(PTAD, Mask);             /* [bit (Bit+5)]=0x01U */
  Shadow_AD0 |= Mask;                  /* Set appropriate bit in shadow variable */
}

/*
** ===================================================================
**     Method      :  BOARD_ID_NegBit (component BitsIO)
**
**     Description :
**         This method negates (inverts) the specified bit of the
**         output value.
**           a) direction = Input  : inverts the specified bit;
**                                   this operation will be shown on
**                                   output after the direction has
**                                   been switched to output
**                                   (SetDir(TRUE);)
**           b) direction = Output : directly inverts the value
**                                   of the appropriate pin
**     Parameters  :
**         NAME       - DESCRIPTION
**         Bit        - Number of the bit to invert (0 to 31)
**     Returns     : Nothing
** ===================================================================
*/
void BOARD_ID_NegBit(byte Bit)
{
  byte const Mask = BOARD_ID_GetMsk(Bit); /* Temporary variable - set bit mask */
  invertReg8Bits(PTAD, Mask);          /* [bit (Bit+5)]=invert */
  Shadow_AD0 ^= Mask;                  /* Set appropriate bit in shadow variable */
}

/*
** ===================================================================
**     Method      :  BOARD_ID_GetDir (component BitsIO)
**
**     Description :
**         This method returns direction of the component.
**     Parameters  : None
**     Returns     :
**         ---        - Direction of the component (FALSE or TRUE)
**                      FALSE = Input, TRUE = Output
** ===================================================================
*/
/*
bool BOARD_ID_GetDir(void)

**  This method is implemented as a macro. See BOARD_ID.h file.  **
*/

/*
** ===================================================================
**     Method      :  BOARD_ID_SetDir (component BitsIO)
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
void BOARD_ID_SetDir(bool Dir)
{
  if (Dir) {
    setReg8(PTAD, (getReg8(PTAD) & (byte)(~(byte)0xE0U)) | (Shadow_AD0 & 0xE0U)); /* Put masked value on port */
    /* DDRAD: DDRAD7=1,DDRAD6=1,DDRAD5=1 */
    setReg8Bits(DDRAD, 0xE0U);          
  } else { /* !Dir */
    /* DDRAD: DDRAD7=0,DDRAD6=0,DDRAD5=0 */
    clrReg8Bits(DDRAD, 0xE0U);          
  } /* !Dir */
}


/* END BOARD_ID. */
/*
** ###################################################################
**
**     This file was created by Processor Expert 3.02 [04.44]
**     for the Freescale HCS12 series of microcontrollers.
**
** ###################################################################
*/