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

static void comm_initTransmissionCyclicMessages(void);
static void comm_cyclicTransmission(void);
static eCommStatus comm_readMessageFromBuffer(tCommMessage * const arg_message);
static void comm_writeMessageToBuffer(const tCommMessage * const arg_message);
static void comm_transmitMessage(tCommMessage * const arg_message);

tCommMessage msgControl =
{
	{
		E_COMM_MSG_CONTROL_ID,
		{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU},
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
		{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU},
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
		{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU},
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
		{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU},
		0xFFU
	},
	E_COMM_MSG_STATE_INIT,
	COMM_TIMEOUT,
	COMM_MAXRETRANSMISSIONS,
	E_COMM_MSG_STATUS_INACTIVE,
	E_COMM_STATUS_FAILED,
	E_COMM_MSG_TYPE_CYCLIC
};

static tCommMessage * transmitMessages[] =
{
	&msgCurrent,
	&msgSuspension,
	&msgDirection,
	NULL
};

static tCommMessage * receiveMessages[] =
{
	NULL
};

void comm_init(void)
{
	comm_initTransmissionCyclicMessages();
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

eCommStatus comm_readMessageFromBuffer(tCommMessage * const arg_message)
{
	uint8_t loc_bytesRead;
	uint8_t loc_buffer[sizeof(tCommMessageBody)];
	eCommStatus loc_result = E_COMM_STATUS_FAILED;

#if defined(WIN32)
		loc_bytesRead = RS232_PollComport(UART_PORT, loc_buffer, sizeof(tCommMessageBody));
#else
	for (i = 0U; i < sizeof(tCommMessageBody); i++)
		{
	//		loc_buffer[i] = fifo_in();
		}
#endif

	if (loc_bytesRead == sizeof(tCommMessageBody))
	{
		memcpy(arg_message, loc_buffer, sizeof(tCommMessageBody));
		loc_result = E_COMM_STATUS_OK;
	}

	return loc_result;
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
			if (loc_messageStatus == E_COMM_MSG_STATUS_ACTIVE)
			{
				loc_state = E_COMM_MSG_STATE_TX_READY;
			}

			break;
		}
		case E_COMM_MSG_STATE_TX_READY:
		{
			/*If the UART buffer is available, start transmission*/
			//TODO uC
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
			break;
		}
	}

	arg_message->state = loc_state;
	arg_message->timeout = loc_timeout;
	arg_message->retransmissions = loc_retransmissions;
	arg_message->status = loc_messageStatus;
	arg_message->ack = loc_ack;
}

/*This function is called by the UART RX interrupt routine*/
void comm_receiveMessages(void)
{
	tCommMessage loc_receivedMessage;

	const eCommStatus loc_receiveResult = comm_readMessageFromBuffer(&loc_receivedMessage);

	if (loc_receiveResult == E_COMM_STATUS_OK)
	{
		const tCommMessageBody loc_receivedMessageBody = loc_receivedMessage.body;
		const eCommMessageId loc_receivedMessageId = loc_receivedMessageBody.messageId;

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
					const tCommMessageBody * loc_body = &((*pMessage)->body);
					const eCommStatus loc_checkCRC = comm_checkCRC(loc_body);

					if (loc_checkCRC == E_COMM_STATUS_OK)
					{
						/* Send ack. Content of the message is not important */
						tCommMessage loc_message;
						loc_message.body.messageId = (loc_receivedMessageId | COMM_ACK);
						comm_writeMessageToBuffer(&loc_message);
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

eCommStatus comm_checkCRC(const tCommMessageBody * const arg_messageBody)
{
	const uint8_t loc_receivedCRC = arg_messageBody->crc;
	const uint8_t loc_calculatedCRC = crc8(arg_messageBody->data, sizeof(arg_messageBody->data));
	const eCommStatus loc_result = (loc_receivedCRC == loc_calculatedCRC) ? E_COMM_STATUS_OK : E_COMM_STATUS_FAILED;

	return loc_result;
}
