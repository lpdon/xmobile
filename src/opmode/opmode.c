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

#ifndef OPMODE_H
	#include "opmode.h"
#endif

#ifndef WIN32
  #include "PE_Types.h"
#endif

static const uint32_t WATCHDOG_LIMIT = 100U;
static uint32_t opmode_watchdog = 0U;                
static eOpmode opmode_activeMode = E_OPMODE_SHUTDOWN;

void opmode_init(void)
{
	opmode_activeMode = E_OPMODE_NORMAL;
}

void opmode_cyclic(void) 
{
  opmode_watchdog++;  
  
  if (opmode_watchdog >= WATCHDOG_LIMIT)
    opmode_activeMode = E_OPMODE_SHUTDOWN;
  else
    opmode_activeMode = E_OPMODE_NORMAL;
}

void opmode_resetWatchdog(void) 
{
  opmode_watchdog = 0U;  
}

void opmode_setMode(const eOpmode arg_mode)
{
	opmode_activeMode = arg_mode;
}

const eOpmode opmode_getActiveMode(void)
{
	const eOpmode loc_mode = opmode_activeMode;
	return loc_mode;
}
