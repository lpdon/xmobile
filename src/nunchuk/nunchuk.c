#ifndef NUNCHUK
	#include "nunchuk.h"
#endif

joystick nunchuk;

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
	error_code = EI2C1_SendBlock(init_block+2, 2, &length);
#endif
	return error_code;
}

uint8_t nunchuk_cyclic()
{
	uint8_t received[6];
	uint16_t length = 0U;
	uint16_t error_code = 0U;

#if !defined(WIN32)
	error_code = EI2C1_SendChar(0x00);
	error_code = EI2C1_RecvBlock(received,6,&length);
#endif

	if(length>5){
		nunchuk.joystickX = (received[0] - 0x1a)-100;
		nunchuk.joystickY = (received[1] - 0x1d)-100;
		nunchuk.buttons = ((received[5] & 0x02) ? 0 : 1)<<1;
		nunchuk.buttons += (received[5] & 0x01) ? 0 : 1;
	}

	return error_code;

}
