/** ###################################################################
**     Filename  : Events.h
**     Project   : Project_4
**     Processor : MC9S12C32CFU16
**     Component : Events
**     Version   : Driver 01.04
**     Compiler  : CodeWarrior HC12 C Compiler
**     Date/Time : 08.05.2015, 08:42
**     Abstract  :
**         This is user's event module.
**         Put your event handler code here.
**     Settings  :
**     Contents  :
**         EI2C1_OnRxChar - void EI2C1_OnRxChar(void);
**         EI2C1_OnTxChar - void EI2C1_OnTxChar(void);
**         EI2C1_OnNACK   - void EI2C1_OnNACK(void);
**
** ###################################################################*/

#ifndef __Events_H
#define __Events_H
/* MODULE Events */

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "PE_Timer.h"
#include "EI2C1.h"
#include "Inhr1.h"
#include "Inhr2.h"
#include "CAN1.h"
#include "AD1.h"
#include "AS1.h"
#include "PWM_SUSPENSION_UP.h"
#include "PWM_SUSPENSION_DOWN.h"
#include "PWM_STEERING_LEFT.h"
#include "PWM_STEERING_RIGHT.h"
#include "PWM_WHEEL_FORWARDS.h"
#include "PWM_WHEEL_BACKWARDS.h"
#include "BOARD_ID.h"
#include "LED.h"

#pragma CODE_SEG DEFAULT

void EI2C1_OnRxChar(void);
/*
** ===================================================================
**     Event       :  EI2C1_OnRxChar (module Events)
**
**     Component   :  EI2C1 [SW_I2C]
**     Description :
**         Called when a correct character is received. In the SLAVE
**         mode, this event is not called if the component receives the
**         first byte with slave address and R/W bit.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void EI2C1_OnTxChar(void);
/*
** ===================================================================
**     Event       :  EI2C1_OnTxChar (module Events)
**
**     Component   :  EI2C1 [SW_I2C]
**     Description :
**         Called when a correct character is sent. In MASTER mode,
**         this event is not called if the component sends the first
**         byte with slave address and R/W bit.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void EI2C1_OnNACK(void);
/*
** ===================================================================
**     Event       :  EI2C1_OnNACK (module Events)
**
**     Component   :  EI2C1 [SW_I2C]
**     Description :
**         In the MASTER mode, this event is called when an invalid
**         slaves acknowledgement occurs during communication transfer.
**         If the acknowledge polling is provided (MASTER mode only), i.
**         e., the <Acknowledge polling trials> property value is
**         higher than one, this event is called only when no trial is
**         successful. In the SLAVE mode, this event is called when a
**         master sends an acknowledgement instead of no
**         acknowledgement at the end of the last byte transfer.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/


void CAN1_OnFreeTxBuffer(word BufferMask);
/*
** ===================================================================
**     Event       :  CAN1_OnFreeTxBuffer (module Events)
**
**     Component   :  CAN1 [FreescaleCAN]
**     Description :
**         This event is called after a successful transmission of a
**         message. The event is available only if Interrupt
**         service/event is enabled.
**     Parameters  :
**         NAME            - DESCRIPTION
**         BufferMask      - Transmit buffer mask. The
**                           mask can be used to check what message
**                           buffer caused the transmit interrupt.
**     Returns     : Nothing
** ===================================================================
*/

