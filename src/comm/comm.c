/*The MIT License (MIT)

Copyright (c) 2015 Marcelo Chimentao, Leandro Piekarski do Nascimento

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#ifndef COMM_H
	#include "comm.h"
#endif

#ifndef CRC_H
	#include "../crc/crc.h"
#endif

//#ifndef UART_INTERFACE_H
//	#include "../bus_interface/uart_interface.h"
//#endif

#ifndef BUS_INTERFACE_H
#include "../bus_interface/bus_interface.h"
#endif

#ifndef HANDSHAKE_H
	#include "../handshake/handshake.h"
#endif

#if !defined(WIN32)
	#ifndef __Cpu
		#include "Cpu.h"
	#endif
#endif

#include <string.h>

#include "../bus_interface/can_interface.h"

static void comm_initTransmissionCyclicMessages(void);
static void comm_cyclicReception(void);
static void comm_cyclicTransmission(void);
static void comm_receiveMessagesFromBus(const eMessageBus arg_busType);
static const eCommStatus comm_readMessageFromBus(const eMessageBus arg_busType, tMessage * const arg_message);
static const eCommStatus comm_writeMessageToBus(const tMessage * const arg_message);
static void comm_transmitMessage(tMessage * const arg_message);
static const eCommStatus comm_checkMessageId(const uint8_t arg_messageId);
static void comm_clearAcks(void);

tMessage msgControl =
{
	{
		E_MSG_ID_CONTROL,
		{{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU}},
		0xFFU
	},
	E_MSG_STATE_INIT,
	COMM_TIMEOUT,
	COMM_MAXRETRANSMISSIONS,
	E_MSG_STATUS_INACTIVE,
	E_COMM_STATUS_FAILED,
	E_MSG_TYPE_CYCLIC,
	E_MSG_BUS_UART,
	E_MSG_ACK_ACTIVE,
	E_MSG_CRC_ACTIVE
};

tMessage msgCurrent =
{
	{
		E_MSG_ID_CURRENT,
		{{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU}},
		0xFFU
	},
	E_MSG_STATE_INIT,
	COMM_TIMEOUT,
	COMM_MAXRETRANSMISSIONS,
	E_MSG_STATUS_INACTIVE,
	E_COMM_STATUS_FAILED,
	E_MSG_TYPE_CYCLIC,
	E_MSG_BUS_CAN,
	E_MSG_ACK_ACTIVE,
	E_MSG_CRC_INACTIVE
};

tMessage msgSuspension =
{
	{
		E_MSG_ID_SUSP,
		{{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU}},
		0xFFU
	},
	E_MSG_STATE_INIT,
	COMM_TIMEOUT,
	COMM_MAXRETRANSMISSIONS,
	E_MSG_STATUS_INACTIVE,
	E_COMM_STATUS_FAILED,
	E_MSG_TYPE_CYCLIC,
	E_MSG_BUS_CAN,
	E_MSG_ACK_INACTIVE,
	E_MSG_CRC_INACTIVE
};

tMessage msgSteering =
{
	{
		E_MSG_ID_STEERING,
		{{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU}},
		0xFFU
	},
	E_MSG_STATE_INIT,
	COMM_TIMEOUT,
	COMM_MAXRETRANSMISSIONS,
	E_MSG_STATUS_INACTIVE,
	E_COMM_STATUS_FAILED,
	E_MSG_TYPE_CYCLIC,
	E_MSG_BUS_CAN,
	E_MSG_ACK_INACTIVE,
	E_MSG_CRC_INACTIVE
};

tMessage msgWheel =
{
	{
		E_MSG_ID_WHEEL,
		{{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU}},
		0xFFU
	},
	E_MSG_STATE_INIT,
	COMM_TIMEOUT,
	COMM_MAXRETRANSMISSIONS,
	E_MSG_STATUS_INACTIVE,
	E_COMM_STATUS_FAILED,
	E_MSG_TYPE_CYCLIC,
	E_MSG_BUS_CAN,
	E_MSG_ACK_INACTIVE,
	E_MSG_CRC_INACTIVE
};

static tMessage * transmitMessages[] =
{
#if NODE==CONTROL
	&msgControl,
#elif NODE==MASTER
	&msgWheel,
//	&msgCurrent,
	&msgSteering,
//	&msgSuspension,
#elif NODE==SLAVE1
#endif
	NULL
};

static tMessage * receiveMessages[] =
{
#if NODE!=CONTROL
#if NODE==MASTER
	&msgControl,
#else //NODE==SLAVE1...4
	&msgSteering,
	&msgWheel,
	&msgCurrent,
	&msgSuspension,
#endif
#endif
	NULL
};

void comm_setData(eMessageId arg_id, const void * const arg_data, const uint8_t arg_size)
{
	tMessage ** pMessage = transmitMessages;

	while (*pMessage != NULL)
	{
		const eMessageId loc_id = (*pMessage)->body.messageId;
		if (loc_id == arg_id)
		{
			memcpy((*pMessage)->body.data.rawData, arg_data, arg_size);
			break;
		}
		pMessage++;
	}
}

void comm_getData(eMessageId arg_id, void * const arg_data, const uint8_t arg_size)
{
	tMessage ** pMessage = receiveMessages;

	while (*pMessage != NULL)
	{
		const eMessageId loc_id = (*pMessage)->body.messageId;
		if (loc_id == arg_id)
		{
			memcpy((void *)arg_data, (*pMessage)->body.data.rawData, arg_size);
			break;
		}
		pMessage++;
	}
}

void comm_init(void)
{
	comm_initTransmissionCyclicMessages();
}

void comm_end(void)
{

}

void comm_initTransmissionCyclicMessages(void)
{
	tMessage ** pMessage = transmitMessages;

	while (*pMessage != NULL)
	{
		const eMessageType loc_type = (*pMessage)->type;
		(*pMessage)->status = (loc_type == E_MSG_TYPE_CYCLIC) ? E_MSG_STATUS_ACTIVE : E_MSG_STATUS_INACTIVE;
		pMessage++;
	}
}

void comm_cyclic(void)
{
#if HANDSHAKE==ACTIVE
	if (handshake_getStatus() == E_HANDSHAKE_STATUS_OK)
#endif
	{
		comm_cyclicReception();
		comm_cyclicTransmission();
	}
}

void comm_cyclicReception(void)
{
	comm_clearAcks();
	comm_receiveMessagesFromBus(E_BUS_TYPE_UART);
	comm_receiveMessagesFromBus(E_BUS_TYPE_CAN);
}

void comm_cyclicTransmission(void)
{
	tMessage ** pMessage = transmitMessages;

	while (*pMessage != NULL)
	{
		comm_transmitMessage(*pMessage);
#if !defined(WIN32)
//		Cpu_Delay100US(20);
#endif
		pMessage++;
	}
}

void comm_transmitMessage(tMessage * const arg_message)
{
	const eMessageId loc_messageId = arg_message->body.messageId;
	const eMessageType loc_type = arg_message->type;
	const eMessageBus loc_bus = arg_message->bus;

	eMessageState loc_state = arg_message->state;
	uint8_t loc_timeout = arg_message->timeout;
	uint8_t loc_retransmissions = arg_message->retransmissions;
	eMessageStatus loc_messageStatus = arg_message->status;
	eMessageAck loc_ack = arg_message->ack;

	tMessage loc_message;
	uint8_t loc_crc;

	switch (loc_state)
	{
		case E_MSG_STATE_INIT:
		{
			/*Reset retransmissions*/
			loc_timeout = COMM_TIMEOUT;

			if (loc_messageStatus == E_MSG_STATUS_ACTIVE)
			{
				loc_state = E_MSG_STATE_TX_READY;
			}

			break;
		}
		case E_MSG_STATE_TX_READY:
		{
			loc_retransmissions = COMM_MAXRETRANSMISSIONS;
			loc_state = E_MSG_STATE_TRANSMIT;
			break;
		}
		case E_MSG_STATE_TRANSMIT:
		{
			eCommStatus loc_sts_comm = E_COMM_STATUS_FAILED;

			/*Reset timeout*/
			loc_timeout = COMM_TIMEOUT;

			/*Reset ack flag*/
			loc_ack = E_MSG_ACK_FAILED;

			/*Calculate crc*/
			loc_crc = crc8(arg_message->body.data.rawData, sizeof(arg_message->body.data.rawData));

			loc_message.body.messageId = loc_messageId;
			memcpy(loc_message.body.data.rawData, arg_message->body.data.rawData, sizeof(arg_message->body.data.rawData));
			loc_message.body.crc = loc_crc;

			/*Set bus type of the message*/
			loc_message.bus = loc_bus;

			/*Try to write message to bus*/
			loc_sts_comm = comm_writeMessageToBus(&loc_message);

			loc_state = (loc_sts_comm == E_COMM_STATUS_OK) ? E_MSG_STATE_WAITFORACK : E_MSG_STATE_TRANSMIT;
			break;
		}
		case E_MSG_STATE_WAITFORACK:
		{
			if (arg_message->ackCheck == E_MSG_ACK_ACTIVE)
			{
				if (loc_timeout > 0)
				{
					/*Verify if ACK was received*/
					if (loc_ack == E_MSG_ACK_OK)
					{
						/*If the message is cyclic it can be prepared to be transmitted once again*/
						if (loc_type == E_MSG_TYPE_CYCLIC)
						{
							loc_state = E_MSG_STATE_TRANSMIT;
						}
						/*Otherwise, the next transmission must be triggered*/
						else
						{
							loc_state = E_MSG_STATE_INIT;
							loc_messageStatus = E_MSG_STATUS_INACTIVE;
						}
					}

					loc_timeout--;
				}
				/*Timeout expired*/
				else
				{
					loc_state = (loc_retransmissions > 0U) ? E_MSG_STATE_TX_READY : E_MSG_STATE_END;
					loc_retransmissions--;
				}
			}
			else
			{
				loc_state = E_MSG_STATE_TRANSMIT;
			}

			break;
		}
		default:
		case E_MSG_STATE_END:
		{
			//TODO TRM Shutdown :)
//#if defined(WIN32)
			loc_state = E_MSG_STATE_INIT;
//#endif
			break;
		}
	}

	arg_message->state = loc_state;
	arg_message->timeout = loc_timeout;
	arg_message->retransmissions = loc_retransmissions;
	arg_message->status = loc_messageStatus;
	arg_message->ack = loc_ack;
}

