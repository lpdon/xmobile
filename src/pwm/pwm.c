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

#ifndef PWM_H
	#include "pwm.h"
#endif

#if !defined(WIN32)
	#ifndef __PWM_STEERING_LEFT
		#include "PWM_STEERING_LEFT.h"
	#endif

	#ifndef __PWM_STEERING_RIGHT
		#include "PWM_STEERING_RIGHT.h"
	#endif

	#ifndef __PWM_SUSPENSION_UP
		#include "PWM_SUSPENSION_UP.h"
	#endif

	#ifndef __PWM_SUSPENSION_DOWN
		#include "PWM_SUSPENSION_DOWN.h"
	#endif

	#ifndef __PWM_WHEEL_FORWARDS
		#include "PWM_WHEEL_FORWARDS.h"
	#endif

	#ifndef __PWM_WHEEL_BACKWARDS
		#include "PWM_WHEEL_BACKWARDS.h"
	#endif

	#ifndef __PE_Error_H
		#include "PE_Error.h"
	#endif
#endif

#include <math.h>

void pwm_setSignal(ePWMMotor arg_motor, int32_t arg_pwm)
{
#if !defined(_WIN32)
	uint16_t loc_absPwm = (uint16_t)abs(arg_pwm);
	uint16_t loc_pwm1 = 0U;
	uint16_t loc_pwm2 = 0U;

	if (arg_pwm > 0U)
	{
		loc_pwm1 = loc_absPwm;
	}
	else
	{
		loc_pwm2 = loc_absPwm;
	}

	switch (arg_motor)
	{
		case E_PWM_MOTOR_STEERING:
		{
			//PT4 PP5
			PWM_STEERING_LEFT_SetRatio16(~loc_pwm2);
			PWM_STEERING_RIGHT_SetRatio16(~loc_pwm1);
			break;
		}
		case E_PWM_MOTOR_SUSPENSION:
		{
			//PT2 PT3
			PWM_SUSPENSION_UP_SetRatio16(~loc_pwm1);
			PWM_SUSPENSION_DOWN_SetRatio16(~loc_pwm2);
			break;
		}
		case E_PWM_MOTOR_WHEEL:
		{
			//PT0 PT1
			PWM_WHEEL_FORWARDS_SetRatio8(loc_pwm1);
			PWM_WHEEL_BACKWARDS_SetRatio8(loc_pwm2);
			
			if (loc_pwm1 == 0 && loc_pwm2 == 0) 
			{
			  ENABLE_WHEEL_PutVal(0);  
			} 
			else 
			{
			  ENABLE_WHEEL_PutVal(1);
			}
			 
			break;
		}
		default:
		{
			break;
		}
	}
#endif
}
