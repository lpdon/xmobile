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

#ifndef NUNCHUK_H
	#include "nunchuk.h"
#endif

static uint8_t nunchukCycletime = NUNCHUK_CYCLETIME;

tNunchuk nunchuk =
{
	0,
	0,
	0U
};

//don't forget to initialize the I2C before using this function
uint8_t nunchuk_init()
{
	uint8_t init_block[4] = {0xf0, 0x55, 0xfb, 0x00};
	uint16_t length = 0U;
	uint8_t error_code = 0U;

	nunchuk.joystickX = 0;
	nunchuk.joystickY = 0;

#if !defined(WIN32)
	error_code = EI2C1_SendBlock(init_block, 2, &length);
	EI2C1_SendStop();
	error_code = EI2C1_SendBlock(init_block+2, 2, &length);
	EI2C1_SendStop();
#endif
	return error_code;
}

uint8_t nunchuk_cyclic()
{
	uint8_t received[6];
	uint16_t length = 0U;
	uint8_t error_code = 0U;

	if (nunchukCycletime == 0U)
	{
#if !defined(WIN32)
		error_code = EI2C1_SendChar(0x00);
		EI2C1_SendStop();
		error_code = EI2C1_RecvBlock(received,6,&length);
		EI2C1_SendStop();
#endif
		if(length>5){
			nunchuk.joystickX = (received[0] - 0x1a)-100;
			nunchuk.joystickY = (received[1] - 0x1d)-100;
			nunchuk.buttons = ((received[5] & 0x02) ? 0 : 1)<<1;
			nunchuk.buttons += (received[5] & 0x01) ? 0 : 1;
		}
		nunchukCycletime = NUNCHUK_CYCLETIME;
	}
	else
	{
		nunchukCycletime--;
	}

	return error_code;
}
