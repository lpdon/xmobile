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

#ifndef HANDSHAKE_H
	#include "handshake.h"
#endif

#ifndef CRC_H
	#include "../crc/crc.h"
#endif

#ifndef UART_INTERFACE_H
	#include "../bus_interface/uart_interface.h"
#endif

#include <string.h>

static void handshake_cyclicReception(void);
static void handshake_cyclicTransmission(void);
static void handshake_transmitMessage(tHandshakeMessage * const arg_message);
static void handshake_receiveMessages(void);

const eHandshakeStatus handshake_readMessageFromBuffer(tHandshakeMessage * arg_message);
static void handshake_writeMessageToBuffer(const tHandshakeMessage * const arg_message);
eHandshakeStatus handshake_checkCRC(const tHandshakeMessageBody * const arg_messageBody);
static const eHandshakeStatus handshake_checkMessageId(const uint8_t arg_messageId);

tHandshakeMessage handshakeRqst =
{
	{
		E_HANDSHAKE_RQST_ID,
		{{0xF0U, 0xDAU, 0xCEU, 0xF0U, 0xDAU, 0xCEU, 0xFFU, 0xFFU}},
		0xFFU
	},
	E_HANDSHAKE_STATE_INIT,
	{
		HANDSHAKE_CYCLETIME,
		HANDSHAKE_CYCLETIME
	},
	HANDSHAKE_RETRANSMISSIONS,
	E_HANDSHAKE_MSG_STATUS_INACTIVE,
	E_HANDSHAKE_STATUS_FAILED
};

tHandshakeMessage handshakeResponse =
{
	{
		E_HANDSHAKE_RESPONSE_ID,
		{{0xFFU, 0xFFU, 0xF0U, 0xDAU, 0xCEU, 0xF0U, 0xDAU, 0xCEU}},
		0xFFU
	},
	E_HANDSHAKE_STATE_INIT,
	{
		1U,
		1U
	},
	HANDSHAKE_RETRANSMISSIONS,
	E_HANDSHAKE_MSG_STATUS_INACTIVE,
	E_HANDSHAKE_STATUS_FAILED
};

#if NODE==CONTROL
tHandshakeMessage * transmitMessage = &handshakeRqst;
tHandshakeMessage * receiveMessage = &handshakeResponse;
#else
tHandshakeMessage * transmitMessage = &handshakeResponse;
tHandshakeMessage * receiveMessage = &handshakeRqst;
#endif

void handshake_init(void)
{
#if NODE==CONTROL
	handshakeRqst.status = E_HANDSHAKE_MSG_STATUS_ACTIVE;
#endif
}

void handshake_cyclic(void)
{
	handshake_cyclicReception();
	handshake_cyclicTransmission();
}

void handshake_cyclicReception(void)
{
	handshake_receiveMessages();
}

void handshake_cyclicTransmission(void)
{
	handshake_transmitMessage(transmitMessage);
}