void CAN1_OnFullRxBuffer(void);
/*
** ===================================================================
**     Event       :  CAN1_OnFullRxBuffer (module Events)
**
**     Component   :  CAN1 [FreescaleCAN]
**     Description :
**         This event is called when the receive buffer is full
**         after a successful reception of a message. The event is
**         available only if Interrupt service/event is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void CAN1_OnBusOff(void);
/*
** ===================================================================
**     Event       :  CAN1_OnBusOff (module Events)
**
**     Component   :  CAN1 [FreescaleCAN]
**     Description :
**         This event is called when the node status becomes bus-off.
**         The event is available only if Interrupt service/event is
**         enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void AD1_OnEnd(void);
/*
** ===================================================================
**     Event       :  AD1_OnEnd (module Events)
**
**     Component   :  AD1 [ADC]
**     Description :
**         This event is called after the measurement (which consists
**         of <1 or more conversions>) is/are finished.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void AS1_OnError(void);
/*
** ===================================================================
**     Event       :  AS1_OnError (module Events)
**
**     Component   :  AS1 [AsynchroSerial]
**     Description :
**         This event is called when a channel error (not the error
**         returned by a given method) occurs. The errors can be read
**         using <GetError> method.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void AS1_OnRxChar(void);
/*
** ===================================================================
**     Event       :  AS1_OnRxChar (module Events)
**
**     Component   :  AS1 [AsynchroSerial]
**     Description :
**         This event is called after a correct character is received.
**         The event is available only when the <Interrupt
**         service/event> property is enabled and either the <Receiver>
**         property is enabled or the <SCI output mode> property (if
**         supported) is set to Single-wire mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void AS1_OnTxChar(void);
/*
** ===================================================================
**     Event       :  AS1_OnTxChar (module Events)
**
**     Component   :  AS1 [AsynchroSerial]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void AS1_OnFullRxBuf(void);
/*
** ===================================================================
**     Event       :  AS1_OnFullRxBuf (module Events)
**
**     Component   :  AS1 [AsynchroSerial]
**     Description :
**         This event is called when the input buffer is full;
**         i.e. after reception of the last character 
**         that was successfully placed into input buffer.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void AS1_OnFreeTxBuf(void);
/*
** ===================================================================
**     Event       :  AS1_OnFreeTxBuf (module Events)
**
**     Component   :  AS1 [AsynchroSerial]
**     Description :
**         This event is called after the last character in output
**         buffer is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void TI2_OnInterrupt(void);
/*
** ===================================================================
**     Event       :  TI2_OnInterrupt (module Events)
**
**     Component   :  TI2 [TimerInt]
**     Description :
**         When a timer interrupt occurs this event is called (only
**         when the component is enabled - <Enable> and the events are
**         enabled - <EnableEvent>). This event is enabled only if a
**         <interrupt service/event> is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void CAN1_OnError(void);
/*
** ===================================================================
**     Event       :  CAN1_OnError (module Events)
**
**     Component   :  CAN1 [FreescaleCAN]
**     Description :
**         This event is called when a channel error (not the error
**         returned by a given method) occurs. These errors can be
**         read using <GetError> method. The event is available only
**         if Interrupt service/event is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void CAN1_OnReceiverErrorPassive(void);
/*
** ===================================================================
**     Event       :  CAN1_OnReceiverErrorPassive (module Events)
**
**     Component   :  CAN1 [FreescaleCAN]
**     Description :
**         This event is called when the CAN controller goes into
**         error passive status due to the receive error counter
**         exceeding 127 and the BusOff status is not present. The
**         event is available only if Interrupt service/event is
**         enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void CAN1_OnOverrun(void);
/*
** ===================================================================
**     Event       :  CAN1_OnOverrun (module Events)
**
**     Component   :  CAN1 [FreescaleCAN]
**     Description :
**         This event is called when receive buffer has overrun. The
**         event is available only if Interrupt service/event is
**         enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void CAN1_OnReceiverWarning(void);
/*
** ===================================================================
**     Event       :  CAN1_OnReceiverWarning (module Events)
**
**     Component   :  CAN1 [FreescaleCAN]
**     Description :
**         This event is called when the CAN controller goes into a
**         warning status due to the receive error counter exceeding
**         96 and neither an error status nor a BusOff status are
**         present. The event is available only if Interrupt
**         service/event is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

/* END Events */
#endif /* __Events_H*/

/*
** ###################################################################
**
**     This file was created by Processor Expert 3.02 [04.44]
**     for the Freescale HCS12 series of microcontrollers.
**
** ###################################################################
*/
