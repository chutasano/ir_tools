// based off http://hertaville.com/introduction-to-accessing-the-raspberry-pis-gpio-in-c.html

#pragma once

#include <string>
#include <vector>

class IrReader
{
    public:
        IrReader(int num);
        ~IrReader();
        std::vector<std::string> get_code();

    private:
        int get_val();
        std::string gpionum;
};

