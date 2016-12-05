#ifndef IRSLINGER_H
#define IRSLINGER_H

#include <string.h>
#include <math.h>
#include <pigpio.h>
#include <stdlib.h>
#include <stdio.h>
#define MAX_COMMAND_SIZE 512
#define MAX_PULSES 12000



static inline void addPulse(uint32_t onPins, uint32_t offPins, uint32_t duration, gpioPulse_t *irSignal, int *pulseCount)
{
  int index = *pulseCount;

  irSignal[index].gpioOn = onPins;
  irSignal[index].gpioOff = offPins;
  irSignal[index].usDelay = duration;

  (*pulseCount)++;
}

// Generates a square wave for duration (microseconds) at frequency (Hz)
// on GPIO pin outPin. dutyCycle is a floating value between 0 and 1.
static inline void carrierFrequency(uint32_t outPin, double frequency, double dutyCycle, double duration, gpioPulse_t *irSignal, int *pulseCount)
{
  double oneCycleTime = 1000000.0 / frequency; // 1000000 microseconds in a second
  int onDuration = (int)round(oneCycleTime * dutyCycle);
  int offDuration = (int)round(oneCycleTime * (1.0 - dutyCycle));

  int totalCycles = (int)round(duration / oneCycleTime);
  int totalPulses = totalCycles * 2;

  int i;
  for (i = 0; i < totalPulses; i++)
  {
    if (i % 2 == 0)
    {
      // High pulse
      addPulse(1 << outPin, 0, onDuration, irSignal, pulseCount);
    }
    else
    {
      // Low pulse
      addPulse(0, 1 << outPin, offDuration, irSignal, pulseCount);
    }
  }
}

// Generates a low signal gap for duration, in microseconds, on GPIO pin outPin
static inline void gap(uint32_t outPin, double duration, gpioPulse_t *irSignal, int *pulseCount)
{
  addPulse(0, 0, duration, irSignal, pulseCount);
}


//for raws, positive value = turn on for given microseconds
//          negative value = turn off for given microseconds
static inline int ir_send_raw(uint32_t outPin,
    int frequency,
    double dutyCycle,
    const int *pulses,
    int numPulses)
{
  if (outPin > 31)
  {
    // Invalid pin number
    return 1;
  }
  // Generate Code
  gpioPulse_t irSignal[MAX_PULSES];
  int pulseCount = 0;
  int i;
  for (i = 0; i < numPulses; i++)
  {
    if (pulses[i] >= 0) {
      carrierFrequency(outPin, frequency, dutyCycle, pulses[i], irSignal, &pulseCount);
    } else {
      gap(outPin, -pulses[i], irSignal, &pulseCount);
    }
  }

//  printf("pulse count is %i\n", pulseCount);
  // End Generate Code

  // Init pigpio
  if (gpioInitialise() < 0)
  {
    // Initialization failed
    printf("GPIO Initialization failed\n");
    return 1;
  }

  // Setup the GPIO pin as an output pin
  gpioSetMode(outPin, PI_OUTPUT);

  // Start a new wave
  gpioWaveClear();

  gpioWaveAddGeneric(pulseCount, irSignal);
  int waveID = gpioWaveCreate();

  if (waveID >= 0)
  {
    int result = gpioWaveTxSend(waveID, PI_WAVE_MODE_ONE_SHOT);

  //  printf("Result: %i\n", result);
  }
  else
  {
    printf("Wave creation failure!\n %i", waveID);
  }

  // Wait for the wave to finish transmitting
  while (gpioWaveTxBusy())
  {
    time_sleep(0.1);
  }

  // Delete the wave if it exists
  if (waveID >= 0)
  {
    gpioWaveDelete(waveID);
  }

  // Cleanup
  gpioTerminate();
  return 0;
}


//time_slot is the time in microseconds of a bit, header footer can be null
static inline int send_bi_phase(uint32_t outPin,
    int frequency,
    double dutyCycle, int time_slot, int* h, int h_size, char* sequence, int sequence_size, int* f, int f_size)
{
  int* raw_code = (int*)malloc(sizeof(int) * (h_size + sequence_size*2 + f_size));
  int i;
  for (i=0; i<h_size; i++)
  {
    raw_code[i] = h[i];
  }
  //generate codes
  for (i=0; i<sequence_size*2; i++)
  {
    if (sequence[h_size+i] == '0') // go down in the middle of time_slot
    {
      *(raw_code+i + h_size) = time_slot/2 + time_slot%2; // incase it's odd
      *(raw_code+ ++i + h_size) = -time_slot/2; // note this mutator on i
    }
    else if (sequence[i] == '1')
    {
      *(raw_code+i + h_size) = -time_slot/2 + time_slot%2;
      *(raw_code+ ++i + h_size) = time_slot/2;
    }
    else
    {
      // TODO ERROR
    }
  }
  for (i=0; i<f_size; i++)
  {
    raw_code[i+h_size+sequence_size*2] = f[i];
  }
  int output = ir_send_raw(outPin, frequency, dutyCycle, raw_code, h_size+sequence_size*2+f_size);
  free(raw_code);
  return output;
}

