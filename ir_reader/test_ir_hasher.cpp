#include <iostream>

#include <pigpio.h>
#include <time.h>
#include "ir_hasher.hpp"

/*

REQUIRES

An IR receiver output pin connected to a Pi gpio.

TO BUILD

g++ -o ir_hash_cpp test_ir_hasher.cpp ir_hasher.cpp -lpigpio -lrt -lpthread

TO RUN

sudo ./ir_hash_cpp

*/

#define IR_IN (18)

void callback(uint32_t hash)
{
   std::cout << "hash=" << hash << std::endl;
}

int main(int argc, char *argv[])
{
   if (gpioInitialise() >= 0)
   {
      /* Can't instantiate a Hasher before pigpio is initialised. */

      /* 
         This assumes the output pin of an IR receiver is
         connected to gpio 7.
      */
  struct timespec ts;
  ts.tv_sec = 0;
ts.tv_nsec = 300000;
      Hasher ir(IR_IN, callback);

//      nanosleep(&ts, NULL);
sleep(300);
std::cout << "Done\n";
   }
else
{
std::cout << "PiGPIO initialized\n";
}
}


