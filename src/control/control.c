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

#ifndef CONTROL_H
	#include "control.h"
#endif

#ifndef COMM_H
	#include "../comm/comm.h"
#endif

#ifndef NUNCHUK_H
	#include "../nunchuk/nunchuk.h"
#endif

#ifndef MESSAGE_H
	#include "../comm/message.h"
#endif

void control_init(void)
{

}

void control_cyclic(void)
{
	const int8_t loc_x = nunchuk.joystickX;
	const int8_t loc_y = nunchuk.joystickY;
	const uint8_t loc_buttons = nunchuk.buttons;
	tMessageControlData loc_ctrlData;

	loc_ctrlData.joystickData.joystickX = loc_x;
	loc_ctrlData.joystickData.joystickY = loc_y;
	loc_ctrlData.joystickData.buttons = loc_buttons;
	comm_setData(E_MSG_ID_CONTROL, &loc_ctrlData, sizeof(tMessageControlData));
}
