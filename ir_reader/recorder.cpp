#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "ir_reader.h"

#define GPIO_PIN 23

IrReader r(GPIO_PIN);

int main(int argc, char* argv[])
{
    if (argc > 2 || argc < 1)
    {
        std::cout << "Usage: " << argv[0] << " [filename: default = ircodes.txt]\n]";
        exit(1);
    }
    std::cout << "Press a button to record on the remote\n";
    std::vector<std::string> codes = r.get_code();
    std::string filename;
    if (argc == 2)
    {
        std::string filename = argv[1];
    }
    else
    {
        filename = "ircodes.txt";
    }
    std::string name;
    std::cout << "Done recording.\nEnter name to save: ";
    std::cin >> name;
    std::ofstream w(filename, std::ofstream::out | std::ofstream::app);
    w << "name: " << name << "\n";
    for (int i = 0; i < codes.size(); i++)
    {
        w << codes[i] << " ";
    }
    w << "\n";
    w.close();
    std::cout << "Saved to " << filename << " with name: " << name << "\n";
}
