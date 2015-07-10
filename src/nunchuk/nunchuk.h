#ifndef NUNCHUK_H
#define NUNCHUK_H

#if defined(WIN32)
	#include <stdint.h>
#else
	#include "PE_Types.h"
#endif

typedef struct
{
	int8_t 	joystickX;
	int8_t 	joystickY;
	uint8_t buttons;
} joystick;

extern joystick nunchuk;

//starts the operation of the nunchuk
uint8_t nunchuk_init();

//updates the data from the nunchuk
uint8_t nunchuk_cyclic();

#endif
