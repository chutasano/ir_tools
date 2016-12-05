#include <stdio.h>
#include <stdlib.h>
#include "ir_sender.h"

int main(int argc, char *argv[])
{

	uint32_t outPin = 22;            // The Broadcom pin number the signal will be sent on
	int frequency = 38000;          // The frequency of the IR signal in Hz
	double dutyCycle = 0.5;         // The duty cycle of the IR signal. 0.5 means for every cycle,

int i;
int list[67];
for (i=1; i<68; i++)
{
  char* buffer = malloc(sizeof(char)*10);
  int buf_size = 10;
  getline(&buffer, &buf_size, stdin);
  if (i%2 == 1) 
    list[i-1] = atoi(buffer);
  else
    list[i-1] = -atoi(buffer);
  printf("%d, ", list[i-1]);
}



int result = ir_send_raw(outPin, frequency, dutyCycle, list, 67);
	return result;
}
