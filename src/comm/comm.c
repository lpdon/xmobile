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

#include <string.h>

#if defined(WIN32)
	#include "../uart_windows/rs232.h"
	#define UART_PORT 15
	#define UART_BAUD 9600
	#define UART_MODE "8N1"
#endif

static void comm_initTransmissionMessages(void);
static void comm_cyclicTransmission(void);
static void comm_readMessageFromBuffer(tCommMessage * const arg_message);
static void comm_writeMessageToBuffer(const tCommMessage * const arg_message);
static void comm_transmitCyclicMessage(tCommMessage * const arg_message);

tCommMessage msgControl =
{
	{
		E_COMM_MSG_CONTROL_ID,
		{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU},
		0xFFU
	},
	E_COMM_MSG_STATE_INIT,
	COMM_TIMEOUT,
	E_COMM_MSG_STATUS_INACTIVE,
	E_COMM_MSG_TYPE_CYCLIC,
	E_COMM_STATUS_FAILED
};

tCommMessage msgCurrent =
{
	{
		E_COMM_MSG_CURRENT_ID,
		{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU},
		0xFFU
	},
	E_COMM_MSG_STATE_INIT,
	COMM_TIMEOUT,
	E_COMM_MSG_STATUS_INACTIVE,
	E_COMM_MSG_TYPE_CYCLIC,
	E_COMM_STATUS_FAILED
};

tCommMessage msgSuspension =
{
	{
		E_COMM_MSG_SUSP_ID,
		{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU},
		0xFFU
	},
	E_COMM_MSG_STATE_INIT,
	COMM_TIMEOUT,
	E_COMM_MSG_STATUS_INACTIVE,
	E_COMM_MSG_TYPE_CYCLIC,
	E_COMM_STATUS_FAILED
};

tCommMessage msgDirection =
{
	{
		E_COMM_MSG_DIR_ID,
		{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU},
		0xFFU
	},
	E_COMM_MSG_STATE_INIT,
	COMM_TIMEOUT,
	E_COMM_MSG_STATUS_INACTIVE,
	E_COMM_MSG_TYPE_CYCLIC,
	E_COMM_STATUS_FAILED
};

static tCommMessage * transmitMessages[] =
{
	&msgCurrent,
	&msgSuspension,
	&msgDirection,
	NULL
};

void comm_init(void)
{
	comm_initTransmissionMessages();
#if defined(WIN32)
	RS232_OpenComport(UART_PORT, UART_BAUD, UART_MODE);
#endif
}

void comm_end(void)
{
#if defined(WIN32)
	RS232_CloseComport(UART_PORT);
#endif
}

void comm_initTransmissionMessages(void)
{
	tCommMessage ** pMessage = transmitMessages;

	while (*pMessage != NULL)
	{
		(*pMessage)->status = E_COMM_MSG_STATUS_ACTIVE;
		pMessage++;
	}
}

void comm_cyclic(void)
{
	comm_cyclicTransmission();
}

void comm_cyclicTransmission(void)
{
	tCommMessage ** pMessage = transmitMessages;

	while (*pMessage != NULL)
	{
		comm_transmitCyclicMessage(*pMessage);
		pMessage++;
	}
}

void comm_readMessageFromBuffer(tCommMessage * const arg_message)
{
	uint8_t i;
	uint8_t loc_buffer[sizeof(tCommMessageBody)];

#if defined(WIN32)
		RS232_PollComport(UART_PORT, loc_buffer, sizeof(tCommMessageBody));
#else
	for (i = 0U; i < sizeof(tCommMessageBody); i++)
		{
	//		loc_buffer[i] = fifo_in();
		}
#endif

	memcpy(arg_message, loc_buffer, sizeof(tCommMessageBody));
}

void comm_writeMessageToBuffer(const tCommMessage * const arg_message)
{
	uint8_t i;
	uint8_t loc_buffer[sizeof(tCommMessageBody)];

	memcpy(loc_buffer, arg_message, sizeof(tCommMessageBody));

	for (i = 0U; i < sizeof(tCommMessageBody); i++)
	{
//		fifo_out(loc_buffer)
#if defined(WIN32)
		RS232_SendByte(UART_PORT, loc_buffer[i]);
#endif
	}
}

void comm_transmitCyclicMessage(tCommMessage * const arg_message)
{
	const eCommMessageId loc_messageId = arg_message->body.messageId;
	const eCommMessageStatus loc_messageStatus = arg_message->status;
	uint8_t loc_timeout = arg_message->timeout;
	eCommMessageState loc_state = arg_message->state;
	eCommStatus loc_ack = arg_message->ack;
	eCommMessageType loc_messageType = arg_message->type;

	tCommMessage loc_message;
	uint8_t loc_crc;

	switch (loc_state)
	{
		case E_COMM_MSG_STATE_INIT:
		{
			if (loc_messageStatus == E_COMM_MSG_STATUS_ACTIVE)
			{
				loc_state = E_COMM_MSG_STATE_TX_READY;
			}

			break;
		}
		case E_COMM_MSG_STATE_TX_READY:
		{
			/*If the UART buffer is available, start transmission*/
//			if (bufferAvailable)
//			{
//				loc_state = E_COMM_TRANSMIT;
//			}
#if defined(WIN32)
			loc_state = E_COMM_MSG_STATE_TRANSMIT;
#endif
			break;
		}
		case E_COMM_MSG_STATE_TRANSMIT:
		{
			/*Reset timeout*/
			loc_timeout = COMM_TIMEOUT;

			/*Reset ack flag*/
			loc_ack = E_COMM_STATUS_FAILED;

			/*Calculate crc*/
			loc_crc = crc8(arg_message->body.data, sizeof(arg_message->body.data));

			loc_message.body.messageId = loc_messageId;
			memcpy(loc_message.body.data, arg_message->body.data, sizeof(arg_message->body.data));
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
				loc_timeout = loc_timeout - 1;

				/*Verify if ACK was received*/
				if (loc_ack == E_COMM_STATUS_OK)
				{
					loc_state = E_COMM_MSG_STATE_TX_READY;
				}
			}
			/*Timeout expired*/
			else
			{
				loc_state = E_COMM_MSG_STATE_TRANSMIT;
			}
			break;
		}
		default:
		case E_COMM_MSG_STATE_END:
		{

			break;
		}
	}

	arg_message->state = loc_state;
	arg_message->timeout = loc_timeout;
	arg_message->ack = loc_ack;
}

/*This function is called by the UART RX interrupt routine*/
void comm_receiveMessages(void)
{
	uint8_t loc_buffer[sizeof(tCommMessageBody)];
	tCommMessageBody loc_messageBody;
	uint8_t loc_receiveResult;

#if defined(WIN32)
	loc_receiveResult = RS232_PollComport(UART_PORT, loc_buffer, sizeof(tCommMessageBody));

	if (loc_receiveResult == sizeof(tCommMessageBody))
	{
		memcpy(&loc_messageBody, loc_buffer, sizeof(tCommMessageBody));
		const eCommMessageId loc_messageId = loc_messageBody.messageId;

		/*Detect if the message received is a reply*/
		if ((loc_messageId & COMM_REPLYMASK) != 0U)
		{
			tCommMessage ** pMessage = transmitMessages;

			while (*pMessage != NULL)
			{
				const eCommMessageId loc_maskedMessageId = loc_messageId & COMM_IDMASK;
				if ((*pMessage)->body.messageId == loc_maskedMessageId)
				{
					if (loc_messageId & COMM_ACK)
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
			//TODO
		}
	}
#endif
}
