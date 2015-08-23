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

#ifndef MESSAGE_H
#define MESSAGE_H

#ifndef NUNCHUK_H
	#include "../nunchuk/nunchuk.h"
#endif

#define MSG_DATASIZE                 8U

#define MSG_ACK                   0x01U
#define MSG_NACK                  0x02U
#define MSG_IDMASK                0xF0U
#define MSG_REPLYMASK             (MSG_ACK | MSG_NACK)

typedef enum
{
	E_MSG_ACK_OK,
	E_MSG_ACK_FAILED
} eMessageAck;

typedef enum
{
	E_MSG_ID_CONTROL   =        0x10U,
	E_MSG_ID_CURRENT   =        0x20U,
	E_MSG_ID_SUSP      =        0x30U,
	E_MSG_ID_STEERING  =        0x40U,
	E_MSG_ID_WHEEL     =        0x50U
} eMessageId;

typedef enum
{
	E_MSG_STATE_INIT,
	E_MSG_STATE_TX_READY,
	E_MSG_STATE_TRANSMIT,
	E_MSG_STATE_WAITFORACK,
	E_MSG_STATE_END
} eMessageState;

typedef enum
{
	E_MSG_STATUS_ACTIVE,
	E_MSG_STATUS_INACTIVE
} eMessageStatus;

typedef enum
{
	E_MSG_TYPE_CYCLIC,
	E_MSG_TYPE_TRIGGERED
} eMessageType;

typedef enum
{
	E_MSG_BUS_UART,
	E_MSG_BUS_CAN
} eMessageBus;

typedef enum
{
	E_MSG_ACK_ACTIVE,
	E_MSG_ACK_INACTIVE
} eMessageCheckACK;

typedef enum
{
	E_MSG_CRC_ACTIVE,
	E_MSG_CRC_INACTIVE
} eMessageCheckCRC;

typedef enum
{
	E_MSG_TIMEOUT_ACTIVE,
	E_MSG_TIMEOUT_INACTIVE
} eMessageCheckTimeout;

typedef struct
{
	tNunchuk joystickData;
} tMessageControlData;

typedef struct
{
	uint16_t current[4];
} tMessageCurrentData;

typedef struct
{
	int16_t suspension[4];
} tMessageSuspensionData;

typedef struct
{
	int16_t steering[4];
} tMessageSteeringData;

typedef struct
{
	int16_t wheel[4];
} tMessageWheelData;

typedef struct
{
	uint8_t pFactor[4];
	uint8_t iFactor[4];
} tMessageParameterData;

typedef union
{
	uint8_t rawData[MSG_DATASIZE];
} uMessageData;

typedef struct
{
	uint8_t messageId;
	uMessageData data;
	uint8_t crc;
} tMessageBody;

typedef struct
{
	tMessageBody body;
	eMessageState state;
	uint8_t timeout;
	uint8_t retransmissions;
	eMessageStatus status;
	eMessageAck ack;
	eMessageType type;
	eMessageBus bus;
	eMessageCheckACK ackCheck;
	eMessageCheckCRC crcCheck;
	eMessageCheckTimeout timeoutCheck;
} tMessage;

#endif
