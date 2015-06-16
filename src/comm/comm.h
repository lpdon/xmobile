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

#include <stdint.h>

#define COMM_ACK                   0x01U
#define COMM_NACK                  0x02U
#define COMM_IDMASK                0xF0U
#define COMM_REPLYMASK             (COMM_ACK | COMM_NACK)

/*Timeout in ms*/
#define COMM_TIMEOUT                 10U
#define COMM_MAXRETRANSMISSIONS       3U

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
	E_COMM_MSG_DIR_ID     =        0x40U
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

typedef struct
{
	eCommMessageId messageId;
	uint8_t data[8];
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
} tCommMessage;

extern tCommMessage msgCurrent;
extern tCommMessage msgSuspension;
extern tCommMessage msgDirection;

void comm_init(void);
void comm_end(void);
void comm_cyclic(void);

void comm_receiveMessages(void);
eCommStatus comm_checkCRC(const tCommMessageBody * const arg_messageBody);

#endif
