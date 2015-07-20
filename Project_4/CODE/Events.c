/** ###################################################################
**     Filename  : Events.c
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
/* MODULE Events */


#include "Cpu.h"
#include "Events.h"

/* User includes (#include below this line is not maintained by Processor Expert) */
#include "uart_interface.h"
#include "can_interface.h"
#include "message.h"
#include "sensor_interface.h"
#include "fifo.h"
#include "comm.h"

int16_t Values[8];
int16_t i16CurrentADC;
int16_t i16SteeringADC;
int16_t i16Suspension1ADC;
int16_t i16Suspension2ADC;   
int16_t i16CurrentSetPoint;
int16_t i16SteeringSetPoint;
int16_t i16Suspension1SetPoint;
int16_t i16Suspension2SetPoint;  

#pragma CODE_SEG DEFAULT

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
void EI2C1_OnRxChar(void)
{
  /* Write your code here ... */
}

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
void EI2C1_OnTxChar(void)
{
  /* Write your code here ... */
}

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
void EI2C1_OnNACK(void)
{
  /* Write your code here ... */
}

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
void CAN1_OnFreeTxBuffer(word BufferMask)
{
  static char cont = 0x00;
  cont += 1;
  /* Write your code here ... */
}

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
void CAN1_OnFullRxBuffer(void)
{
  /* Write your code here ... */
  uint8_t loc_totalBytesRead = 0U;
	uint32_t loc_messageId = 0U;
	uint8_t loc_frameType = 0U;
	uint8_t loc_frameFormat = 0U;  
	uint8_t loc_buffer[8];  
	uint8_t loc_numMsgs = 4U;
	uint8_t i;
	
	static uint32_t loc_counterSteering = 0U;
	static uint32_t loc_counterWheel = 0U;
	static uint32_t loc_counterSuspension = 0U;
	
	tMessageBody loc_messageReceivedBody;
	tMessageSteeringData loc_steeringData;
	
	LED_PutVal(1);
	
	//for (i = 0U; i < loc_numMsgs; i++) 
	{
	  CAN1_ReadFrame(&loc_messageId, &loc_frameType, &loc_frameFormat, &loc_totalBytesRead, loc_buffer);
    
    if (loc_totalBytesRead == 8U) 
    {
      uint8_t j;
      fifo_in(&can_fifo, (uint8_t)loc_messageId);
      
      for (j = 0U; j < loc_totalBytesRead; j++) 
      {
        fifo_in(&can_fifo, loc_buffer[j]);
      }
      
      //if (loc_messageId == E_MSG_ID_STEERING) loc_counterSteering++;
      //if (loc_messageId == E_MSG_ID_WHEEL) loc_counterWheel++;
      //if (loc_messageId == E_MSG_ID_SUSP) loc_counterSuspension++;
      
      if (loc_messageId == E_MSG_ID_STEERING) 
      {
        //loc_messageReceivedBody[0] = loc_buffer[0];
        //memcpy(&loc_messageReceivedBody, loc_buffer, 9);
        loc_steeringData.steering[0] = (int16_t)loc_buffer[1];
        loc_steeringData.steering[1] = (int16_t)loc_buffer[3];
        loc_steeringData.steering[2] = (int16_t)loc_buffer[5];
        loc_steeringData.steering[3] = (int16_t)loc_buffer[7];
        
        //msgSteering
        
        //memcpy(&loc_steeringData, loc_messageReceivedBody.data.rawData, sizeof(tMessageSteeringData));
        memcpy(msgSteering.body.data.rawData, &loc_steeringData, sizeof(tMessageSteeringData));
        //comm_setData(E_MSG_ID_STEERING, &loc_steeringData);
      }
  
      can_setDataAvailable();
    }  
	}
  LED_PutVal(0);
}

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
void CAN1_OnBusOff(void)
{
  /* Write your code here ... */
}

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
void AD1_OnEnd(void)
{
  /* Write your code here ... */
  //AD1_GetChanValue8(E_SENSOR_CURRENT, &sensor[E_SENSOR_CURRENT]);
	//AD1_GetChanValue8(E_SENSOR_STEERING, &sensor[E_SENSOR_STEERING]);
	//AD1_GetChanValue8(E_SENSOR_SUSPENSION_SPRING, &sensor[E_SENSOR_SUSPENSION_SPRING]);
	//AD1_GetChanValue8(E_SENSOR_SUSPENSION_JOINT, &sensor[E_SENSOR_SUSPENSION_JOINT]);
}

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
void AS1_OnError(void)
{
  /* Write your code here ... */
}

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
void AS1_OnRxChar(void)
{
  /* Write your code here ... */
  
  uint8_t loc_char;
  
  //while (AS1_GetCharsInRxBuf() > 0) 
  {    
    AS1_RecvChar(&loc_char);  
    fifo_in(&uart_fifo, loc_char);
  }
  
  //if (AS1_GetCharsInRxBuf() >= 1*sizeof(tMessageBody)) 
  //{
     //uart_setDataAvailable();
  //}  
}

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
void AS1_OnTxChar(void)
{
  /* Write your code here ... */
}

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
void AS1_OnFullRxBuf(void)
{
  /* Write your code here ... */
  //uart_setDataAvailable();
}

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
void AS1_OnFreeTxBuf(void)
{
  /* Write your code here ... */
}

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
void TI2_OnInterrupt(void)
{
  /* Write your code here ... */
  char message[] = "Fodasse ";
  uint8_t snd;
  //AS1_SendBlock(message, 8, &snd);
}

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
void CAN1_OnError(void)
{
  /* Write your code here ... */
}

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
void CAN1_OnReceiverErrorPassive(void)
{
  /* Write your code here ... */
}

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
void CAN1_OnOverrun(void)
{
  /* Write your code here ... */
}

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
void CAN1_OnReceiverWarning(void)
{
  /* Write your code here ... */
}

/* END Events */

/*
** ###################################################################
**
**     This file was created by Processor Expert 3.02 [04.44]
**     for the Freescale HCS12 series of microcontrollers.
**
** ###################################################################
*/
