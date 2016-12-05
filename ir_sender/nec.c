#include <stdio.h>
#include <string.h>
#include "ir_sender.h"

char* int_to_bin(int a);

int main(int argc, char *argv[])
{
	uint32_t outPin = 22;            // The Broadcom pin number the signal will be sent on
	int frequency = 38000;           // The frequency of the IR signal in Hz
	double dutyCycle = 0.5;          // The duty cycle of the IR signal. 0.5 means for every cycle,
	                                 // the LED will turn on for half the cycle time, and off the other half
	int i,j;
	int result;
	for (i=0; i<1; i++)
	{
		for (j=0; j<256; j++)
		{
			char* address = int_to_bin(i);
			char* command = int_to_bin(j);
			send_nec(outPin, frequency, dutyCycle, address, command);
			free(address);
			free(command);
		}
		//if(result%25 == 0) 
printf("Checkpoint. Current address: %d\n", i);
	}

	return 0;
}

char* int_to_bin(int a)
{
	if (a > 255) return "error";
	else {
		char* toreturn = malloc(sizeof(char)*8);
		int i;
		for (i=0; i<8; i++)
		{
			toreturn[7-i] = (a%2 + '0');
			a/=2;
		}
		return toreturn;
	}
}