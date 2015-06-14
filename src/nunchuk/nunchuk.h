#ifndef NUNCHUK_H
#define NUNCHUK_H

#define 

typedef struct st_status{
	int8_t 	joystickX;
	int8_t 	joystickY;
	uint8_t buttons;
} joystick;

//updates the data from the nunchuk
uint8_t updateJoystickInput();

//starts the operation of the nunchuk
uint8_t initJoystick();

#endif