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

#ifndef UART_INTERFACE_H
	#include "../uart_interface/uart_interface.h"
#endif

#include <string.h>

static void comm_initTransmissionCyclicMessages(void);
static void comm_cyclicReception(void);
static void comm_cyclicTransmission(void);
static void comm_receiveMessages(void);
static const eCommStatus comm_readMessageFromBuffer(tCommMessage * const arg_message);
static void comm_writeMessageToBuffer(const tCommMessage * const arg_message);
static void comm_transmitMessage(tCommMessage * const arg_message);
static const eCommStatus comm_checkMessageId(const uint8_t arg_messageId);

#if !defined(WIN32)
static eCommStatus comm_dataAvailable = E_COMM_STATUS_FAILED;
#else
static eCommStatus comm_dataAvailable = E_COMM_STATUS_OK;
#endif

tCommMessage msgControl =
{
	{
		E_COMM_MSG_CONTROL_ID,
		{{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU}},
		0xFFU
	},
	E_COMM_MSG_STATE_INIT,
	COMM_TIMEOUT,
	COMM_MAXRETRANSMISSIONS,
	E_COMM_MSG_STATUS_INACTIVE,
	E_COMM_STATUS_FAILED,
	E_COMM_MSG_TYPE_CYCLIC
};

tCommMessage msgCurrent =
{
	{
		E_COMM_MSG_CURRENT_ID,
		{{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU}},
		0xFFU
	},
	E_COMM_MSG_STATE_INIT,
	COMM_TIMEOUT,
	COMM_MAXRETRANSMISSIONS,
	E_COMM_MSG_STATUS_INACTIVE,
	E_COMM_STATUS_FAILED,
	E_COMM_MSG_TYPE_CYCLIC
};

tCommMessage msgSuspension =
{
	{
		E_COMM_MSG_SUSP_ID,
		{{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU}},
		0xFFU
	},
	E_COMM_MSG_STATE_INIT,
	COMM_TIMEOUT,
	COMM_MAXRETRANSMISSIONS,
	E_COMM_MSG_STATUS_INACTIVE,
	E_COMM_STATUS_FAILED,
	E_COMM_MSG_TYPE_CYCLIC
};

tCommMessage msgDirection =
{
	{
		E_COMM_MSG_DIR_ID,
		{{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU}},
		0xFFU
	},
	E_COMM_MSG_STATE_INIT,
	COMM_TIMEOUT,
	COMM_MAXRETRANSMISSIONS,
	E_COMM_MSG_STATUS_INACTIVE,
	E_COMM_STATUS_FAILED,
	E_COMM_MSG_TYPE_CYCLIC
};

#if NODE==CONTROL
static tCommMessage * transmitMessages[] =
{
	&msgControl,
	NULL
};

static tCommMessage * receiveMessages[] =
{
	&msgCurrent,
//	&msgSuspension,
//	&msgDirection,
	NULL
};
#else
static tCommMessage * transmitMessages[] =
{
	&msgCurrent,
	NULL
};

static tCommMessage * receiveMessages[] =
{
	&msgControl,
	NULL
};
#endif

void comm_setData(eCommMessageId arg_id, const void * const arg_data)
{
	tCommMessage ** pMessage = transmitMessages;
	const eCommMessageId loc_id = (*pMessage)->body.messageId;

	while (*pMessage != NULL)
	{
		if (loc_id == arg_id)
		{
			memcpy((*pMessage)->body.data.rawData, arg_data, COMM_DATASIZE);
			break;
		}
	}
}

void comm_init(void)
{
	comm_initTransmissionCyclicMessages();
	uart_init();
}

void comm_end(void)
{
	uart_end();
}

void comm_initTransmissionCyclicMessages(void)
{
	tCommMessage ** pMessage = transmitMessages;

	while (*pMessage != NULL)
	{
		const eCommMessageType loc_type = (*pMessage)->type;
		(*pMessage)->status = (loc_type == E_COMM_MSG_TYPE_CYCLIC) ? E_COMM_MSG_STATUS_ACTIVE : E_COMM_MSG_STATUS_INACTIVE;
		pMessage++;
	}
}

void comm_cyclic(void)
{
	comm_cyclicReception();
	comm_cyclicTransmission();
}

void comm_cyclicReception(void)
{
	comm_receiveMessages();
}

