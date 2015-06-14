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
static void comm_readMessageFromBuffer(tCommCyclicMessage * const arg_message);
static void comm_writeMessageToBuffer(const tCommCyclicMessage * const arg_message);
static void comm_transmitCyclicMessage(tCommCyclicMessage * const arg_message);

tCommCyclicMessage msgControl =
{
	{
			E_COMM_MSG_CONTROL_ID,
			{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU},
			0xFFU
	},
	E_COMM_INIT,
	COMM_TIMEOUT,
	E_COMM_MSG_INACTIVE
};

tCommCyclicMessage msgCurrent =
{
	{
		E_COMM_MSG_CURRENT_ID,
		{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU},
		0xFFU
	},
	E_COMM_INIT,
	COMM_TIMEOUT,
	E_COMM_MSG_INACTIVE
};

tCommCyclicMessage msgSuspension =
{
	{
		E_COMM_MSG_SUSP_ID,
		{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU},
		0xFFU
	},
	E_COMM_INIT,
	COMM_TIMEOUT,
	E_COMM_MSG_INACTIVE
};

tCommCyclicMessage msgDirection =
{
	{
		E_COMM_MSG_DIR_ID,
		{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU},
		0xFFU
	},
	E_COMM_INIT,
	COMM_TIMEOUT,
	E_COMM_MSG_INACTIVE
};

static tCommCyclicMessage * transmitMessages[] =
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
	tCommCyclicMessage ** pMessage = transmitMessages;

	while (*pMessage != NULL)
	{
		(*pMessage)->status = E_COMM_MSG_ACTIVE;
		pMessage++;
	}
}

void comm_cyclic(void)
{
	comm_cyclicTransmission();
}

void comm_cyclicTransmission(void)
{
	tCommCyclicMessage ** pMessage = transmitMessages;

	while (*pMessage != NULL)
	{
		comm_transmitCyclicMessage(*pMessage);
		pMessage++;
	}
}

void comm_readMessageFromBuffer(tCommCyclicMessage * const arg_message)
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

void comm_writeMessageToBuffer(const tCommCyclicMessage * const arg_message)
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

void comm_transmitCyclicMessage(tCommCyclicMessage * const arg_message)
{
	const uint8_t loc_messageId = arg_message->body.messageId;
	const eCommMessageStatus loc_messageStatus = arg_message->status;
	uint8_t loc_timeout = arg_message->timeout;
	eCommMessageState loc_state = arg_message->state;

	tCommCyclicMessage loc_message;
	uint8_t loc_crc;

	switch (loc_state)
	{
		case E_COMM_INIT:
		{
			if (loc_messageStatus == E_COMM_MSG_ACTIVE)
			{
				loc_state = E_COMM_TX_READY;
			}

			break;
		}
		case E_COMM_TX_READY:
		{
			/*If the UART buffer is available, start transmission*/
//			if (bufferAvailable)
//			{
//				loc_state = E_COMM_TRANSMIT;
//			}
#if defined(WIN32)
			loc_state = E_COMM_TRANSMIT;
#endif
			break;
		}
		case E_COMM_TRANSMIT:
		{
			/*Reset timeout*/
			loc_timeout = COMM_TIMEOUT;

			/*Calculate crc*/
			loc_crc = crc8(arg_message->body.data, sizeof(arg_message->body.data));

			loc_message.body.messageId = loc_messageId;
			memcpy(loc_message.body.data, arg_message->body.data, sizeof(arg_message->body.data));
			loc_message.body.crc = loc_crc;

			/*Write message to UART buffer*/
			comm_writeMessageToBuffer(&loc_message);

			loc_state = E_COMM_WAITFORACK;
			break;
		}
		case E_COMM_WAITFORACK:
		{
			if (loc_timeout > 0)
			{
				loc_timeout = loc_timeout - 1;

				/*Verify if ACK was received*/
				tCommCyclicMessage loc_ackMessage;
				comm_readMessageFromBuffer(&loc_ackMessage);
				const uint8_t loc_ackMessageId = loc_ackMessage.body.messageId;

				if ((loc_ackMessageId && COMM_IDMASK) == loc_messageId)
				{
					if (loc_ackMessageId & COMM_ACK)
					{
						/*Everything ok, can transmit again*/
						loc_state = E_COMM_TX_READY;
					}
					else
					{
						/*NACK received or something else, retransmit*/
						loc_state = E_COMM_TRANSMIT;
					}
				}
			}
			/*Timeout expired*/
			else
			{
				loc_state = E_COMM_TRANSMIT;
			}
			break;
		}
		default:
		case E_COMM_END:
		{

			break;
		}
	}

	arg_message->state = loc_state;
	arg_message->timeout = loc_timeout;
}
