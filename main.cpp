#include <any>
#include <iostream>
#include <sstream>

#include "Lox.h"

using namespace lox;

int main(int argc, char const *argv[])
{
    if (argc == 1)
    {
        std::cout << "lox 0.0.0d" << std::endl;
        std::cout << "------------" << std::endl;
        Lox::RunRepl();
    }
    else if (argc >= 2)
    {
        Lox::RunFile(string(argv[1]));
    }

    return 0;
}
