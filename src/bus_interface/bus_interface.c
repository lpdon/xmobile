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

eBusStatus bus_writeToBuffer(const eBusType arg_busType, const tCommMessageBody * const arg_messageBody)
{
	eBusStatus loc_result = E_BUS_STATUS_FAILED;

	switch (arg_busType)
	{
		case E_BUS_TYPE_UART:
		{
			const eUartStatus loc_sts_uart = uart_writeToBuffer((uint8_t *)arg_messageBody, sizeof(tCommMessageBody));
			loc_result = (loc_sts_uart == E_UART_STATUS_OK) ? E_BUS_STATUS_OK : E_BUS_STATUS_FAILED;
			break;
		}
		case E_BUS_TYPE_CAN:
		{
			const eCanStatus loc_sts_can = can_writeToBuffer(arg_messageBody->messageId, arg_messageBody->data.rawData, sizeof(uCommMessageData));
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

eBusStatus bus_readFromBuffer(const eBusType arg_busType, tCommMessageBody * const arg_messageBody)
{
	eBusStatus loc_result = E_BUS_STATUS_FAILED;

	switch (arg_busType)
	{
		case E_BUS_TYPE_UART:
		{
			const eUartStatus loc_sts_uart = uart_readFromBuffer((uint8_t *)arg_messageBody, sizeof(tCommMessageBody));
			loc_result = (loc_sts_uart == E_UART_STATUS_OK) ? E_BUS_STATUS_OK : E_BUS_STATUS_FAILED;
			break;
		}
		case E_BUS_TYPE_CAN:
		{
			const eCanStatus loc_sts_can = can_readFromBuffer(arg_messageBody->messageId, arg_messageBody->data.rawData, sizeof(uCommMessageData));
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
			const eUartStatus loc_sts_uart = uart_getDataAvailable();
			loc_result = (loc_sts_uart == E_UART_STATUS_OK) ? E_BUS_STATUS_OK : E_BUS_STATUS_FAILED;
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
