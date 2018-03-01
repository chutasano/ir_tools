#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <list>
#include "ir_reader.h"

#define GPIO_PIN 23

IrReader r(GPIO_PIN);

using namespace std;

int main(int argc, char* argv[])
{
    if (argc > 2 || argc < 1)
    {
        cout << "Usage: " << argv[0] << " [filename: default = ircodes.txt]\n]";
        exit(1);
    }
    cout << "Press a button to record on the remote\n";
    list<string> codes = r.get_code();
    string filename;
    if (argc == 2)
    {
        string filename = argv[1];
    }
    else
    {
        filename = "ircodes.txt";
    }
    string name;
    cout << "Done recording.\nEnter name to save: ";
    cin >> name;
    ofstream w(filename, ofstream::out | ofstream::app);
    w << "name: " << name << "\n";
    for (string s : codes)
    {
        w << s << " ";
    }
    w << "\n";
    w.close();
    cout << "Saved to " << filename << " with name: " << name << "\n";
}
