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

#ifndef CAN_INTERFACE
	#include "can_interface.h"
#endif

#if !defined(WIN32)
	#ifndef __CAN1
		#include "CAN1.h"
	#endif

	#ifndef __PE_Error_H
		#include "PE_Error.h"
	#endif
#endif

#include "string.h"

#if !defined(_WIN32)
static eCanStatus can_dataAvailable = E_CAN_STATUS_FAILED;
#else
static eCanStatus can_dataAvailable = E_CAN_STATUS_OK;
#endif

tFIFO can_fifo =
{
	{0x00},
	0U,
	0U,
	0U
};

eCanStatus can_init(void)
{
	eCanStatus loc_result = E_CAN_STATUS_FAILED;
	int8_t loc_value = 0;

	loc_result = (loc_value == 0) ? E_CAN_STATUS_OK : E_CAN_STATUS_FAILED;
	return loc_result;
}

void can_end(void)
{

}

eCanStatus can_writeToBuffer(const uint8_t arg_messageId, const uint8_t * const arg_buffer, const uint8_t arg_length)
{
	eCanStatus loc_result = E_CAN_STATUS_FAILED;

#if !defined(WIN32)
	//byte CAN1_SendFrameExt(dword MessageID,byte FrameType,byte Length,const byte *Data);
	CAN1_SendFrameExt((dword)arg_messageId, DATA_FRAME, arg_length, arg_buffer);
#endif

	loc_result = (arg_length == arg_length) ? E_CAN_STATUS_OK : E_CAN_STATUS_FAILED;
	return loc_result;
}

eCanStatus can_readFromBuffer(uint8_t * const arg_buffer, const uint8_t arg_length)
{
	eCanStatus loc_result = E_CAN_STATUS_FAILED;
	uint16_t loc_totalBytesRead = 0U;
	uint8_t loc_length = arg_length;
	uint8_t loc_buffer[20];

	while (loc_length > 0)
	{
		eFIFOStatus loc_sts_fifo = fifo_out(&can_fifo, &loc_buffer[loc_totalBytesRead]);
		loc_totalBytesRead += (loc_sts_fifo == E_FIFO_STATUS_OK) ? 1U : 0U;
		loc_length--;
	}

	if (loc_totalBytesRead == arg_length)
	{
		memcpy(arg_buffer, loc_buffer, arg_length);
		loc_result = E_CAN_STATUS_OK;
	}

	return loc_result;
}

void can_setDataAvailable(void)
{
	can_dataAvailable = E_CAN_STATUS_OK;
}

void can_clearDataAvailable(void)
{
	can_dataAvailable = E_CAN_STATUS_FAILED;
}

eCanStatus can_getDataAvailable(void)
{
	const eCanStatus loc_result = can_dataAvailable;
	return loc_result;
}
