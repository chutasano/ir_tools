#ifndef IRSLINGER_H
#define IRSLINGER_H

#include <string.h>
#include <math.h>
#include <pigpio.h>

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

static inline int irSling(uint32_t outPin,
    int frequency,
    double dutyCycle,
    int leadingPulseDuration,
    int leadingGapDuration,
    int onePulse,
    int zeroPulse,
    int oneGap,
    int zeroGap,
    int sendTrailingPulse,
    const char *code)
{
  if (outPin > 31)
  {
    // Invalid pin number
    return 1;
  }

  size_t codeLen = strlen(code);

  printf("code size is %zu\n", codeLen);

  if (codeLen > MAX_COMMAND_SIZE)
  {
    // Command is too big
    return 1;
  }

  gpioPulse_t irSignal[MAX_PULSES];
  int pulseCount = 0;

  // Generate Code
  carrierFrequency(outPin, frequency, dutyCycle, leadingPulseDuration, irSignal, &pulseCount);
  gap(outPin, leadingGapDuration, irSignal, &pulseCount);

  int i;
  for (i = 0; i < codeLen; i++)
  {
    if (code[i] == '0')
    {
      carrierFrequency(outPin, frequency, dutyCycle, zeroPulse, irSignal, &pulseCount);
      gap(outPin, zeroGap, irSignal, &pulseCount);
    }
    else if (code[i] == '1')
    {
      carrierFrequency(outPin, frequency, dutyCycle, onePulse, irSignal, &pulseCount);
      gap(outPin, oneGap, irSignal, &pulseCount);
    }
    else
    {
      printf("Warning: Non-binary digit in command\n");
    }
  }

  if (sendTrailingPulse)
  {
    carrierFrequency(outPin, frequency, dutyCycle, onePulse, irSignal, &pulseCount);
  }

  printf("pulse count is %i\n", pulseCount);
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

    printf("Result: %i\n", result);
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

  printf("pulse count is %i\n", pulseCount);
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

    printf("Result: %i\n", result);
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
int* generate_bi_phase(int* new_size, int time_slot, char* sequence, int sequence_size)
{
  int* raw_code = malloc(sizeof(int) * sequence_size*2);

  //generate codes
  for (int i; i<sequence_size*2; i++)
  {
    if (sequence[i] == '0') // go down in the middle of time_slot
    {
      (raw_code+i)* = time_slot/2 + time_slot%2; // incase it's odd
      (raw_code+ ++i)* = -time_slot/2; // note this mutator on i
    }
    else if (sequence[i] == '1')
    {
      (raw_code+i)* = -time_slot/2 + time_slot%2;
      (raw_code+ ++i)* = time_slot/2;
    }
    else
    {
      // TODO ERROR
    }
  }
  new_size* = sequence_size * 2;
  return raw_code;
}

// on_length on time in micros
// zero_distance off time for 0 in micros (positive value)
// one_distance on time for 1 in micros
int* generate_pulse_distance(int* new_size, int on_length, int zero_distance, int one_distance, char* sequence, int sequence_size)
{
  int* raw_code = malloc(sizeof(int) * sequence_size * 2);
  for (int i=0; i<sequence_size*2; i++)
  {
    (raw_code+i)* = on_length;
    if (sequence[i] == '0')
    {
      (raw_code+ ++i)* = -zero_distance; //note the mutation
    }
    else if (sequence[i] == '1')
    {
      (raw_code+ ++i)* = one_distance;
    }
    else
    {
      // TODO error
    }
  }
  new_size* = sequence_size * 2;
  return raw_code;
}

int* generate_pulse_length(int* new_size, int off_length, int zero_burst, int one_burst, char* sequence, int sequence_size)
{
  int* raw_code = malloc(sizeof(int) * sequence_size * 2);
  for (int i=0; i<sequence_size*2; i++)
  {
    if (sequence[i] == '0')
    {
      (raw_code+i)* = zero_burst;
    }
    else if (sequence[i] == '1')
    {
      (raw_code+i)* = one_burst;
    }
    else
    {
      // TODO error
    }
    (raw_code + ++i)* = off_length;
  }
  new_size* = sequence_size * 2;
  return raw_code;
}

#endif
