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

#ifndef ID_H
	#include "id.h"
#endif

#if defined(WIN32)
	#include <stdint.h>
#else
	#include "PE_Types.h"

#ifndef BOARD_ID_H_
	#include "BOARD_ID.h"
#endif

#endif

static eId id = E_ID_UNKNOWN;

void id_init(void)
{
	int16_t loc_id = -1;
#if !defined(_WIN32) && NODE!=CONTROL
	loc_id = BOARD_ID_GetBit(0) | BOARD_ID_GetBit(1)<<1 | BOARD_ID_GetBit(2)<<2 - 1;
#endif

	switch (loc_id)
	{
		case 0:
		{
			id = E_ID_BRAIN;
			break;
		}
		case 1:
		{
			id = E_ID_L1;
			break;
		}
		case 2:
		{
			id = E_ID_L2;
			break;
		}
		case 3:
		{
			id = E_ID_L3;
			break;
		}
		case 4:
		{
			id = E_ID_L4;
			break;
		}
		default:
		{
			id = E_ID_UNKNOWN;
			break;
		}
	}
}

eId id_getId(void)
{
	eId loc_id = (eId)id;
	return loc_id;
}