void comm_receiveMessagesFromBus(const eMessageBus arg_busType)
{
	const eBusStatus loc_dataAvailable = bus_getDataAvailable(arg_busType);
	tMessage loc_receivedMessage;

	if (loc_dataAvailable == E_BUS_STATUS_OK)
	{
		while (comm_readMessageFromBus(arg_busType, &loc_receivedMessage) == E_COMM_STATUS_OK)
		{
			tMessageBody loc_receivedMessageBody = loc_receivedMessage.body;
			eMessageId loc_receivedMessageId = loc_receivedMessageBody.messageId;
			eCommStatus loc_checkId = comm_checkMessageId(loc_receivedMessageId);

			/*Check if message is transmitted/received by node*/
			if (loc_checkId == E_COMM_STATUS_OK)
			{
				/*Detect if the message received is a reply*/
				if ((loc_receivedMessageId & MSG_REPLYMASK) != 0U)
				{
					tMessage ** pMessage = transmitMessages;

					while (*pMessage != NULL)
					{
						const eMessageId loc_maskedMessageId = loc_receivedMessageId & MSG_IDMASK;
						if ((*pMessage)->body.messageId == loc_maskedMessageId)
						{
							if (loc_receivedMessageId & MSG_ACK)
							{
								(*pMessage)->ack = E_MSG_ACK_OK;
							}
							else if (loc_receivedMessageId & MSG_NACK)
							{
								(*pMessage)->ack = E_MSG_ACK_FAILED;
								(*pMessage)->timeout = 0U;
							}
							break;
						}
						pMessage++;
					}
				}
				else
				{
					tMessage ** pMessage = receiveMessages;

					while (*pMessage != NULL)
					{
						if ((*pMessage)->body.messageId == loc_receivedMessageId)
						{
							if((*pMessage)->ack == E_MSG_ACK_FAILED && (*pMessage)->ackCheck == E_MSG_ACK_ACTIVE)
							{
								if ((*pMessage)->crcCheck == E_MSG_CRC_ACTIVE)
								{
									const eCrcStatus loc_checkCRC = checkCRC(loc_receivedMessageBody.data.rawData,
																			 sizeof(uMessageData),
																			 loc_receivedMessageBody.crc);

									if (loc_checkCRC == E_CRC_STATUS_OK)
									{
										/* Send ack. Content of the message is not important */
										tMessage loc_message;
										loc_message.body.messageId = (loc_receivedMessageId | MSG_ACK);
										loc_message.bus = (*pMessage)->bus;
										comm_writeMessageToBus(&loc_message);

										/* Write valid values */
										(*pMessage)->body = loc_receivedMessageBody;
										(*pMessage)->ack = E_MSG_ACK_OK;
									}
									else
									{
										/* Send nack. Content of the message is not important */
										tMessage loc_message;
										loc_message.body.messageId = (loc_receivedMessageId | MSG_NACK);
										loc_message.bus = (*pMessage)->bus;
										comm_writeMessageToBus(&loc_message);
									}
								}
								else
								{
									tMessage loc_message;
									loc_message.body.messageId = (loc_receivedMessageId | MSG_ACK);
									loc_message.bus = (*pMessage)->bus;

									/* Write valid values */
									(*pMessage)->body = loc_receivedMessageBody;
									(*pMessage)->ack = E_MSG_ACK_OK;
								}
							}
							else
							{
								/* Write valid values */
								(*pMessage)->body = loc_receivedMessageBody;
								(*pMessage)->ack = E_MSG_ACK_OK;
							}

							break;
						}
						pMessage++;
					}
				}
			}
		}
		bus_clearDataAvailable(arg_busType);
	}
}

