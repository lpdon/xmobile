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

#include "crc/crc.h"
#include "pid/pid.h"

int main(void) {
	puts("Hallo Welt"); /* prints Hallo Welt */

	uint8_t test_crc[] = {1, 2, 3, 4, 5, 6, 7, 8};

	uint8_t crc = crc8(test_crc, sizeof(test_crc));

	printf("crc: %x", crc);

	int i = 0;

	for (i = 0; i<10; i++)
	{
		printf("out: %i \n", (int)(pid(88, 90)));
	}


	return EXIT_SUCCESS;
}
