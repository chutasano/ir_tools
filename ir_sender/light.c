#include <stdio.h>
#include "ir_sender.h"

int main(int argc, char *argv[])
{
	uint32_t outPin = 22;            // The Broadcom pin number the signal will be sent on
	int frequency = 38000;           // The frequency of the IR signal in Hz
	double dutyCycle = 0.5;          // The duty cycle of the IR signal. 0.5 means for every cycle,
	                                 // the LED will turn on for half the cycle time, and off the other half


	int result = send_nec(outPin, frequency, dutyCycle, "00000000", "00000010");
	return result;
}
