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
	#include "pid.h"
#endif

/*Timestamp of the pid in ms*/
uint16_t pid_timestep = 10U;

uint16_t pid_pFactor = 1U;
uint16_t pid_iFactor = 1U;
uint16_t pid_dFactor = 1U;

static const uint8_t PID_OFFSET = 50U;

/*Accumulated error ~ integral*/
static int16_t pid_iError;

/*Differential error ~ derivative*/
static int16_t pid_dError;

static int16_t pid_calcI(int16_t arg_error)
{
	const uint16_t loc_iFactor = pid_iFactor;
	const int16_t loc_error = (arg_error + pid_iError);
	int16_t loc_value;

	loc_value = loc_iFactor * loc_error;

	//updates accumulated error
	pid_iError = pid_iError + arg_error;

	return loc_value;
}

static int16_t pid_calcD(int16_t arg_error)
{
	const uint16_t loc_dFactor = pid_dFactor;
	const int16_t loc_error = (arg_error - pid_dError);
	int16_t loc_value;

	loc_value = loc_dFactor * loc_error;

	//updates stored error
	pid_dError = arg_error;

	return loc_value;
}

uint8_t pid(uint16_t arg_setpoint, uint16_t arg_actualValue)
{
	const int16_t loc_error = arg_setpoint - arg_actualValue;

	const int16_t loc_iTerm = pid_calcI(loc_error);
	const int16_t loc_dTerm = pid_calcD(loc_error);
	int16_t loc_value;

	loc_value = pid_pFactor * (loc_error + loc_iTerm + loc_dTerm);

	//loc_value = loc_value + loc_offset;
	if (loc_value < 0)
	{
		loc_value = PID_MAX + loc_value;
	}

	/*Limit output*/
	if (loc_value > PID_MAX)
	{
		loc_value = PID_MAX;
	}
	else if (loc_value < PID_MIN)
	{
		loc_value = PID_MIN;
	}

	return (uint8_t)(loc_value);
}
