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

#ifndef CAN_INTERFACE_H
#define CAN_INTERFACE_H

#if defined(WIN32)
	#include <stdint.h>
#else
	#include "PE_Types.h"
#endif

#ifndef FIFO_H
	#include "../fifo/fifo.h"
#endif

typedef enum
{
	E_CAN_STATUS_OK,
	E_CAN_STATUS_FAILED
} eCanStatus;

extern tFIFO can_fifo;

eCanStatus can_init(void);
void can_end(void);
eCanStatus can_writeToBuffer(const uint8_t arg_messageId, const uint8_t * const arg_buffer, const uint8_t arg_length);
eCanStatus can_readFromBuffer(uint8_t * const arg_buffer, const uint8_t arg_length);
void can_setDataAvailable(void);
void can_clearDataAvailable(void);
eCanStatus can_getDataAvailable(void);

#endif
