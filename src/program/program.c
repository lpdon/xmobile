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

#ifndef PROGRAM_H
	#include "program.h"
#endif

#ifndef BUS_INTERFACE_H
	#include "../bus_interface/bus_interface.h"
#endif

#ifndef COMM_H
	#include "../comm/comm.h"
#endif

#ifndef HANDSHAKE_H
	#include "../handshake/handshake.h"
#endif

#ifndef PID_H
	#include "../pid/pid.h"
#endif

#ifndef DRIVE_H
	#include "../drive/drive.h"
#endif

#ifndef NUNCHUK_H
	#include "../nunchuk/nunchuk.h"
#endif

void program_init(void)
{
	id_init();
	bus_init(E_BUS_TYPE_CAN);
	bus_init(E_BUS_TYPE_UART);
	handshake_init();
	comm_init();
	pid_init();
	drive_init();
	nunchuk_init();
}

void program_cyclic(void)
{
	handshake_cyclic();
	comm_cyclic();
	pid_cyclic();
	drive_cyclic();
}
