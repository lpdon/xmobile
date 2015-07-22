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

#ifndef PID_H
#define PID_H

#if defined(WIN32)
	#include <stdint.h>
#else
	#include "PE_Types.h"
#endif

#define PID_NUMINST     3
#define PID_MIN         -65000
#define PID_MAX          65000
#define PID_IMAXERROR (PID_MAX >> 3)

typedef enum
{
	E_PID_MOTOR_WHEEL,
	E_PID_MOTOR_STEERING,
	E_PID_MOTOR_SUSPENSION
} ePIDMotor;

typedef struct
{
	uint16_t pFactor;
	uint16_t iFactor;
	uint16_t dFactor;
	int32_t iError;
	int32_t dError;

	struct
	{
		int32_t setpoint;
		int32_t actualValue;
	} input;

	int32_t output;
} tPid;

extern tPid pid[PID_NUMINST];

void pid_init(void);
void pid_cyclic(void);

#endif