void comm_cyclicTransmission(void)
{
	tCommMessage ** pMessage = transmitMessages;

	while (*pMessage != NULL)
	{
		comm_transmitMessage(*pMessage);
		pMessage++;
	}
}

void comm_transmitMessage(tCommMessage * const arg_message)
{
	const eCommMessageId loc_messageId = arg_message->body.messageId;
	const eCommMessageType loc_type = arg_message->type;

	eCommMessageState loc_state = arg_message->state;
	uint8_t loc_timeout = arg_message->timeout;
	uint8_t loc_retransmissions = arg_message->retransmissions;
	eCommMessageStatus loc_messageStatus = arg_message->status;
	eCommStatus loc_ack = arg_message->ack;

	tCommMessage loc_message;
	uint8_t loc_crc;

	switch (loc_state)
	{
		case E_COMM_MSG_STATE_INIT:
		{
			/*Reset retransmissions*/
			loc_timeout = COMM_TIMEOUT;

			if (loc_messageStatus == E_COMM_MSG_STATUS_ACTIVE)
			{
				loc_state = E_COMM_MSG_STATE_TX_READY;
			}

			break;
		}
		case E_COMM_MSG_STATE_TX_READY:
		{
			/*If the UART buffer is available, start transmission*/
//#if defined(WIN32)
			loc_state = E_COMM_MSG_STATE_TRANSMIT;
//#endif
			break;
		}
		case E_COMM_MSG_STATE_TRANSMIT:
		{
			/*Reset timeout*/
			loc_timeout = COMM_TIMEOUT;

			/*Reset ack flag*/
			loc_ack = E_COMM_STATUS_FAILED;

			/*Calculate crc*/
			loc_crc = crc8(arg_message->body.data.rawData, sizeof(arg_message->body.data.rawData));

			loc_message.body.messageId = loc_messageId;
			memcpy(loc_message.body.data.rawData, arg_message->body.data.rawData, sizeof(arg_message->body.data.rawData));
			loc_message.body.crc = loc_crc;

			/*Write message to UART buffer*/
			comm_writeMessageToBuffer(&loc_message);

			loc_state = E_COMM_MSG_STATE_WAITFORACK;
			break;
		}
		case E_COMM_MSG_STATE_WAITFORACK:
		{
			if (loc_timeout > 0)
			{
				/*Verify if ACK was received*/
				if (loc_ack == E_COMM_STATUS_OK)
				{
					/*If the message is cyclic it can be prepared to be transmitted once again*/
					if (loc_type == E_COMM_MSG_TYPE_CYCLIC)
					{
						loc_state = E_COMM_MSG_STATE_TX_READY;
					}
					/*Otherwise, the next transmission must be triggered*/
					else
					{
						loc_state = E_COMM_MSG_STATE_INIT;
						loc_messageStatus = E_COMM_MSG_STATUS_INACTIVE;
					}
				}

				loc_timeout--;
			}
			/*Timeout expired*/
			else
			{
				loc_state = (loc_retransmissions > 0U) ? E_COMM_MSG_STATE_TRANSMIT : E_COMM_MSG_STATE_END;
				loc_retransmissions--;
			}
			break;
		}
		default:
		case E_COMM_MSG_STATE_END:
		{
			//TODO TRM Shutdown :)
//#if defined(WIN32)
			loc_state = E_COMM_MSG_STATE_INIT;
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

void comm_receiveMessages(void)
{
	const eCommStatus loc_dataAvailable = comm_dataAvailable;
	tCommMessage loc_receivedMessage;

	if (loc_dataAvailable == E_COMM_STATUS_OK)
	{
		while (comm_readMessageFromBuffer(&loc_receivedMessage) == E_COMM_STATUS_OK)
		{
			tCommMessageBody loc_receivedMessageBody = loc_receivedMessage.body;
			eCommMessageId loc_receivedMessageId = loc_receivedMessageBody.messageId;

			/*Detect if the message received is a reply*/
			if ((loc_receivedMessageId & COMM_REPLYMASK) != 0U)
			{
				tCommMessage ** pMessage = transmitMessages;

				while (*pMessage != NULL)
				{
					const eCommMessageId loc_maskedMessageId = loc_receivedMessageId & COMM_IDMASK;
					if ((*pMessage)->body.messageId == loc_maskedMessageId)
					{
						if (loc_receivedMessageId & COMM_ACK)
						{
							(*pMessage)->ack = E_COMM_STATUS_OK;
						}
						break;
					}
					pMessage++;
				}
			}
			else
			{
				tCommMessage ** pMessage = receiveMessages;

				while (*pMessage != NULL)
				{
					if ((*pMessage)->body.messageId == loc_receivedMessageId)
					{
						const eCommStatus loc_checkCRC = comm_checkCRC(&loc_receivedMessageBody);

						if (loc_checkCRC == E_COMM_STATUS_OK)
						{
							/* Send ack. Content of the message is not important */
							tCommMessage loc_message;
							loc_message.body.messageId = (loc_receivedMessageId | COMM_ACK);
							comm_writeMessageToBuffer(&loc_message);

							/* Write valid values */
							(*pMessage)->body = loc_receivedMessageBody;
						}
						else
						{
							/* Send nack. Content of the message is not important */
							tCommMessage loc_message;
							loc_message.body.messageId = (loc_receivedMessageId | COMM_NACK);
							comm_writeMessageToBuffer(&loc_message);
						}

						break;
					}
					pMessage++;
				}
			}
		}
	}
#if !defined(WIN32)
	comm_dataAvailable = E_COMM_STATUS_FAILED;
#endif
}

void comm_setDataAvailable(void)
{
	comm_dataAvailable = E_COMM_STATUS_OK;
}

eCommStatus comm_checkCRC(const tCommMessageBody * const arg_messageBody)
{
	const uint8_t loc_receivedCRC = arg_messageBody->crc;
	const uint8_t loc_calculatedCRC = crc8(arg_messageBody->data.rawData, sizeof(uCommMessageData));
	const eCommStatus loc_result = (loc_receivedCRC == loc_calculatedCRC) ? E_COMM_STATUS_OK : E_COMM_STATUS_FAILED;

	return loc_result;
}

void comm_kickoutMessage(tCommMessage * const arg_message)
{
	const eCommMessageStatus loc_status = arg_message->status;
	arg_message->status = (loc_status == E_COMM_MSG_STATUS_INACTIVE) ? E_COMM_MSG_STATUS_ACTIVE : E_COMM_MSG_STATUS_INACTIVE;
}

const eCommStatus comm_checkMessageId(const uint8_t arg_messageId)
{
	eCommStatus loc_result = E_COMM_STATUS_FAILED;

	tCommMessage ** pMessage = receiveMessages;
	const uint8_t loc_maskedMessageId = arg_messageId & COMM_IDMASK;

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

const eCommStatus comm_readMessageFromBuffer(tCommMessage * arg_message)
{
	uint16_t loc_totalBytesRead = 0U;
	tCommMessageBody loc_messageBody;
	eCommStatus loc_validId = E_COMM_STATUS_FAILED;
	eCommStatus loc_result = E_COMM_STATUS_FAILED;
	eUartStatus loc_uartResult = E_UART_STATUS_FAILED;

	/*Read 1 byte to check if it is really the beginning of a message*/
	loc_uartResult = uart_readFromBuffer(&loc_messageBody.messageId, sizeof(uint8_t));

	if (loc_uartResult == E_UART_STATUS_OK)
	{
		loc_validId = comm_checkMessageId(loc_messageBody.messageId);
	}

	if (loc_validId == E_COMM_STATUS_OK)
	{
		loc_uartResult = uart_readFromBuffer(loc_messageBody.data.rawData, sizeof(uCommMessageData));
		loc_totalBytesRead += (loc_uartResult == E_UART_STATUS_OK) ? sizeof(uCommMessageData) : 0U;
		loc_uartResult = uart_readFromBuffer(&loc_messageBody.crc, sizeof(uint8_t));
		loc_totalBytesRead += (loc_uartResult == E_UART_STATUS_OK) ? sizeof(uint8_t) : 0U;

		if (loc_totalBytesRead == sizeof(tCommMessageBody))
		{
			memcpy(arg_message, &loc_messageBody, sizeof(tCommMessageBody));
			loc_result = E_COMM_STATUS_OK;
		}
	}

	return loc_result;
}

void comm_writeMessageToBuffer(const tCommMessage * const arg_message)
{
	uint8_t loc_buffer[sizeof(tCommMessageBody)];

	memcpy(loc_buffer, arg_message, sizeof(tCommMessageBody));
	uart_writeToBuffer(loc_buffer, sizeof(tCommMessageBody));
}
