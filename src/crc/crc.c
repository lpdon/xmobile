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

#ifndef CRC_H
	#include "crc.h"
#endif

uint8_t crc8 (const uint8_t * const arg_data, const uint8_t arg_length)
{
	uint8_t loc_crc = 0xFFU;
	uint8_t i;

	for (i = 0U; i < arg_length; i++)
	{
		loc_crc = loc_crc ^ arg_data[i];
		if (loc_crc & 0x01U)
		{
			loc_crc = (loc_crc >> 1) ^ CRC_MASK_REV;
		}
		else
		{
			loc_crc = loc_crc >> 1;
		}
	}

	loc_crc = loc_crc ^ 0xFF;

	return loc_crc;
}

eCrcStatus checkCRC(const uint8_t * const arg_data, const uint8_t arg_length, const uint8_t arg_calculatedCRC)
{
	const uint8_t loc_receivedCRC = arg_calculatedCRC;
	const uint8_t loc_calculatedCRC = crc8(arg_data, arg_length);
	const eCrcStatus loc_result = (loc_receivedCRC == loc_calculatedCRC) ? E_CRC_STATUS_OK : E_CRC_STATUS_FAILED;

	return loc_result;
}
