#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "ir_reader.h"

#define GPIO_PIN 22

IrReader r(GPIO_PIN);

int main()
{
    while(true)
    {
        std::vector<std::string> codes = r.get_code();
        std::string filename;
        std::cout << "Enter file name to save: ";
        std::cin >> filename;
        std::ofstream w(filename);
        for (int i = 0; i < codes.size(); i++)
        {
            w << codes[i] << " ";
        }
        w << "\n";
        w.close();
    }
}
