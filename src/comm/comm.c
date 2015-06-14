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

tCommMessage msgControl =
{
	E_COMM_MSG_CONTROL_ID,
	{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU},
	0xFFU,
	E_COMM_INIT,
	COMM_TIMEOUT,
	E_COMM_MSG_INACTIVE
};

tCommMessage msgCurrent =
{
	E_COMM_MSG_CURRENT_ID,
	{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU},
	0xFFU,
	E_COMM_INIT,
	COMM_TIMEOUT,
	E_COMM_MSG_INACTIVE
};

tCommMessage msgSuspension =
{
	E_COMM_MSG_SUSP_ID,
	{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU},
	0xFFU,
	E_COMM_INIT,
	COMM_TIMEOUT,
	E_COMM_MSG_INACTIVE
};

tCommMessage msgDirection =
{
	E_COMM_MSG_DIR_ID,
	{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU},
	0xFFU,
	E_COMM_INIT,
	COMM_TIMEOUT,
	E_COMM_MSG_INACTIVE
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
}

void comm_initTransmissionMessages(void)
{
	tCommMessage ** pMessage = transmitMessages;

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
	tCommMessage ** pMessage = transmitMessages;

	while (*pMessage != NULL)
	{
		comm_transmitMessage(*pMessage);
		pMessage++;
	}
}

void comm_readMessageFromBuffer(tCommMessage * const arg_message)
{
	uint8_t i;
	uint8_t loc_buffer[sizeof(tCommMessage)];

	for (i = 0U; i < sizeof(tCommMessage); i++)
	{
//		loc_buffer[i] = fifo_in();
	}

	memcpy(arg_message, loc_buffer, sizeof(tCommMessage));
}

void comm_writeMessageToBuffer(tCommMessage * const arg_message)
{
	uint8_t i;
	uint8_t loc_buffer[sizeof(tCommMessage)];

	memcpy(loc_buffer, arg_message->data, sizeof(tCommMessage));

	for (i = 0U; i < sizeof(tCommMessage); i++)
	{
//		fifo_out(loc_buffer)
	}
}

void comm_transmitMessage(tCommMessage * const arg_message)
{
	const uint8_t loc_messageId = arg_message->messageId;
	const eCommMessageStatus loc_messageStatus = arg_message->status;
	uint8_t loc_timeout = arg_message->timeout;
	uint8_t loc_state = arg_message->state;

	tCommMessage loc_message;
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

			break;
		}
		case E_COMM_TRANSMIT:
		{
			/*Reset timeout*/
			loc_timeout = COMM_TIMEOUT;

			/*Calculate crc*/
			loc_crc = crc8(arg_message->data, sizeof(arg_message->data));

			loc_message.messageId = loc_messageId;
			memcpy(loc_message.data, arg_message->data, sizeof(arg_message->data));
			loc_message.crc = loc_crc;

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
				tCommMessage loc_ackMessage;
				comm_readMessageFromBuffer(&loc_ackMessage);
				const uint8_t loc_ackMessageId = loc_ackMessage.messageId;

				if ((loc_ackMessageId && 0xF0U) == loc_messageId)
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