// on_length on time in micros
// zero_distance off time for 0 in micros (positive value)
// one_distance on time for 1 in micros
static inline int send_pulse_distance(uint32_t outPin,
    int frequency,
    double dutyCycle,
    int on_length,
    int zero_distance,
    int one_distance,
    int* h, int h_size,
    char* sequence, int sequence_size,
    int* f, int f_size)
{
  int* raw_code = (int*)malloc(sizeof(int) * (h_size + sequence_size * 2 + f_size));
  int i;
  for (i=0; i<h_size; i++)
  {
    raw_code[i] = h[i];
  }
  for (i=0; i<sequence_size; i++)
  {
    *(raw_code+2*i+h_size) = on_length;
    if (sequence[i] == '0')
    {
      *(raw_code+ 2*i+1 + h_size) = -zero_distance;
    }
    else if (sequence[i] == '1')
    {
      *(raw_code+ 2*i+1 + h_size) = -one_distance;
    }
    else
    {
      printf("send_pulse_distance: Supplied code is not 0 or 1");
      // TODO error
    }
  }
  for (i=0; i<f_size; i++)
  {
    raw_code[i+h_size+sequence_size*2] = f[i];
  }
  int output = ir_send_raw(outPin, frequency, dutyCycle, raw_code, h_size + sequence_size*2 + f_size);
  free(raw_code);
  return output;
}

static inline int send_pulse_length(uint32_t outPin,
    int frequency,
    double dutyCycle, 
    int off_length, int zero_burst, int one_burst,
    int* h, int h_size,
    char* sequence, int sequence_size,
    int* f, int f_size)
{
  int* raw_code = (int*)malloc(sizeof(int) * (h_size + sequence_size * 2 + f_size));
  int i;
  for (i=0; i<h_size; i++)
  {
    raw_code[i] = h[i];
  }
  for (i=0; i<sequence_size*2; i++)
  {
    if (sequence[i] == '0')
    {
      *(raw_code+i + h_size) = zero_burst;
    }
    else if (sequence[i] == '1')
    {
      *(raw_code+i + h_size) = one_burst;
    }
    else
    {
      // TODO error
    }
    *(raw_code + ++i + h_size) = -off_length;
  }
  for (i=0; i<f_size; i++)
  {
    raw_code[i+h_size+sequence_size*2] = f[i];
  }
  int output = ir_send_raw(outPin, frequency, dutyCycle, raw_code, h_size + sequence_size*2 + f_size);
  free(raw_code);
  return output;
}


//note default: address_size = 5, data_size = 6
static inline int send_rc5(uint32_t outPin,
    int frequency,
    double dutyCycle, char* address, char* data)
{
  int time_slot = 1780;
  char* command = (char*)malloc(sizeof(char) * 14);
  command[0] = '1'; command[1] = '1'; command[2] = '0';
  memcpy(command+3, address, 5);
  memcpy(command+8, address, 6);
  int output = send_bi_phase(outPin, frequency, dutyCycle, time_slot, NULL, 0, command, 14, NULL, 0);
  free(command); 
  return output;
}

static inline char flip(char a)
{
  if (a == '0') return '1';
  else if (a == '1') return '0';
  else return 'e';
}

//address + data size are both 8 bits
static inline int send_nec(uint32_t outPin,
    int frequency,
    double dutyCycle, char* address, char* data)
{
  int header[2] = {9000,-4500};
  int* size = (int*)malloc(sizeof(int));
  char* total_string = (char*)malloc(sizeof(char)*(8*4));
  int i;
  for (i=0; i<8; i++)
  {
    total_string[i] = address[i];
  }
  for (i=0; i<8; i++)
  {
    total_string[i+8] = flip(address[i]);
  }
  for (i=0; i<8; i++)
  {
    total_string[i+16] = data[i];
  }
  for (i=0; i<8; i++)
  {
    total_string[i+24] = flip(data[i]);
  }
  //header
  int trail[] = {563};
  int out = send_pulse_distance(outPin, frequency, dutyCycle, 563, 562, 1688, header, 2, total_string, 32, trail, 1);
  free(total_string);
  return out; //TODO some better error
}

#endif
