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

#ifndef SENSOR_INTERFACE_H
	#include "sensor_interface.h"
#endif

#if !defined(_WIN32)
	#ifndef __AD1
		#include "AD1.h"
	#endif
#endif

uint8_t sensor[SENSOR_NUMINST];

void sensor_init(void)
{
#if NODE!=CONTROL && NODE!=MASTER
	AD1_Measure(0);
	//AD1_Enable();
	//AD1_Start();
#endif
}

void sensor_cyclic(void)
{
#if NODE!=CONTROL && NODE!=MASTER
	AD1_GetChanValue8(E_SENSOR_CURRENT, &sensor[E_SENSOR_CURRENT]);
	AD1_GetChanValue8(E_SENSOR_STEERING, &sensor[E_SENSOR_STEERING]);
	AD1_GetChanValue8(E_SENSOR_SUSPENSION_SPRING, &sensor[E_SENSOR_SUSPENSION_SPRING]);
	AD1_GetChanValue8(E_SENSOR_SUSPENSION_JOINT, &sensor[E_SENSOR_SUSPENSION_JOINT]);
	AD1_Measure(1);
#endif
}
