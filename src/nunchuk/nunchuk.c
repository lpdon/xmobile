#ifndef NUNCHUK
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
//	__DI();
//	error_code = EI2C1_SendBlock(init_block, 2, &length);
//	error_code = EI2C1_SendBlock(init_block+2, 2, &length);
//	__EI();
#endif
	return error_code;
}

uint8_t nunchuk_cyclic()
{
	uint8_t received[6];
	uint16_t length = 0U;
	uint16_t error_code = 0U;
	int16_t loc_x = 0;
	int16_t loc_y = 0;
	uint8_t loc_buffer[2];

	if (nunchukCycletime == 0U)
	{
		volatile int8_t loc_scalX = 0;
		volatile int8_t loc_scalY = 0;
#if !defined(WIN32)
//		__DI();
//		error_code = EI2C1_SendChar(0x00);
//		error_code = EI2C1_RecvBlock(received,6,&length);
//		__EI();
		AD1_Measure(1);
		AD1_GetValue8(loc_buffer);
		loc_x = ((int16_t)loc_buffer[1U]) - 127;
		loc_y = ((int16_t)loc_buffer[0U]) - 127;
#endif
//		if(length>5){
//			nunchuk.x = (received[0] - 0x1a)-100;
//			nunchuk.y = (received[1] - 0x1d)-100;
//			nunchuk.buttons = ((received[5] & 0x02) ? 0 : 1)<<1;
//			nunchuk.buttons += (received[5] & 0x01) ? 0 : 1;
//		}

		loc_scalX = (int8_t)((loc_x * 100) / 127);
		loc_scalY = (int8_t)((loc_y * 100) / 127);

		nunchuk.joystickX = loc_scalX;
		nunchuk.joystickY = loc_scalY;

		nunchukCycletime = NUNCHUK_CYCLETIME;
	}
	else
	{
		nunchukCycletime--;
	}

	return error_code;
}