void handshake_transmitMessage(tHandshakeMessage * const arg_message)
{
	const eHandshakeMessageId loc_messageId = arg_message->body.messageId;

	eHandshakeMessageState loc_state = arg_message->state;
	uint8_t loc_retransmissions = arg_message->retransmissions;
	uint8_t loc_cycleTime = arg_message->cycleTime.current;
	eHandshakeMessageStatus loc_messageStatus = arg_message->status;
	eHandshakeStatus loc_ack = arg_message->ack;

	tHandshakeMessage loc_message;
	uint8_t loc_crc;

	switch (loc_state)
	{
		case E_HANDSHAKE_STATE_INIT:
		{
			if (loc_messageStatus == E_HANDSHAKE_MSG_STATUS_ACTIVE)
			{
				loc_state = E_HANDSHAKE_STATE_TRANSMIT;
			}
			break;
		}
		case E_HANDSHAKE_STATE_TX_READY:
		{
			/*Reset retransmissions*/
			loc_retransmissions = HANDSHAKE_RETRANSMISSIONS;

			loc_state = E_HANDSHAKE_STATE_TRANSMIT;
			break;
		}
		case E_HANDSHAKE_STATE_TRANSMIT:
		{
			/*Reset cycle time*/
			loc_cycleTime = arg_message->cycleTime.initial;

			/*Reset ack flag*/
			loc_ack = E_HANDSHAKE_STATUS_FAILED;

			/*Calculate crc*/
			loc_crc = crc8(arg_message->body.data.rawData, sizeof(arg_message->body.data.rawData));

			loc_message.body.messageId = loc_messageId;
			memcpy(loc_message.body.data.rawData, arg_message->body.data.rawData, sizeof(arg_message->body.data.rawData));
			loc_message.body.crc = loc_crc;

			/*Write message to UART buffer*/
			handshake_writeMessageToBuffer(&loc_message);

			loc_state = E_HANDSHAKE_STATE_WAITFORACK;
			break;
		}
		case E_HANDSHAKE_STATE_WAITFORACK:
		{
			/*Verify if ACK was received*/
			if (loc_ack == E_HANDSHAKE_STATUS_OK)
			{
				loc_state = E_HANDSHAKE_STATE_END_SUCCESS;
			}
			else
			{
				loc_state = (loc_cycleTime > 0) ? E_HANDSHAKE_STATE_WAITFORACK : E_HANDSHAKE_STATE_TRANSMIT;
				loc_cycleTime--;
			}
			break;
		}
		case E_HANDSHAKE_STATE_END_SUCCESS:
		{
			break;
		}
	}

	arg_message->state = loc_state;
	arg_message->retransmissions = loc_retransmissions;
	arg_message->status = loc_messageStatus;
	arg_message->cycleTime.current = loc_cycleTime;
}

void handshake_receiveMessages(void)
{
	const eUartStatus loc_dataAvailable = uart_getDataAvailable();
	tHandshakeMessage loc_receivedMessage;

	if (loc_dataAvailable == E_UART_STATUS_OK)
	{
		while (handshake_readMessageFromBuffer(&loc_receivedMessage) == E_HANDSHAKE_STATUS_OK)
		{
			tHandshakeMessageBody loc_receivedMessageBody = loc_receivedMessage.body;
			eHandshakeMessageId loc_receivedMessageId = loc_receivedMessageBody.messageId;

			/*Detect if the message received is a reply*/
			if ((loc_receivedMessageId & HANDSHAKE_REPLYMASK) != 0U)
			{
				const eHandshakeMessageId loc_maskedMessageId = loc_receivedMessageId & HANDSHAKE_IDMASK;
				if (transmitMessage->body.messageId == loc_maskedMessageId)
				{
					if (loc_receivedMessageId & HANDSHAKE_ACK)
					{
						transmitMessage->ack = E_HANDSHAKE_STATUS_OK;
					}
				}
			}
			else
			{
				if (receiveMessage->body.messageId == loc_receivedMessageId)
				{
					const eCrcStatus loc_checkCRC = checkCRC(loc_receivedMessageBody.data.rawData,
															 sizeof(uHandshakeMessageData),
															 loc_receivedMessageBody.crc);

					if (loc_checkCRC == E_CRC_STATUS_OK)
					{
						/* Send ack. Content of the message is not important */
						tHandshakeMessage loc_message;
						loc_message.body.messageId = (loc_receivedMessageId | HANDSHAKE_ACK);
						handshake_writeMessageToBuffer(&loc_message);
						receiveMessage->state = E_HANDSHAKE_STATE_END_SUCCESS;

						/* If the message received was a handshake request, activate the response*/
						if (loc_receivedMessageId == E_HANDSHAKE_RQST_ID)
						{
							handshakeResponse.status = E_HANDSHAKE_MSG_STATUS_ACTIVE;
						}
					}
					else
					{
						/* Send nack. Content of the message is not important */
						tHandshakeMessage loc_message;
						loc_message.body.messageId = (loc_receivedMessageId | HANDSHAKE_NACK);
						handshake_writeMessageToBuffer(&loc_message);
					}
				}
			}
		}
	}
#if !defined(WIN32)
	uart_clearDataAvailable();
#endif
}

