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

#ifndef UART_INTERFACE
	#include "uart_interface.h"
#endif

#if defined(WIN32)
	#include "../uart_windows/rs232.h"
	#define UART_PORT 15
	#define UART_BAUD 9600
	#define UART_MODE "8N1"
#else
	#ifndef __AS1
		#include "AS1.h"
	#endif

	#ifndef __PE_Error_H
		#include "PE_Error.h"
	#endif
#endif

eUartStatus uart_init(void)
{
	eUartStatus loc_result = E_UART_STATUS_FAILED;
	int8_t loc_value = -1;
#if defined(WIN32)
	loc_value = RS232_OpenComport(UART_PORT, UART_BAUD, UART_MODE);
#endif

	loc_result = (loc_value == 0) ? E_UART_STATUS_OK : E_UART_STATUS_FAILED;
	return loc_result;
}

void uart_end(void)
{
#if defined(WIN32)
	RS232_CloseComport(UART_PORT);
#endif
}

eUartStatus uart_writeToBuffer(const uint8_t * const arg_buffer, const uint8_t arg_length)
{
	const uint8_t loc_length = arg_length;

	eUartStatus loc_result = E_UART_STATUS_FAILED;
	uint8_t loc_buffer[loc_length];
	uint16_t loc_totalBytesSent = 0U;

	memcpy(loc_buffer, arg_buffer, loc_length);

#if defined(WIN32)
	loc_totalBytesSent += RS232_SendBuf(UART_PORT, loc_buffer, loc_length);
#else
	AS1_SendBlock(loc_buffer, loc_length, &loc_totalBytesSent);
#endif

	loc_result = (loc_totalBytesSent == loc_length) ? E_UART_STATUS_OK : E_UART_STATUS_FAILED;
	return loc_result;
}

eUartStatus uart_readFromBuffer(const uint8_t * const arg_buffer, const uint8_t arg_length)
{
	const uint8_t loc_length = arg_length;

	eUartStatus loc_result = E_UART_STATUS_FAILED;
	uint8_t loc_buffer[loc_length];
	uint16_t loc_totalBytesRead = 0U;

	memcpy(loc_buffer, arg_buffer, loc_length);

#if defined(WIN32)
	loc_totalBytesRead = RS232_PollComport(UART_PORT, loc_buffer, loc_length);
#else
	AS1_RecvBlock(loc_buffer, loc_length, &loc_totalBytesRead);
#endif

	loc_result = (loc_totalBytesRead == loc_length) ? E_UART_STATUS_OK : E_UART_STATUS_FAILED;
	return loc_result;
}