eCommStatus comm_checkCRC(const tMessageBody * const arg_messageBody)
{
	const uint8_t loc_receivedCRC = arg_messageBody->crc;
	const uint8_t loc_calculatedCRC = crc8(arg_messageBody->data.rawData, sizeof(uMessageData));
	const eCommStatus loc_result = (loc_receivedCRC == loc_calculatedCRC) ? E_COMM_STATUS_OK : E_COMM_STATUS_FAILED;

	return loc_result;
}

void comm_kickoutMessage(tMessage * const arg_message)
{
	const eMessageStatus loc_status = arg_message->status;
	arg_message->status = (loc_status == E_MSG_STATUS_INACTIVE) ? E_MSG_STATUS_ACTIVE : E_MSG_STATUS_INACTIVE;
}

const eCommStatus comm_checkMessageId(const uint8_t arg_messageId)
{
	eCommStatus loc_result = E_COMM_STATUS_FAILED;

	tMessage ** pMessage = receiveMessages;
	const uint8_t loc_maskedMessageId = arg_messageId & MSG_IDMASK;

	while (*pMessage != NULL)
	{
		if ((*pMessage)->body.messageId == loc_maskedMessageId)
		{
			loc_result = E_COMM_STATUS_OK;
			break;
		}

		pMessage++;
	}

	pMessage = transmitMessages;

	while (*pMessage != NULL)
	{
		if ((*pMessage)->body.messageId == loc_maskedMessageId)
		{
			loc_result = E_COMM_STATUS_OK;
			break;
		}

		pMessage++;
	}

	return loc_result;
}

