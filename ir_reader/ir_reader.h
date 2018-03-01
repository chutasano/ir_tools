// based off http://hertaville.com/introduction-to-accessing-the-raspberry-pis-gpio-in-c.html

#pragma once

#include <fstream>
#include <list>
#include <string>

class IrReader
{
    public:
        IrReader(int num);
        ~IrReader();
        std::list<std::string> get_code();

    private:
        int get_val();
        std::string gpionum;
        std::ifstream getvalgpio;
};

