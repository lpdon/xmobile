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

#ifndef COMMTYPES_H
#define COMMTYPES_H

#define COMM_DATASIZE                 8U

typedef enum
{
	E_COMM_STATUS_OK,
	E_COMM_STATUS_FAILED
} eCommStatus;

typedef enum
{
	E_COMM_MSG_CONTROL_ID =        0x10U,
	E_COMM_MSG_CURRENT_ID =        0x20U,
	E_COMM_MSG_SUSP_ID    =        0x30U,
	E_COMM_MSG_DIR_ID     =        0x40U,
	E_COMM_MSG_W1_ID      =        0x50U,
	E_COMM_MSG_W2_ID      =        0x60U,
	E_COMM_MSG_W3_ID      =        0x70U,
	E_COMM_MSG_W4_ID      =        0x80U
} eCommMessageId;

typedef enum
{
	E_COMM_MSG_STATE_INIT,
	E_COMM_MSG_STATE_TX_READY,
	E_COMM_MSG_STATE_TRANSMIT,
	E_COMM_MSG_STATE_WAITFORACK,
	E_COMM_MSG_STATE_END
} eCommMessageState;

typedef enum
{
	E_COMM_MSG_STATUS_ACTIVE,
	E_COMM_MSG_STATUS_INACTIVE
} eCommMessageStatus;

typedef enum
{
	E_COMM_MSG_TYPE_CYCLIC,
	E_COMM_MSG_TYPE_TRIGGERED
} eCommMessageType;

typedef enum
{
	E_COMM_MSG_BUS_UART,
	E_COMM_MSG_BUS_CAN
} eCommMessageBus;

typedef struct
{
	uint16_t current[4];
} tCommCurrentData;

typedef struct
{
	uint8_t suspension[8];
} tCommSuspensionData;

typedef struct
{
	uint8_t direction[8];
} tCommDirectionData;

typedef union
{
	uint8_t rawData[COMM_DATASIZE];
//	tCommCurrentData currentData;
//	tCommSuspensionData suspensionData;
//	tCommDirectionData directionData;
} uCommMessageData;

typedef struct
{
	uint8_t messageId;
	uCommMessageData data;
	uint8_t crc;
} tCommMessageBody;

typedef struct
{
	tCommMessageBody body;
	eCommMessageState state;
	uint8_t timeout;
	uint8_t retransmissions;
	eCommMessageStatus status;
	eCommStatus ack;
	eCommMessageType type;
	eCommMessageBus bus;
} tCommMessage;

#endif