const eCommStatus comm_readMessageFromBus(const eMessageBus arg_busType, tMessage * arg_message)
{
	const eBusStatus loc_sts_bus = bus_readMessageFromBuffer(arg_busType, &arg_message->body);
	const eCommStatus loc_sts_comm = (loc_sts_bus == E_BUS_STATUS_OK) ? E_COMM_STATUS_OK : E_COMM_STATUS_FAILED;

	return loc_sts_comm;
}

const eCommStatus comm_writeMessageToBus(const tMessage * const arg_message)
{
	const eBusStatus loc_sts_bus = bus_writeMessageToBuffer(arg_message->bus, &arg_message->body);
	const eCommStatus loc_sts_comm = (loc_sts_bus == E_BUS_STATUS_OK) ? E_COMM_STATUS_OK : E_COMM_STATUS_FAILED;

	return loc_sts_comm;
}

void comm_clearAcks(void)
{
	tMessage ** pMessage = transmitMessages;

	while (*pMessage != NULL)
	{
		(*pMessage)->ack = E_MSG_ACK_FAILED;
		pMessage++;
	}

	pMessage = receiveMessages;

	while (*pMessage != NULL)
	{
		(*pMessage)->ack = E_MSG_ACK_FAILED;
		pMessage++;
	}
}
