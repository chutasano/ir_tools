#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "ir_sender.h"

int main(int argc, char *argv[])
{
    if (argc < 2 || argc > 3)
    {
        std::cout << "Usage: " << argv[0] << " <name> [filename: default = ircodes.txt]\n";
        exit(1);
    }
    std::string filename;
    if (argc == 3)
    {
        filename = argv[2];
    }
    else
    {
        filename = "ircodes.txt";
    }
    std::string codename = argv[1];
    std::ifstream codes_list(filename);
    if (!codes_list)
    {
        std::cout << "Error cannot open: " << filename << "\n";
        exit(1);
    }
    std::string line;
    while (getline(codes_list, line))
    {
        if (line == "name: " + codename)
        {
            break; //found entry, line below should contain codes
        }
    }
    if (!getline(codes_list, line))
    {
        std::cout << "Error bad file format\n";
        exit(1);
    }
    std::istringstream is(line);
    int n;
    std::vector<int> codes;
    while (is >> n)
    {
        codes.push_back(n);
    }
    std::cout << "Sending command " << name << " from " << filename << "\n";
    uint32_t outPin = 22;            // The Broadcom pin number the signal will be sent on
    int frequency = 38000;           // The frequency of the IR signal in Hz
    double dutyCycle = 0.5;          // The duty cycle of the IR signal. 0.5 means for every cycle,
    // the LED will turn on for half the cycle time, and off the other half
    IrSender light(outPin, frequency, dutyCycle);
    int result = light.ir_send_raw(codes);
    std::cout << "Sent\n";
    return result;
}
