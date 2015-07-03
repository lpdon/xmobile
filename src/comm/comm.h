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

#ifndef COMMTYPES_H
	#include "comm_types.h"
#endif

#define COMM_ACK                   0x01U
#define COMM_NACK                  0x02U
#define COMM_IDMASK                0xF0U
#define COMM_REPLYMASK             (COMM_ACK | COMM_NACK)

#define COMM_TIMEOUT                 10U
#define COMM_MAXRETRANSMISSIONS       3U

extern tCommMessage msgCurrent;
extern tCommMessage msgSuspension;
extern tCommMessage msgDirection;
extern tCommMessage msgW1;
extern tCommMessage msgW2;
extern tCommMessage msgW3;
extern tCommMessage msgW4;

void comm_setData(eCommMessageId arg_id, const void * const arg_data);

void comm_init(void);
void comm_end(void);
void comm_cyclic(void);

//void comm_receiveMessages(void);
void comm_kickoutMessage(tCommMessage * const arg_message);

#endif
