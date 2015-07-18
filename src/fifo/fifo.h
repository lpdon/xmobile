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

#ifndef FIFO_H
#define FIFO_H

#if defined(WIN32)
	#include <stdint.h>
#else
	#include "PE_Types.h"
#endif

#define FIFO_BUFFER_SIZE 128

typedef enum
{
	E_FIFO_STATUS_OK,
	E_FIFO_STATUS_FAILED
} eFIFOStatus;

typedef struct
{
	uint8_t buffer[FIFO_BUFFER_SIZE];
	uint8_t read_index;
	uint8_t write_index;
} tFIFO;

uint8_t fifo_in(tFIFO* arg_fifo, uint8_t arg_byte);
uint8_t fifo_out(tFIFO* arg_fifo, uint8_t* arg_byte);

#endif
