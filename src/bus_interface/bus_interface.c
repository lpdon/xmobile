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

#ifndef BUS_INTEFACE_H
	#include "bus_interface.h"
#endif

#ifndef UART_INTERFACE_H
	#include "uart_interface.h"
#endif

#ifndef CAN_INTERFACE_H
	#include "can_interface.h"
#endif

#ifndef MESSAGE_H
	#include "../comm/message.h"
#endif

#include <string.h>

static const eBusStatus bus_checkMessageId(const uint8_t arg_messageId);

eBusStatus bus_init(eBusType arg_busType)
{
	switch (arg_busType)
	{
		case E_BUS_TYPE_UART:
		{
			uart_init();
			break;
		}
		case E_BUS_TYPE_CAN:
		{
			can_init();
			break;
		}
		default:
		{
			break;
		}
	}

	return E_BUS_STATUS_OK;
}

void bus_end(eBusType arg_busType)
{
	switch (arg_busType)
	{
		case E_BUS_TYPE_UART:
		{
			uart_end();
			break;
		}
		case E_BUS_TYPE_CAN:
		{
			can_end();
			break;
		}
		default:
		{
			break;
		}
	}
}

eBusStatus bus_writeMessageToBuffer(const eBusType arg_busType, const tMessageBody * const arg_messageBody)
{
	eBusStatus loc_result = E_BUS_STATUS_FAILED;

	switch (arg_busType)
	{
		case E_BUS_TYPE_UART:
		{
			const eUartStatus loc_sts_uart = uart_writeToBuffer((uint8_t *)arg_messageBody, sizeof(tMessageBody));
			loc_result = (loc_sts_uart == E_UART_STATUS_OK) ? E_BUS_STATUS_OK : E_BUS_STATUS_FAILED;
			break;
		}
		case E_BUS_TYPE_CAN:
		{
			const eCanStatus loc_sts_can = can_writeToBuffer(arg_messageBody->messageId, arg_messageBody->data.rawData, sizeof(uMessageData));
			loc_result = (loc_sts_can == E_CAN_STATUS_OK) ? E_BUS_STATUS_OK : E_BUS_STATUS_FAILED;
			break;
		}
		default:
		{
			break;
		}
	}

	return loc_result;
}

eBusStatus bus_readMessageFromBuffer(const eBusType arg_busType, tMessageBody * const arg_messageBody)
{
	eBusStatus loc_result = E_BUS_STATUS_FAILED;
	tMessageBody loc_messageBody;

	switch (arg_busType)
	{
		case E_BUS_TYPE_UART:
		{
			uint16_t loc_totalBytesRead = 0U;
			eBusStatus loc_validId = E_BUS_STATUS_FAILED;
			eUartStatus loc_sts_uart = E_UART_STATUS_FAILED;

			/*Read 1 byte to check if it is really the beginning of a message*/
			loc_sts_uart = uart_readFromBuffer(&loc_messageBody.messageId, sizeof(uint8_t));
			while(loc_sts_uart == E_UART_STATUS_OK)
			{
				loc_totalBytesRead = 0U;
				loc_validId = bus_checkMessageId(loc_messageBody.messageId);

				if (loc_validId == E_BUS_STATUS_OK)
				{
					loc_totalBytesRead++;
					loc_sts_uart = uart_readFromBuffer(loc_messageBody.data.rawData, sizeof(uMessageData));
					loc_totalBytesRead += (loc_sts_uart == E_UART_STATUS_OK) ? sizeof(uMessageData) : 0U;
					loc_sts_uart = uart_readFromBuffer(&loc_messageBody.crc, sizeof(uint8_t));
					loc_totalBytesRead += (loc_sts_uart == E_UART_STATUS_OK) ? sizeof(uint8_t) : 0U;

					if (loc_totalBytesRead == sizeof(tMessageBody))
					{
						memcpy(arg_messageBody, &loc_messageBody, sizeof(tMessageBody));
						loc_result = E_BUS_STATUS_OK;
					}
				}

				loc_sts_uart = uart_readFromBuffer(&loc_messageBody.messageId, sizeof(uint8_t));
			}
			break;
		}
		case E_BUS_TYPE_CAN:
		{
			const eCanStatus loc_sts_can = can_readFromBuffer(arg_messageBody->messageId, arg_messageBody->data.rawData, sizeof(uMessageData));
			loc_result = (loc_sts_can == E_CAN_STATUS_OK) ? E_BUS_STATUS_OK : E_BUS_STATUS_FAILED;
			break;
		}
		default:
		{
			break;
		}
	}

	return loc_result;
}

void bus_setDataAvailable(eBusType arg_busType)
{
	switch (arg_busType)
	{
		case E_BUS_TYPE_UART:
		{
			uart_setDataAvailable();
			break;
		}
		case E_BUS_TYPE_CAN:
		{
			can_setDataAvailable();
			break;
		}
		default:
		{
			break;
		}
	}
}

void bus_clearDataAvailable(eBusType arg_busType)
{
	switch (arg_busType)
	{
		case E_BUS_TYPE_UART:
		{
			uart_clearDataAvailable();
			break;
		}
		case E_BUS_TYPE_CAN:
		{
			can_clearDataAvailable();
			break;
		}
		default:
		{
			break;
		}
	}
}

eBusStatus bus_getDataAvailable(eBusType arg_busType)
{
	eBusStatus loc_result = E_BUS_STATUS_FAILED;

	switch (arg_busType)
	{
		case E_BUS_TYPE_UART:
		{
#if !defined(_WIN32)
			const eUartStatus loc_sts_uart = uart_getDataAvailable();
			loc_result = (loc_sts_uart == E_UART_STATUS_OK) ? E_BUS_STATUS_OK : E_BUS_STATUS_FAILED;
#else
			loc_result = E_BUS_STATUS_OK;
#endif
			break;
		}
		case E_BUS_TYPE_CAN:
		{
			const eCanStatus loc_sts_can = can_getDataAvailable();
			loc_result = (loc_sts_can == E_CAN_STATUS_OK) ? E_BUS_STATUS_OK : E_BUS_STATUS_FAILED;
			break;
		}
		default:
		{
			break;
		}
	}

	return loc_result;
}

const eBusStatus bus_checkMessageId(const uint8_t arg_messageId)
{
//	const uint8_t loc_maskedMessageId = arg_messageId & MSG_IDMASK;
	volatile uint8_t loc_maskedMessageId = arg_messageId & MSG_IDMASK;
	eBusStatus loc_result = E_BUS_STATUS_FAILED;

	switch ((eMessageId)loc_maskedMessageId)
	{
		case E_MSG_CONTROL_ID:
		case E_MSG_CURRENT_ID:
		case E_MSG_DIR_ID:
		case E_MSG_SUSP_ID:
		case E_MSG_W1_ID:
		case E_MSG_W2_ID:
		case E_MSG_W3_ID:
		case E_MSG_W4_ID:
		{
			loc_result = E_BUS_STATUS_OK;
			break;
		}
		default:
		{
			break;
		}
	}

	return loc_result;
}
