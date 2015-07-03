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

#ifndef BUS_INTERFACE_H
#define BUS_INTERFACE_H

#if defined(WIN32)
	#include <stdint.h>
#else
	#include "PE_Types.h"
#endif

#ifndef COMMTYPES_H
	#include "../comm/comm_types.h"
#endif

typedef enum
{
	E_BUS_STATUS_OK,
	E_BUS_STATUS_FAILED
} eBusStatus;

typedef enum
{
	E_BUS_TYPE_UART = E_COMM_MSG_BUS_UART,
	E_BUS_TYPE_CAN = E_COMM_MSG_BUS_CAN
} eBusType;

eBusStatus bus_init(eBusType arg_busType);
void bus_end(eBusType arg_busType);
eBusStatus bus_writeToBuffer(const eBusType arg_busType, const tCommMessageBody * const arg_messageBody);
eBusStatus bus_readFromBuffer(const eBusType arg_busType, tCommMessageBody * const arg_messageBody);
void bus_setDataAvailable(eBusType arg_busType);
void bus_clearDataAvailable(eBusType arg_busType);
eBusStatus bus_getDataAvailable(eBusType arg_busType);

#endif