const eHandshakeStatus handshake_readMessageFromBuffer(tHandshakeMessage * arg_message)
{
	uint16_t loc_totalBytesRead = 0U;
	tHandshakeMessageBody loc_messageBody;
	eHandshakeStatus loc_validId = E_HANDSHAKE_STATUS_FAILED;
	eHandshakeStatus loc_result = E_HANDSHAKE_STATUS_FAILED;
	eUartStatus loc_uartResult = E_UART_STATUS_FAILED;

	/*Read 1 byte to check if it is really the beginning of a message*/
	loc_uartResult = uart_readFromBuffer(&loc_messageBody.messageId, sizeof(uint8_t));
	while (loc_uartResult == E_UART_STATUS_OK)
	{
		loc_totalBytesRead = 0U;
		loc_validId = handshake_checkMessageId(loc_messageBody.messageId);

		if (loc_validId == E_HANDSHAKE_STATUS_OK)
		{
			loc_totalBytesRead++;
			loc_uartResult = uart_readFromBuffer(loc_messageBody.data.rawData, sizeof(uHandshakeMessageData));
			loc_totalBytesRead += (loc_uartResult == E_UART_STATUS_OK) ? sizeof(uHandshakeMessageData) : 0U;
			loc_uartResult = uart_readFromBuffer(&loc_messageBody.crc, sizeof(uint8_t));
			loc_totalBytesRead += (loc_uartResult == E_UART_STATUS_OK) ? sizeof(uint8_t) : 0U;

			if (loc_totalBytesRead == sizeof(tHandshakeMessageBody))
			{
				memcpy(arg_message, &loc_messageBody, sizeof(tHandshakeMessageBody));
				loc_result = E_HANDSHAKE_STATUS_OK;
			}
		}

		loc_uartResult = uart_readFromBuffer(&loc_messageBody.messageId, sizeof(uint8_t));
	}

	return loc_result;
}

static void handshake_writeMessageToBuffer(const tHandshakeMessage * const arg_message)
{
	uint8_t loc_buffer[sizeof(tHandshakeMessageBody)];

	memcpy(loc_buffer, arg_message, sizeof(tHandshakeMessageBody));
	uart_writeToBuffer(loc_buffer, sizeof(tHandshakeMessageBody));
}

eHandshakeStatus handshake_checkCRC(const tHandshakeMessageBody * const arg_messageBody)
{
	const uint8_t loc_receivedCRC = arg_messageBody->crc;
	const uint8_t loc_calculatedCRC = crc8(arg_messageBody->data.rawData, sizeof(uHandshakeMessageData));
	const eHandshakeStatus loc_result = (loc_receivedCRC == loc_calculatedCRC) ? E_HANDSHAKE_STATUS_OK : E_HANDSHAKE_STATUS_FAILED;

	return loc_result;
}

eHandshakeStatus handshake_getStatus(void)
{
	eHandshakeStatus loc_result = E_HANDSHAKE_STATUS_FAILED;

	if  ((handshakeResponse.state == E_HANDSHAKE_STATE_END_SUCCESS) &&
		 (handshakeRqst.state == E_HANDSHAKE_STATE_END_SUCCESS))
	{
		loc_result = E_HANDSHAKE_STATUS_OK;
	}

	return loc_result;
}

const eHandshakeStatus handshake_checkMessageId(const uint8_t arg_messageId)
{
	eHandshakeStatus loc_result = E_HANDSHAKE_STATUS_FAILED;

	const uint8_t loc_maskedMessageId = arg_messageId & HANDSHAKE_IDMASK;

	if ((loc_maskedMessageId == handshakeResponse.body.messageId) ||
	    (loc_maskedMessageId == handshakeRqst.body.messageId))
	{
		loc_result = E_HANDSHAKE_STATUS_OK;
	}

	return loc_result;
}
