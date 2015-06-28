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
#define HANDSHAKE_H

#if defined(WIN32)
	#include <stdint.h>
#else
	#include "PE_Types.h"
#endif

#define HANDSHAKE_ACK                   0x01U
#define HANDSHAKE_NACK                  0x02U
#define HANDSHAKE_IDMASK                0xF0U
#define HANDSHAKE_REPLYMASK             (HANDSHAKE_ACK | HANDSHAKE_NACK)

#define HANDSHAKE_DATASIZE                 8U
#define HANDSHAKE_CYCLETIME              100U
#define HANDSHAKE_RETRANSMISSIONS        100U

typedef enum
{
	E_HANDSHAKE_STATUS_OK,
	E_HANDSHAKE_STATUS_FAILED
} eHandshakeStatus;

typedef enum
{
	E_HANDSHAKE_MSG_STATUS_ACTIVE,
	E_HANDSHAKE_MSG_STATUS_INACTIVE
} eHandshakeMessageStatus;

typedef enum
{
	E_HANDSHAKE_RQST_ID     =     0xF0U,
	E_HANDSHAKE_RESPONSE_ID =     0xD0U
} eHandshakeMessageId;

typedef enum
{
	E_HANDSHAKE_STATE_INIT,
	E_HANDSHAKE_STATE_TX_READY,
	E_HANDSHAKE_STATE_TRANSMIT,
	E_HANDSHAKE_STATE_WAITFORACK,
	E_HANDSHAKE_STATE_END_SUCCESS
} eHandshakeMessageState;

typedef union
{
	uint8_t rawData[HANDSHAKE_DATASIZE];
//	tCommCurrentData currentData;
//	tCommSuspensionData suspensionData;
//	tCommDirectionData directionData;
} uHandshakeMessageData;

typedef struct
{
	uint8_t messageId;
	uHandshakeMessageData data;
	uint8_t crc;
} tHandshakeMessageBody;

typedef struct
{
	uint8_t initial;
	uint8_t current;
} tHandshakeMessageCycleTime;

typedef struct
{
	tHandshakeMessageBody body;
	eHandshakeMessageState state;
	tHandshakeMessageCycleTime cycleTime;
	uint8_t retransmissions;
	eHandshakeMessageStatus status;
	eHandshakeStatus ack;
} tHandshakeMessage;

void handshake_init(void);
void handshake_cyclic(void);

eHandshakeStatus handshake_getStatus(void);

#endif
