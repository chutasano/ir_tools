#include <stdio>
#include <stdlib>
#include <vector>
#include "ir_sender.h"

int main(int argc, char *argv[])
{
  int size=57;
  uint32_t outPin = 22;            // The Broadcom pin number the signal will be sent on
  int frequency = 38000;          // The frequency of the IR signal in Hz
  double dutyCycle = 0.5;         // The duty cycle of the IR signal. 0.5 means for every cycle,
  IrSender(outpin, frequency, dutyCycle);

  std::vector<int> list;
  for (int i=0; i<size; i++)
  {
    char* buffer = malloc(sizeof(char)*10);
    int buf_size = 10;
    getline(&buffer, &buf_size, stdin);
    if (i%2 == 0) 
      list.push_back(atoi(buffer));
    else
      list.push_back(-atoi(buffer));
    printf("%d, ", list[i]);
  }

  int result = ir_send_raw(outPin, frequency, dutyCycle, list);
  return result;
}
