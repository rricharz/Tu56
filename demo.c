/*
 * demo.c
 *
 * Runs a demo on the tu56 visual display of the DECtape TU56 front panel
 * 
 * for the Raspberry Pi and other Linux systems
 * 
 * Copyright 2019  rricharz
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TSTATE_ONLINE		 1		// Turns the online light on
#define TSTATE_DRIVE1		 2		// Selects drive 0 or 1
#define TSTATE_BACKWARDS	 4		// Sets the direction
#define TSTATE_SEEK		 	 8		// Spins the reels
#define TSTATE_READ			16		// Spins the reels
#define TSTATE_WRITE		32		// Turns the write light on and spins the reels

static FILE *statusFile = 0;

void setStatus(int status)
// set the status bits in the status file
// if status file is accessible
{
	
	char *fname = "/tmp/tu56status";
	
	if (statusFile == 0) {
		statusFile = fopen(fname, "w");
	}

	if (statusFile != 0) {
		fseek(statusFile,0L,SEEK_SET);
		putc(32 + status,statusFile);
		fflush(statusFile);
	}
}

int main(int argc, char **argv)
{	
	system("pkill mpg321");
	setStatus(0);
	sleep(1);
	setStatus(TSTATE_ONLINE | TSTATE_BACKWARDS | TSTATE_SEEK);
	sleep(2);
	setStatus(TSTATE_ONLINE | TSTATE_SEEK);
	usleep(500000);
	sleep(1);
	setStatus(TSTATE_ONLINE | TSTATE_BACKWARDS | TSTATE_SEEK | TSTATE_DRIVE1);
	sleep(2);
	setStatus(TSTATE_ONLINE | TSTATE_SEEK | TSTATE_DRIVE1);
	usleep(500000);
	for (int i = 1; i < 10; i++) {
		setStatus(TSTATE_ONLINE);
		usleep(500000);
		setStatus(TSTATE_ONLINE | TSTATE_READ);
		sleep(3);
		setStatus(TSTATE_ONLINE | TSTATE_DRIVE1);
		usleep(500000);
		setStatus(TSTATE_ONLINE | TSTATE_WRITE | TSTATE_DRIVE1);
		sleep(3);
	}
	setStatus(0);
	fclose(statusFile);	
	return 0;
}

