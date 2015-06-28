/*
 ============================================================================
 Name        : xmobile.c
 Author      : pidgey
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "crc/crc.h"
#include "pid/pid.h"
#include "comm/comm.h"
#include "handshake/handshake.h"
#include "uart_interface/uart_interface.h"

void delay(int milliseconds);

int main(void) {
	puts("Hallo Welt"); /* prints Hallo Welt */

	uint8_t test_crc[] = {1, 2, 3, 4, 5, 6, 7, 8};

	uint8_t crc = crc8(test_crc, sizeof(test_crc));

	printf("crc: %x", crc);

//	int i = 0;
//
//	for (i = 0; i<10; i++)
//	{
//		printf("out: %i \n", (int)(pid(88, 90)));
//	}

//	comm_init();
	uart_init();
	handshake_init();

	while (1)
	{
//		comm_cyclic();
		handshake_cyclic();
		delay(10);
	}


	comm_end();


	return EXIT_SUCCESS;
}

void delay(int milliseconds)
{
    long pause;
    clock_t now,then;

    pause = milliseconds*(CLOCKS_PER_SEC/1000);
    now = then = clock();
    while( (now-then) < pause )
        now = clock();
}
