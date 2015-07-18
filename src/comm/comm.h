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
#define COMM_H

#if defined(WIN32)
	#include <stdint.h>
#else
	#include "PE_Types.h"
#endif

#ifndef MESSAGE_H
	#include "message.h"
#endif

#define COMM_TIMEOUT                   10U
#define COMM_MAXRETRANSMISSIONS        3U

typedef enum
{
	E_COMM_STATUS_OK,
	E_COMM_STATUS_FAILED
} eCommStatus;

extern tMessage msgControl;
extern tMessage msgCurrent;
extern tMessage msgSuspension;
extern tMessage msgSteering;
extern tMessage msgWheel;
extern tMessage msgW1;
extern tMessage msgW2;
extern tMessage msgW3;
extern tMessage msgW4;

void comm_setData(eMessageId arg_id, const void * const arg_data, const uint8_t arg_size);
void comm_getData(eMessageId arg_id, void * const arg_data, const uint8_t arg_size);

void comm_init(void);
void comm_end(void);
void comm_cyclic(void);

//void comm_receiveMessages(void);
void comm_kickoutMessage(tMessage * const arg_message);

#endif
