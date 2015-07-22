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

#include <string.h>

tPid pid[PID_NUMINST];

static int32_t pid_calcI(tPid * const arg_pid, int32_t arg_error);
static int32_t pid_calcD(tPid * const arg_pid, int32_t arg_error);

static void pid_initInst(tPid * const arg_pid);
static void pid_updateValues(tPid * const arg_pid);

void pid_init(void)
{
	uint8_t i;

	for(i = 0U; i < PID_NUMINST; i++)
	{
		pid_initInst(&pid[i]);
	}
}

void pid_cyclic(void)
{
	uint8_t i;

	for(i = 0U; i < PID_NUMINST; i++)
	{
		pid_updateValues(&pid[i]);
	}
}

void pid_initInst(tPid * const arg_pid)
{
	memset(arg_pid, 0, sizeof(tPid));
}

int32_t pid_calcI(tPid * const arg_pid, int32_t arg_error)
{
	const uint32_t loc_iFactor = arg_pid->iFactor;
	const volatile int32_t loc_error = (arg_error + arg_pid->iError);
	volatile int32_t loc_value;

	loc_value = loc_iFactor * loc_error;

	//updates accumulated error
	arg_pid->iError = arg_pid->iError + arg_error;

	if (arg_pid->iError > PID_IMAXERROR)
	{
		arg_pid->iError = PID_IMAXERROR;
	}
	else if (arg_pid->iError < -PID_IMAXERROR)
	{
		arg_pid->iError = -PID_IMAXERROR;
	}

	return loc_value >> 3;
}

int32_t pid_calcD(tPid * const arg_pid, int32_t arg_error)
{
	const uint32_t loc_dFactor = arg_pid->dFactor;
	const int32_t loc_error = (arg_error - arg_pid->dError);
	int32_t loc_value;

	loc_value = loc_dFactor * loc_error;

	//updates stored error
	arg_pid->dError = arg_error;

	return loc_value;
}

void pid_updateValues(tPid * const arg_pid)
{
	const int32_t loc_error = arg_pid->input.setpoint - arg_pid->input.actualValue;

	const int32_t loc_iTerm = pid_calcI(arg_pid, loc_error);
	const int32_t loc_dTerm = pid_calcD(arg_pid, loc_error);
	int32_t loc_value;

	loc_value = arg_pid->pFactor * (loc_error + loc_iTerm + loc_dTerm);

	/*Limit output*/
	if (loc_value > PID_MAX)
	{
		loc_value = PID_MAX;
	}
	else if (loc_value < PID_MIN)
	{
		loc_value = PID_MIN;
	}

	arg_pid->output = loc_value;
}
