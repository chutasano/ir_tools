#ifndef IR_SENDER_H
#define IR_SENDER_H

#include <string>
#include <vector>
extern "C" {
#include <pigpio.h>
}
#define MAX_COMMAND_SIZE 512
#define MAX_PULSES 12000


// contains int vector of header and footer and string (body)
// this is because certain protocols contain headers and footers that do not
// follow its encoding format
typedef struct SendData
{
  std::vector<int> header;
  std::string code;
  std::vector<int> footer;
} SendData;

class IrSender
{
public:
  // pin: GPIO pin number
  // frequency:
  // duty_cycle:
  IrSender(int pin, int frequency, double duty_cycle) : m_outpin(pin), m_frequency(frequency), m_duty_cycle(duty_cycle) { }

//time_slot is the time in microseconds of a bit
int send_bi_phase(int time_slot, SendData data);
int send_pulse_distance(int on_length, int zero_distance, int one_distance, SendData data);
int send_pulse_length(int off_length, int zero_burst, int one_burst, SendData data);

//note default: address_size = 5, data_size = 6
int send_rc5(std::string address, std::string data);

//address + data size are both 8 bits
int send_nec(std::string address, std::string data);

private:
  int m_outpin;
  int m_frequency;
  double m_duty_cycle;
  int ir_send_raw(std::vector<int> pulses);
  void addPulse(uint32_t onPins, uint32_t offPins, uint32_t duration, gpioPulse_t *irSignal, int *pulseCount);
  // Generates a square wave for duration (microseconds) at frequency (Hz)
  // on GPIO pin outPin. dutyCycle is a floating value between 0 and 1.
  void carrierFrequency(double duration, gpioPulse_t *irSignal, int *pulseCount);
  // Generates a low signal gap for duration, in microseconds 
  void gap(double duration, gpioPulse_t *irSignal, int *pulseCount);
  char flip(char a);
};

#endif //IR_SENDER_H
