#pragma once

#include <fstream>
#include <vector>
#include <string>

class IrReader
{
    public:
        IrReader(int num);
        ~IrReader();
        std::vector<int> get_code();

    private:
        int gpionum;
};

