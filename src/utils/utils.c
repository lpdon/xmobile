/*The MIT License (MIT)

Copyright (c) 2015 Marcelo Chimentao, Leandro Piekarski do Nascimento, Matthias Leiter

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

#ifndef UTILS_H
	#include "utils.h"
#endif

int32_t utils_cos(int32_t arg_x)
{
	int32_t x = arg_x;
	return(1000 - (x*x / (2*1000)) + (((x*x/1000) * (x*x/1000))/24/1000 ));  // Taylor: 1 - x^2/2! + x^4/4!
}

int32_t utils_sin(int32_t arg_x)
{
	int32_t x = arg_x;
	int32_t arg = 0;
  
	arg = x - ((((x*x)/1000)*x)/6000) + ((((((x*x*x)/1000)*x)/1000) *x)/12000000);
  
	return(arg);   // Taylor: x -x^3/3!+x^5/5! 
}

int32_t utils_atan(int32_t arg_x)
{
	int32_t x = arg_x;
	int32_t arg=0;
   
	if(x <= 1000 && x > -1000)
	{
		arg = x * 1000 / (1000 + 280 * x*x / 1000000);  // lw in mRad
	}    
	if(x >  1000)
	{
		arg = 1570 - (x*1000 / ( x*x/1000 + 280));      // lw in mRad
	}
	
	if(x <= -1000)   
	{
	    arg = -1570 - (x*1000 / ( x*x/1000 + 280));     // lw in mRad
	}   
	
	return(arg); 
}

int32_t utils_tan(int32_t arg_x)
{
	int32_t x = arg_x;
	int32_t arg=0;

	if(x <= 1000 && x > -1000)
		arg = x * 1000 / (1000 + 280 * x*x / 1000000);  // lw in mRad
		
	if(x >  1000)
		arg = 1570 - (x*1000 / ( x*x/1000 + 280));      // lw in mRad

	if(x <= -1000)   
		arg = -1570 - (x*1000 / ( x*x/1000 + 280));     // lw in mRad

	return(arg); 
}
