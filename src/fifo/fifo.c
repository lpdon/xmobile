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
	#include "fifo.h"
#endif

uint8_t fifo_in(tFIFO* arg_fifo, uint8_t arg_byte){
	if(arg_fifo->read_index == arg_fifo->write_index + 1 || 
		(arg_fifo->read_index == 0 && arg_fifo->write_index + 1 == FIFO_BUFFER_SIZE)){
		return E_FIFO_STATUS_FAILED;
	}	   		
	
	arg_fifo->buffer[arg_fifo->write_index++] = arg_byte;

	if(arg_fifo->write_index >= FIFO_BUFFER_SIZE)
		arg_fifo->write_index = 0;		
	
	arg_fifo->counter++;

	return E_FIFO_STATUS_OK;
};

uint8_t fifo_out(tFIFO* arg_fifo, uint8_t* arg_byte){
	if(arg_fifo->write_index == arg_fifo->read_index)
		return E_FIFO_STATUS_FAILED;

	*arg_byte = arg_fifo->buffer[arg_fifo->read_index++];
	
	if(arg_fifo->read_index >= FIFO_BUFFER_SIZE)
		arg_fifo->read_index = 0;

	arg_fifo->counter--;

	return E_FIFO_STATUS_OK;
};
