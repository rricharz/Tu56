/*
 * demoread.c
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
//#include <string.h>
//#include <fcntl.h>
//#include <sys/stat.h>
//#include <sys/types.h>
#include <unistd.h>

int getStatus()
{
	static FILE *statusFile = 0;
	char *fname = "/tmp/tu56status";
	int st;
	
	// file needs to be opened again each time to read new status
	statusFile = fopen(fname, "r");
		
	if (statusFile != 0) {
		st = getc(statusFile) - 32;
		fclose(statusFile);
		if (st >= 0)
			return st;
		else
			return 0;
	}
	else return 0;
}

int main(int argc, char **argv)
{
	int status;
	int lastStatus = 0;
	
	// open the status file in /tmp

	
	while (1) {
		status = getStatus();
		if ((status >= 0) && (lastStatus != status)) {
			printf("Status is 0x%02x\n", status);
			lastStatus = status;
		}

	}	
	
	return 0;
}

