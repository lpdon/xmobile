/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : CAN1.c
**     Project   : Project_4
**     Processor : MC9S12C32CFU16
**     Component : FreescaleCAN
**     Version   : Component 02.355, Driver 01.29, CPU db: 2.87.410
**     Compiler  : CodeWarrior HC12 C Compiler
**     Date/Time : 7/5/2015, 6:31 PM
**     Abstract  :
**         This component "FreescaleCAN" implements a CAN serial channel.
**     Settings  :
**         CAN channel                 : MSCAN
**
**         Protocol
**             Interrupt priority      : 1
**             Time segment 1          : 6
**             Time segment 2          : 7
**             RSJ                     : 1
**
**             Recieve accept. code    : 0x00
**             Recieve accept. mask    : 0xFFFFFFFF
**
**         Input interrupt
**             Vector name             : Vcanrx
**             Priority                : 1
**
**         Output interrupt
**             Vector name             : Vcantx
**             Priority                : 1
**
**     Contents  :
**         SetAcceptanceCode - byte CAN1_SetAcceptanceCode(dword AccCode1, dword AccCode2);
**         SetAcceptanceMask - byte CAN1_SetAcceptanceMask(dword AccMask1, dword AccMask2);
**         SetAcceptanceMode - byte CAN1_SetAcceptanceMode(byte Mode);
**         SendFrame         - byte CAN1_SendFrame(byte BufferNum, dword MessageID, byte FrameType, byte...
**         ReadFrame         - byte CAN1_ReadFrame(dword *MessageID, byte *FrameType, byte *FrameFormat,...
**         GetStateTX        - byte CAN1_GetStateTX(void);
**         GetStateRX        - bool CAN1_GetStateRX(void);
**         GetError          - byte CAN1_GetError(CAN1_TError *Err);
**         SendFrameExt      - byte CAN1_SendFrameExt(dword MessageID, byte FrameType, byte Length, byte...
**
**     Copyright : 1997 - 2010 Freescale Semiconductor, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/

/* MODULE CAN1. */


#include "CAN1.h"
#include "EI2C1.h"
#include "Inhr1.h"
#include "Inhr2.h"
#include "AD1.h"
#include "AS1.h"
#include "PWM8.h"
#include "PWM9.h"
#include "PWM10.h"
#include "PWM11.h"
#include "Events.h"

#define CAN_STANDARD_FRAME_MAX_ID    0x07FFU /* Max ID of the standard frame */
#define CAN_EXTENDED_FRAME_MAX_ID    0x1FFFFFFFUL /* Max ID of the extended frame */
#define CAN_EXTENDED_FRAME_MASK      0x60000000UL /* Invalid extended frame mask*/
#define CAN_MAX_DATA_LEN             0x08U /* Max number of data to be sent in one frame */
#define FULL_RX_BUF                  0x01U /* RX buffer full           */

#define MB_ID_IDE                    0x00080000UL
#define CAN_TX_MBUFFERS              0x03U /* Number of TX buffers */
#define CAN_MAX_RX_FIFO              0x04U /* Max length of the RX fifo */

#define CAN_STATUS_OVERRUN_MASK      0x02U /* Overrun error flag mask */
#define CAN_STATUS_TX_MASK           0x0CU /* Transmitter error state mask*/
#define CAN_STATUS_RX_MASK           0x30U /* Receiver error state mask*/
#define CAN_STATUS_BOFF_MASK         0x0CU /* Bus-Off state mask in register */
#define CAN_STATUS_BOFF_EXT_MASK     0x40U /* Bus-Off state mask in error flag */
#define CAN_STATUS_TX_PASS_MASK      0x08U /* Transmitter error passive state mask */
#define CAN_STATUS_RX_PASS_MASK      0x20U /* Receiver error passive state mask */
#define CAN_STATUS_TX_WARN_MASK      0x04U /* Transmitter warning mask */
#define CAN_STATUS_RX_WARN_MASK      0x10U /* Receiver warning mask */
#define CAN_STATUS_WAKEUP_MASK       0x80U /* Wakeup interrupt flag mask */

typedef struct {                       /* Message buffer structure */
          byte IDR0;
          byte IDR1;
          byte IDR2;
          byte IDR3;
          byte Data[CAN_MAX_DATA_LEN];
          byte DLR;
          byte TBPR;
          byte Reserved;
          byte Reserved2;
        }TMsgBuff;                     /* Message buffer structure */

typedef union {
          dword dw;
          struct {
            byte b0;
            byte b1;
            byte b2;
            byte b3;
          }b;
        }DwordSwap;

#pragma DATA_SEG CAN1_DATA             /* Select data segment "CAN1_DATA" */
#pragma CODE_SEG CAN1_CODE

static volatile byte ErrFlag;          /* Error flags mirror of the status register */
static volatile byte SerFlag;          /* Internal driver flags */

/*
** ===================================================================
**     Method      :  CAN1_SetAcceptanceMode (component FreescaleCAN)
**
**     Description :
**         Sets the acceptance mode register.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Mode            - Acceptance mode.
**                           Supported modes: 
**                           TWO_32_FILTERS - Two 32-bit acceptance
**                           filters
**                           FOUR_16_FILTERS - Four 16-bit acceptance
**                           filters
**                           EIGHT_8_FILTERS - Eight 8-bit acceptance
**                           filters
**                           FILTER_CLOSED - Filter closed
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_DISABLED - This component is disabled by
**                           user
**                           ERR_VALUE - Parameter has incorrect value
** ===================================================================
*/
byte CAN1_SetAcceptanceMode(byte Mode)
{
  if (Mode > 0x03U) {                  /* Is mode parameter greater then 3 */
    return ERR_VALUE;                  /* If yes then error */
  }
  EnterCritical();                     /* Enter critical section */
  CANCTL0_INITRQ = 0x01U;              /* Disable device */
  while(CANCTL1_INITAK == 0U){}        /* Wait for disable */
  CANIDAC_IDAM = Mode;                 /* Set acceptance mode of the receiver */
  CANCTL0_INITRQ = 0x00U;              /* Start device */
  while(CANCTL1_INITAK){}              /* Wait for enable */
  /* CANRFLG: WUPIF=1,CSCIF=1,RSTAT1=1,RSTAT0=1,TSTAT1=1,TSTAT0=1,OVRIF=1 */
  CANRFLG |= 0xFEU;                    /* Reset error flags */
  /* CANRIER: WUPIE=0,CSCIE=1,RSTATE1=1,RSTATE0=1,TSTATE1=1,TSTATE0=1,OVRIE=0,RXFIE=1 */
  CANRIER = 0x7DU;                     /* Enable interrupts */
  ExitCritical();                      /* Exit critical section */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  CAN1_GetStateRX (component FreescaleCAN)
**
**     Description :
**         Returns a value of the reception complete flag.
**     Parameters  : None
**     Returns     :
**         ---             - The value of the receiver complete flag
**                           of the given buffer. 
**                           Possible values:
**                           FALSE - message buffer is empty
**                           TRUE - message buffer isn't empty
** ===================================================================
*/
byte CAN1_GetStateRX(void)
{
  return ((SerFlag & FULL_RX_BUF) != 0x00U)? (byte)1 : (byte)0; /* Return status of the RX buffer */
}

/*
** ===================================================================
**     Method      :  CAN1_SetAcceptanceCode (component FreescaleCAN)
**
**     Description :
**         Sets the acceptance code registers. This method writes a
**         code mask directly to the acceptance code registers.
**     Parameters  :
**         NAME            - DESCRIPTION
**         AccCode1        - Acceptance code for the
**                           message filtering. This acceptance code
**                           will be written to the acceptance code
**                           registers IDAR0-IDAR3. The most
**                           significant byte of the acceptance code
**                           will be written to the IDAR0 register
**                           and the least significant byte of the
**                           acceptance code will be written to the
**                           IDAR3 register.
**         AccCode2        - Acceptance code for the
**                           message filtering. This acceptance code
**                           will be written to the acceptance code
**                           registers IDAR4-IDAR7. The most
**                           significant byte of the acceptance code
**                           will be written to the IDAR4 register
**                           and the least significant byte of the
**                           acceptance code will be written to the
**                           IDAR7 register.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_DISABLED - This component is disabled by
**                           user
** ===================================================================
*/
byte CAN1_SetAcceptanceCode(dword AccCode1, dword AccCode2)
{
  EnterCritical();                     /* Enter critical section */
  CANCTL0_INITRQ = 0x01U;              /* Disable device */
  while(CANCTL1_INITAK == 0U){}        /* Wait for disable */
  /*lint -save  -e926 -e927 -e928 -e929 Disable MISRA rule (11.4) checking. */
  CANIDAR3 = ((DwordSwap *)&AccCode1)->b.b3; /* Set acceptance code, register CANIDAR3 */
  CANIDAR2 = ((DwordSwap *)&AccCode1)->b.b2; /* Set acceptance code, register CANIDAR2 */
  CANIDAR1 = ((DwordSwap *)&AccCode1)->b.b1; /* Set acceptance code, register CANIDAR1 */
  CANIDAR0 = ((DwordSwap *)&AccCode1)->b.b0; /* Set acceptance code, register CANIDAR0 */
  CANIDAR7 = ((DwordSwap *)&AccCode2)->b.b3; /* Set acceptance code, register CANIDAR7 */
  CANIDAR6 = ((DwordSwap *)&AccCode2)->b.b2; /* Set acceptance code, register CANIDAR6 */
  CANIDAR5 = ((DwordSwap *)&AccCode2)->b.b1; /* Set acceptance code, register CANIDAR5 */
  CANIDAR4 = ((DwordSwap *)&AccCode2)->b.b0; /* Set acceptance code, register CANIDAR4 */
  /*lint -restore Enable MISRA rule (11.4) checking. */
  CANCTL0_INITRQ = 0x00U;              /* Start device */
  while(CANCTL1_INITAK){}              /* Wait for enable */
  /* CANRFLG: WUPIF=1,CSCIF=1,RSTAT1=1,RSTAT0=1,TSTAT1=1,TSTAT0=1,OVRIF=1 */
  CANRFLG |= 0xFEU;                    /* Reset error flags */
  /* CANRIER: WUPIE=0,CSCIE=1,RSTATE1=1,RSTATE0=1,TSTATE1=1,TSTATE0=1,OVRIE=0,RXFIE=1 */
  CANRIER = 0x7DU;                     /* Enable interrupts */
  ExitCritical();                      /* Exit critical section */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  CAN1_GetError (component FreescaleCAN)
**
**     Description :
**         Returns the content of the receiver flag register.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * Err             - Pointer to the returned set of errors
**     Returns     :
**         ---             - Error code (if GetError did not succeed),
**                           possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/
byte CAN1_GetError(CAN1_TError *Err)
{
  Err->err = 0x00U;                    /* Clear all errors */
  Err->errName.BusOff = ((ErrFlag & CAN_STATUS_BOFF_EXT_MASK) ? 1U : 0U); /* Bus-Off state */
  Err->errName.TxWarning = ((ErrFlag & CAN_STATUS_TX_WARN_MASK) ? 1U : 0U); /* Transmitter warning */
  Err->errName.RxWarning = ((ErrFlag & CAN_STATUS_RX_WARN_MASK) ? 1U : 0U); /* Receiver warning */
  Err->errName.RxPassive = ((ErrFlag & CAN_STATUS_RX_PASS_MASK) ? 1U : 0U); /* Receiver Error passive state */
  Err->errName.TxPassive = ((ErrFlag & CAN_STATUS_TX_PASS_MASK) ? 1U : 0U); /* Transmitter Error passive state */
  Err->errName.OverRun = ((ErrFlag & CAN_STATUS_OVERRUN_MASK) ? 1U : 0U); /* Overrun error flag */
  ErrFlag = 0x00U;                     /* Clear error flags */
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  CAN1_SendFrame (component FreescaleCAN)
**
**     Description :
**         Sends the frame via the CAN device. Using this method the
**         user can send own message to the CAN bus. This method
**         allows to specify CAN buffer number, message ID, data to
**         be sent and frame type (DATA_FRAME/REMOTE_FRAME).
**     Parameters  :
**         NAME            - DESCRIPTION
**         BufferNum       - Number of the buffer.
**         MessageID       - Identification of the
**                           message - ID. Message ID can be
**                           specified in the STANDARD format
**                           (default) or the EXTENDED format. The
**                           most significant bit in the ID is set to
**                           specify EXTENDED format. Predefined
**                           macro CAN_EXTENDED_FRAME_ID can be used
**                           (ID "bitwise or" CAN_EXTENDED_FRAME_ID)
**                           to mark ID as extended. If the most
**                           significant bit of ID is clear, STANDARD
**                           format is used.
**         FrameType       - Type of frame
**                           DATA_FRAME - data frame
**                           REMOTE_FRAME - remote frame
**         Length          - The length of the frame in bytes
**                           (0..8)
**       * Data            - Pointer to data
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_DISABLED - This component is disabled by
**                           user
**                           ERR_VALUE - Some parameter is out of
**                           possible range
**                           ERR_TXFULL - Transmition buffer is full.
** ===================================================================
*/
byte CAN1_SendFrame(byte BufferNum,dword MessageID,byte FrameType,byte Length,const byte *Data)
{
  byte i;                              /* Temorary variables */
  byte bufmask=(byte)((word)1 << BufferNum); /* Buffer mask */
  TMsgBuff *MsgBuff;
  dword tmpId;

  if (((MessageID & CAN_EXTENDED_FRAME_ID) == 0U) && (MessageID > CAN_STANDARD_FRAME_MAX_ID)) { /* Is the standard ID greater that 2047? */
    return ERR_VALUE;                  /* If yes then error */
  }
  if (((MessageID & CAN_EXTENDED_FRAME_ID) == CAN_EXTENDED_FRAME_ID) && ((MessageID & CAN_EXTENDED_FRAME_MASK) != 0U)) { /* Is the extended ID greater that 536870911? */
    return ERR_VALUE;                  /* If yes then error */
  }
  if ((BufferNum > (CAN_TX_MBUFFERS - 1U)) || (Length > CAN_MAX_DATA_LEN)) { /* Is BufferNum greater than CAN_MAXBUFF or Length greater than CAN_MAX_DATA_LEN? */
    return ERR_VALUE;                  /* If yes then error */
  }
  if (FrameType > REMOTE_FRAME) {      /* Is FrameType other than REMOTE_FRAME or DATA_FRAME */
    return ERR_VALUE;                  /* If yes then error */
  }
  EnterCritical();                     /* Disable global interrupts */
  if ((CANTFLG & bufmask) == 0U) {     /* Is the transmit buffer full? */
    ExitCritical();                    /* Enable global interrupts */
    return ERR_TXFULL;                 /* If yes then error */
  }
  CANTBSEL = bufmask;                  /* Select requested transmit buffer */
  /*lint -save  -e926 -e927 -e928 -e929 Disable MISRA rule (11.4) checking. */
  MsgBuff = (TMsgBuff *)&CANTXIDR0;
  /*lint -restore Enable MISRA rule (11.4) checking. */
  CANTIER_TXEIE |= bufmask;            /* Enable transmit interrupt of the given buffer */
  if (MessageID & CAN_EXTENDED_FRAME_ID) {
    tmpId = (((MessageID & 0x1FFC0000UL) << 3) | 0x00180000UL | ((MessageID & 0x0003FFFFUL) << 1)); /* Extended frame */
  }
  else {
    tmpId = MessageID << 21;           /* Standard frame */
  }
  /*lint -save  -e926 -e927 -e928 -e929 Disable MISRA rule (11.4) checking. */
  MsgBuff->IDR3 = ((DwordSwap *)&tmpId)->b.b3; /* Store the message ID */
  MsgBuff->IDR2 = ((DwordSwap *)&tmpId)->b.b2;
  MsgBuff->IDR1 = ((DwordSwap *)&tmpId)->b.b1;
  MsgBuff->IDR0 = ((DwordSwap *)&tmpId)->b.b0;
  /*lint -restore Enable MISRA rule (11.4) checking. */
  if (FrameType == DATA_FRAME) {       /* Is it a data frame? */
    for (i=0U; i<Length; i++) {
      MsgBuff->Data[i] = Data[i];      /* Store data to the transmit register */
    }
    if (MessageID & CAN_EXTENDED_FRAME_ID) { /* Is it the extended frame? */
      MsgBuff->IDR3 &= 0xFEU;          /* If no then set message type as "data frame" */
    }
    else {
      MsgBuff->IDR1 &= 0xEFU;          /* If yes then set message type as "data frame" */
    }
  }
  else {                               /* Remote frame */
    if (MessageID & CAN_EXTENDED_FRAME_ID) { /* Is it the extended frame? */
      MsgBuff->IDR3 |= 0x01U;          /* If yes then set message type as "remote frame" */
    }
    else {
      MsgBuff->IDR1 |= 0x10U;          /* If yes then set message type as "remote frame" */
    }
  }
  MsgBuff->DLR = Length;               /* Set the length of the message */
  MsgBuff->TBPR = 0x00U;               /* Set the priority (high) */
  CANTFLG = bufmask;                   /* Start transmission */
  ExitCritical();                      /* Enable global interrupts */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  CAN1_SendFrameExt (component FreescaleCAN)
**
**     Description :
**         Sends a frame. This method automatically selects a free
**         transmit buffer for data transmission. The user cannot
**         specify a transmit buffer.
**     Parameters  :
**         NAME            - DESCRIPTION
**         MessageID       - Identification of the
**                           message - ID. Message ID can be
**                           specified in the STANDARD format
**                           (default) or the EXTENDED format. The
**                           most significant bit in the ID is set to
**                           specify EXTENDED format. Predefined
**                           macro CAN_EXTENDED_FRAME_ID can be used
**                           (ID "bitwise or" CAN_EXTENDED_FRAME_ID)
**                           to mark ID as extended. If the most
**                           significant bit of ID is clear, STANDARD
**                           format is used.
**         FrameType       - Type of frame
**                           DATA_FRAME - data frame
**                           REMOTE_FRAME - remote frame
**         Length          - The length of the frame in bytes
**                           (0..8)
**       * Data            - Pointer to data
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_DISABLED - This component is disabled by
**                           user
**                           ERR_VALUE - Some parameter is out of
**                           possible range
** ===================================================================
*/
byte CAN1_SendFrameExt(dword MessageID,byte FrameType,byte Length,const byte *Data)
{
  byte i;                              /* Temorary variables */
  TMsgBuff *MsgBuff;
  dword tmpId;

  if (((MessageID & CAN_EXTENDED_FRAME_ID) == 0U) && (MessageID > CAN_STANDARD_FRAME_MAX_ID)) { /* Is the standard ID greater that 2047? */
    return ERR_VALUE;                  /* If yes then error */
  }
  if (((MessageID & CAN_EXTENDED_FRAME_ID) == CAN_EXTENDED_FRAME_ID) && ((MessageID & CAN_EXTENDED_FRAME_MASK) != 0U)) { /* Is the extended ID greater that 536870911? */
    return ERR_VALUE;                  /* If yes then error */
  }
  if (Length > CAN_MAX_DATA_LEN) {     /* Is the message length greater than 8?*/
    return ERR_VALUE;                  /* If yes then error */
  }
  if (FrameType > REMOTE_FRAME) {      /* Is FrameType other than REMOTE_FRAME or DATA_FRAME */
    return ERR_VALUE;                  /* If yes then error */
  }
  EnterCritical();                     /* Disable global interrupts */
  if ((CANTFLG & 0x07U) == 0U) {       /* Are all transmit buffers full? */
    ExitCritical();                    /* Enable global interrupts */
    return ERR_TXFULL;                 /* If yes then error */
  }
  CANTBSEL = CANTFLG;                  /* Find any empty transmit buffer */
  /*lint -save  -e926 -e927 -e928 -e929 Disable MISRA rule (11.4) checking. */
  MsgBuff = (TMsgBuff *)&CANTXIDR0;
  /*lint -restore Enable MISRA rule (11.4) checking. */
  CANTIER_TXEIE = CANTBSEL;            /* Enable transmit of the selected TX buffer */
  if (MessageID & CAN_EXTENDED_FRAME_ID) {
    tmpId = (((MessageID & 0x1FFC0000UL) << 3) | 0x00180000UL | ((MessageID & 0x0003FFFFUL) << 1)); /* Extended frame */
  }
  else {
    tmpId = MessageID << 21;           /* Standard frame */
  }
  /*lint -save  -e926 -e927 -e928 -e929 Disable MISRA rule (11.4) checking. */
  MsgBuff->IDR3 = ((DwordSwap *)&tmpId)->b.b3; /* Store the message ID */
  MsgBuff->IDR2 = ((DwordSwap *)&tmpId)->b.b2;
  MsgBuff->IDR1 = ((DwordSwap *)&tmpId)->b.b1;
  MsgBuff->IDR0 = ((DwordSwap *)&tmpId)->b.b0;
  /*lint -restore Enable MISRA rule (11.4) checking. */
  if (FrameType == DATA_FRAME) {       /* Is it a data frame? */
    for (i=0U; i<Length; i++) {
      MsgBuff->Data[i] = Data[i];      /* Store data to the transmit register */
    }
    if (MessageID & CAN_EXTENDED_FRAME_ID) { /* Is it the extended frame? */
      MsgBuff->IDR3 &= 0xFEU;          /* If no then set message type as "data frame" */
    }
    else {
      MsgBuff->IDR1 &= 0xEFU;          /* If yes then set message type as "data frame" */
    }
  }
  else {                               /* Remote frame */
    if (MessageID & CAN_EXTENDED_FRAME_ID) { /* Is it the extended frame? */
      MsgBuff->IDR3 |= 0x01U;          /* If yes then set message type as "remote frame" */
    }
    else {
      MsgBuff->IDR1 |= 0x10U;          /* If yes then set message type as "remote frame" */
    }
  }
  CANTXDLR = Length;                   /* Set the length of the message */
  CANTXTBPR = 0x00U;                   /* Set the priority (high) */
  CANTFLG = CANTBSEL;                  /* Start transmission */
  ExitCritical();                      /* Enable global interrupts */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  CAN1_ReadFrame (component FreescaleCAN)
**
**     Description :
**         Reads a frame from the CAN device. The user is informed
**         about CAN reception through OnFullRxBuffer event or
**         GetStateRX method.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * MessageID       - Pointer to a message
**                           indentification
**       * FrameType       - Pointer to a frame type
**                           DATA_FRAME - data frame
**                           REMOTE_FRAME - remote frame
**       * FrameFormat     - Pointer to a frame
**                           format
**                           STANDARD_FORMAT - standard frame 11-bits
**                           EXTENDED_FORMAT - extended frame 29-bits.
**                           Note: This parameter is obsolete and
**                           will be removed in future releases.
**       * Length          - Pointer to a length of the frame
**       * Data            - The buffer for received data
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_DISABLED - This component is disabled by
**                           user
**                           ERR_RXEMPTY - The receive buffer is
**                           empty
**                           ERR_OVERRUN - The previous (unread)
**                           message in the receive buffer was
**                           overwriten by a new message. Returns
**                           only if Interrupt service is enabled.
** ===================================================================
*/
byte CAN1_ReadFrame(dword *MessageID,byte *FrameType,byte *FrameFormat,byte *Length,byte *Data)
{
  byte i;
  dword tmpId = 0U;

  if ((SerFlag & FULL_RX_BUF) == 0U) { /* Is the receive buffer empty? */
    return ERR_RXEMPTY;                /* If yes then error */
  }
  /*lint -save  -e926 -e927 -e928 -e929 Disable MISRA rule (11.4) checking. */
  ((DwordSwap*)&tmpId)->b.b0 = CANRXIDR0;
  ((DwordSwap*)&tmpId)->b.b1 = CANRXIDR1;
  ((DwordSwap*)&tmpId)->b.b2 = CANRXIDR2;
  ((DwordSwap*)&tmpId)->b.b3 = CANRXIDR3;
  /*lint -restore Enable MISRA rule (11.4) checking. */
  if (tmpId & MB_ID_IDE) {
    *MessageID = ((tmpId >> 1) & 0x3FFFFUL) | ((tmpId >> 3) & 0x1FFC0000UL) | CAN_EXTENDED_FRAME_ID; /* Extended frame */
  }
  else {
    *MessageID = tmpId >> 21;          /* Standard frame */
  }
  if (*MessageID & CAN_EXTENDED_FRAME_ID) {
    *FrameFormat = EXTENDED_FORMAT;
    *FrameType = (CANRXIDR3 & 0x01U)? (byte)REMOTE_FRAME : (byte)DATA_FRAME; /* Result the frame type */
    *MessageID &= ~CAN_EXTENDED_FRAME_ID; /* Remove EXTENDED_FRAME indicator, frame type will be returned in FrameType parameter */
  }
  else {
    *FrameFormat = STANDARD_FORMAT;
    *FrameType = (CANRXIDR1 & 0x10U)? (byte)REMOTE_FRAME : (byte)DATA_FRAME; /* Result the frame type */
  }
  *Length = CANRXDLR & 0x0FU;          /* Result length of the message */
  if (*FrameType == DATA_FRAME) {      /* Is it "data frame"? */
    for (i=0U; i<*Length; i++) {
      /*lint -save  -e926 -e927 -e928 -e929 Disable MISRA rule (11.4) checking. */
      Data[i] = *((byte *)&CANRXDSR0 + i); /* Return received data */
      /*lint -restore Enable MISRA rule (11.4) checking. */
    }
  }
  SerFlag &= (byte)(~(byte)FULL_RX_BUF); /* Clear flag "full RX buffer" */
  if (SerFlag & CANRFLG_OVRIF_MASK) {  /* Is the overrun detected? */
    SerFlag &= (byte)(~(byte)CANRFLG_OVRIF_MASK); /* Clear the internal overrun flag */
    return ERR_OVERRUN;                /* If yes then error */
  }
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  CAN1_Init (component FreescaleCAN)
**
**     Description :
**         Initializes the associated peripheral(s) and the components 
**         internal variables. The method is called automatically as a 
**         part of the application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void CAN1_Init(void)
{
  /* CANCTL1: CANE=1,CLKSRC=0,LOOPB=0,LISTEN=0,??=0,WUPM=0,SLPAK=0,INITAK=0 */
  CANCTL1 = 0x80U;                     /* Set the control register */
  /* CANCTL0: RXFRM=0,RXACT=0,CSWAI=0,SYNCH=0,TIME=0,WUPE=0,SLPRQ=0,INITRQ=1 */
  CANCTL0 = 0x01U;                     /* Set the control register */
  CANIDAC_IDAM = 0x00U;                /* Set the acceptance mode */
  CANIDAR0 = 0x00U;                    /* Set the acceptance code, register CANIDAR0 */
  CANIDAR1 = 0x00U;                    /* Set the acceptance code, register CANIDAR1 */
  CANIDAR2 = 0x00U;                    /* Set the acceptance code, register CANIDAR2 */
  CANIDAR3 = 0x00U;                    /* Set the acceptance code, register CANIDAR3 */
  CANIDAR4 = 0x00U;                    /* Set the acceptance code, register CANIDAR4 */
  CANIDAR5 = 0x00U;                    /* Set the acceptance code, register CANIDAR5 */
  CANIDAR6 = 0x00U;                    /* Set the acceptance code, register CANIDAR6 */
  CANIDAR7 = 0x00U;                    /* Set the acceptance code, register CANIDAR7 */
  CANIDMR0 = 0xFFU;                    /* Set the acceptance mask, register CANIDMR0 */
  CANIDMR1 = 0xFFU;                    /* Set the acceptance mask, register CANIDMR1 */
  CANIDMR2 = 0xFFU;                    /* Set the acceptance mask, register CANIDMR2 */
  CANIDMR3 = 0xFFU;                    /* Set the acceptance mask, register CANIDMR3 */
  CANIDMR4 = 0xFFU;                    /* Set the acceptance mask, register CANIDMR4 */
  CANIDMR5 = 0xFFU;                    /* Set the acceptance mask, register CANIDMR5 */
  CANIDMR6 = 0xFFU;                    /* Set the acceptance mask, register CANIDMR6 */
  CANIDMR7 = 0xFFU;                    /* Set the acceptance mask, register CANIDMR7 */
  /* CANBTR0: SJW1=0,SJW0=1,BRP5=0,BRP4=0,BRP3=0,BRP2=0,BRP1=1,BRP0=0 */
  CANBTR0 = 0x42U;                     /* Set the device timing register */
  /* CANBTR1: SAMP=0,TSEG22=1,TSEG21=1,TSEG20=1,TSEG13=0,TSEG12=1,TSEG11=1,TSEG10=0 */
  CANBTR1 = 0x76U;                     /* Set the device timing register */
  CANCTL1_CLKSRC = 0x00U;              /* Select the clock source from crystal */
  CANCTL0_INITRQ = 0x00U;              /* Start device */
  while(CANCTL1_INITAK) {}             /* Wait for enable */
  /* CANRFLG: WUPIF=1,CSCIF=1,RSTAT1=1,RSTAT0=1,TSTAT1=1,TSTAT0=1,OVRIF=1 */
  CANRFLG |= 0xFEU;                    /* Reset error flags */
  /* CANRIER: WUPIE=0,CSCIE=1,RSTATE1=1,RSTATE0=1,TSTATE1=1,TSTATE0=1,OVRIE=0,RXFIE=1 */
  CANRIER = 0x7DU;                     /* Enable interrupts */
}

/*
** ===================================================================
**     Method      :  CAN1_GetStateTX (component FreescaleCAN)
**
**     Description :
**         Returns a value of the transmission complete flags.
**     Parameters  : None
**     Returns     :
**         ---             - Content of the transmitter complete
**                           flag register.
** ===================================================================
*/
/*
byte CAN1_GetStateTX(void)

**      This method is implemented as a macro in the header module. **
*/

/*
** ===================================================================
**     Method      :  CAN1_SetAcceptanceMask (component FreescaleCAN)
**
**     Description :
**         Sets the acceptance mask registers. This method writes an
**         acceptance mask directly to the acceptance mask registers.
**     Parameters  :
**         NAME            - DESCRIPTION
**         AccMask1        - Acceptance mask for the
**                           message filtering. This acceptance mask
**                           will be written to the acceptance mask
**                           registers IDMR0-IDMR3. The most
**                           significant byte of the acceptance mask
**                           will be written to the IDMR0 register
**                           and the least significant byte of the
**                           acceptance mask will be written to the
**                           IDMR3 register.
**         AccMask2        - Acceptance mask for the
**                           message filtering. This acceptance mask
**                           will be written to the acceptance mask
**                           registers IDMR4-IDMR7. The most
**                           significant byte of the acceptance mask
**                           will be written to the IDMR4 register
**                           and the least significant byte of the
**                           acceptance mask will be written to the
**                           IDMR7 register.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_DISABLED - This component is disabled by
**                           user
** ===================================================================
*/
byte CAN1_SetAcceptanceMask(dword AccMask1, dword AccMask2)
{
  EnterCritical();                     /* Enter critical section */
  CANCTL0_INITRQ = 0x01U;              /* Disable device */
  while(CANCTL1_INITAK == 0U){}        /* Wait for disable */
  /*lint -save  -e926 -e927 -e928 -e929 Disable MISRA rule (11.4) checking. */
  CANIDMR3 = ((DwordSwap *)&AccMask1)->b.b3; /* Set acceptance mask, register CANIDMR3 */
  CANIDMR2 = ((DwordSwap *)&AccMask1)->b.b2; /* Set acceptance mask, register CANIDMR2 */
  CANIDMR1 = ((DwordSwap *)&AccMask1)->b.b1; /* Set acceptance mask, register CANIDMR1 */
  CANIDMR0 = ((DwordSwap *)&AccMask1)->b.b0; /* Set acceptance mask, register CANIDMR0 */
  CANIDMR7 = ((DwordSwap *)&AccMask2)->b.b3; /* Set acceptance mask, register CANIDMR7 */
  CANIDMR6 = ((DwordSwap *)&AccMask2)->b.b2; /* Set acceptance mask, register CANIDMR6 */
  CANIDMR5 = ((DwordSwap *)&AccMask2)->b.b1; /* Set acceptance mask, register CANIDMR5 */
  CANIDMR4 = ((DwordSwap *)&AccMask2)->b.b0; /* Set acceptance mask, register CANIDMR4 */
  /*lint -restore Enable MISRA rule (11.4) checking. */
  CANCTL0_INITRQ = 0x00U;              /* Start device */
  while(CANCTL1_INITAK) {}             /* Wait for device initialization acknowledge */
  /* CANRFLG: WUPIF=1,CSCIF=1,OVRIF=1 */
  CANRFLG |= 0xC2U;                    /* Reset error flags */
  /* CANRIER: WUPIE=0,CSCIE=1,RSTATE1=1,RSTATE0=1,TSTATE1=1,TSTATE0=1,OVRIE=0,RXFIE=1 */
  CANRIER = 0x7DU;                     /* Enable interrupts */
  ExitCritical();                      /* Exit critical section */
  return ERR_OK;                       /* OK */
}


/*
** ===================================================================
**     Method      :  CAN1_InterruptTx (component FreescaleCAN)
**
**     Description :
**         The method services the transmit interrupt of the selected 
**         peripheral(s) and eventually invokes the components event(s).
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
#pragma CODE_SEG __NEAR_SEG NON_BANKED
ISR(CAN1_InterruptTx)
{
  byte buffer = CANTFLG;               /* Temporary variable */
  buffer = (buffer & CANTIER) & 7U;

  CANTIER &= (byte)~buffer;            /* Clear appropriate transmit flags and release TX buffer */
  CAN1_OnFreeTxBuffer((word)buffer);   /* If yes then invoke user event */
}

#pragma CODE_SEG CAN1_CODE
/*
** ===================================================================
**     Method      :  CAN1_InterruptRx (component FreescaleCAN)
**
**     Description :
**         The method services the receive interrupt of the selected 
**         peripheral(s) and eventually invokes the components event(s).
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
#pragma CODE_SEG __NEAR_SEG NON_BANKED
#define ON_OVERRUN  1U
ISR(CAN1_InterruptRx)
{
  byte buffer;

  if (SerFlag & FULL_RX_BUF) {         /* Is any char already present in the receive buffer? */
    SerFlag |= CAN_STATUS_OVERRUN_MASK; /* If yes then set internal flag OVERRUN */
  }
  SerFlag |= FULL_RX_BUF;              /* Set flag "full RX buffer" */
  buffer = SerFlag & 0x83U;
  ErrFlag |= buffer;                   /* Add new error flags into the ErrorFlag status variable */
   __DI();                             /* Disable maskable interrupts */
  CAN1_OnFullRxBuffer();               /* If yes then invoke user event. Parameter is always 1 because the CAN module has only one RX message buffer */
  CANRFLG = CANRFLG_RXF_MASK;          /* Reset the reception complete flag and release the RX buffer */
}

#pragma CODE_SEG CAN1_CODE
/*
** ===================================================================
**     Method      :  CAN1_InterruptError (component FreescaleCAN)
**
**     Description :
**         The method services the error interrupt of the selected 
**         peripheral(s) and eventually invokes the components event(s).
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
#pragma CODE_SEG __NEAR_SEG NON_BANKED
ISR(CAN1_InterruptError)
{
  byte Status = CANRFLG;               /* Read the status register */

  if((Status & CAN_STATUS_BOFF_MASK) == CAN_STATUS_BOFF_MASK) { /* Bus-Off state */
    ErrFlag |= CAN_STATUS_BOFF_EXT_MASK;
    ErrFlag |= (Status & 0x83U);       /* Add error flags */
  }
  else {
    ErrFlag |= (Status & 0xBFU);       /* Add error flags */
  }
  CANRFLG = 0xFEU;                     /* Reset error flags */
  if ((Status & CAN_STATUS_BOFF_MASK) == CAN_STATUS_BOFF_MASK) { /* Is busoff error detected? */
    CAN1_OnBusOff();                   /* If yes then invoke user event */
  } else {
  }
}

#pragma CODE_SEG CAN1_CODE

/* END CAN1. */
/*
** ###################################################################
**
**     This file was created by Processor Expert 3.02 [04.44]
**     for the Freescale HCS12 series of microcontrollers.
**
** ###################################################################
*/
