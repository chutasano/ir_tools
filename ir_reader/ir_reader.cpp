// based off http://hertaville.com/introduction-to-accessing-the-raspberry-pis-gpio-in-c.html

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <unistd.h>
#include "ir_reader.h"

extern "C"
{
#include "pigpio.h"
}

const int LED_ON = 0; // for my system, LOW -> receiver detected infrared
const int LED_OFF = 1;
const int TIMEOUT_MS = 2000; // 2 seconds of no change seems reasonable

using namespace std;

IrReader::IrReader(int num)
{
    if (num < 0 || num > 31)
    {
        cout << "Bad GPIO val\n";
        exit(1);
    }
    this->gpionum = num;
}

IrReader::~IrReader()
{
}

vector<int> tmp;
bool init;
bool timed_out;
condition_variable cv;
mutex mtx;

void cb_get_code(int gpio, int level, uint32_t tick)
{
    cout << "Tick, " << level << endl;
    static uint32_t last_tick;
    static const uint32_t tick_wrap = 0xFFFFFFFF;
    if (!init && level != PI_TIMEOUT)
    {
        init = true;
        last_tick = tick;
        gpioSetWatchdog(gpio, TIMEOUT_MS);
        return;
    }
    else if (level == PI_TIMEOUT)
    {
        timed_out = true;
        cv.notify_one();
    }
    else
    {
        int64_t time_diff_us = tick - last_tick;
        if (time_diff_us < 0) time_diff_us += tick_wrap;
        if (level == LED_ON)
        {
            tmp.push_back(-time_diff_us);
            last_tick = tick;
        }
        else if (level == LED_OFF)
        {
            tmp.push_back(time_diff_us);
            last_tick = tick;
        }
    }
}

vector<int> IrReader::get_code()
{
    if (gpioInitialise() < 0)
    {
        cerr << "Can't initialize pigpiod\n";
        exit(1);
    }
    gpioSetMode(gpionum, PI_INPUT);
    unique_lock<mutex> lk(mtx);
    tmp = vector<int>();
    init = false;
    timed_out = false;
    gpioSetAlertFunc(gpionum, cb_get_code);
    cv.wait(lk, [] {return timed_out;
            });
    gpioTerminate();
    return tmp;
}

