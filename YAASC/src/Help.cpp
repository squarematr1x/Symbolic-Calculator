#include "Help.h"

#include <fstream>
#include <iostream>

namespace util {

void help()
{
    std::string path = "";

#if defined _WIN32
    path = "..\\help.txt";
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
    path = ("././help.txt");
#endif
    std::ifstream help_file(path);
    std::string line;
    if (help_file.is_open())
    {
        while (getline(help_file, line))
            std::cout << line << '\n';

        help_file.close();
        std::cout << '\n';
    }
    else 
        std::cout << "Unable to open help file\n";
}

}