#include <cstdio>
#include <string>
#include "ir_sender.h"

std::string int_to_bin(int a);

int main(int argc, char *argv[])
{
  uint32_t outPin = 22;            // The Broadcom pin number the signal will be sent on
  int frequency = 38000;           // The frequency of the IR signal in Hz
  double dutyCycle = 0.5;          // The duty cycle of the IR signal. 0.5 means for every cycle,
  // the LED will turn on for half the cycle time, and off the other half
  IrSender sender(outpin, frequency, dutyCycle);
  int result;
  for (int i=0; i<256; i++)
  {
    for (int j=0; j<256; j++)
    {
      std::string address = int_to_bin(i);
      std::string command = int_to_bin(j);
      send_nec(outPin, frequency, dutyCycle, address, command);
    }
    //if(result%25 == 0) 
    printf("Checkpoint. Current address: %d\n", i);
  }

  return 0;
}

std::string int_to_bin(int a)
{
  if (a > 255) return "error";
  else {
    std::string toreturn;
    int i;
    for (i=0; i<8; i++)
    {
      toreturn[7-i] = (a%2 + '0');
      a/=2;
    }
    return toreturn;
  }
}
