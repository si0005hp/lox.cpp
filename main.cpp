#include <any>
#include <iostream>
#include <sstream>

#include "AstPrinter.h"
#include "Parser.h"
#include "Scanner.h"

using namespace Cclox;

void PrintAst(const string &source)
{
    Scanner s(source);
    Parser p(s.ScanTokens());
    std::cout << AstPrinter().Print(*p.Parse()) << std::endl;
}

void RunRepl()
{
    bool isFirstLine = false;

    while (true)
    {
        if (isFirstLine)
        {
            std::cout << ">> ";
            isFirstLine = false;
        }
        else
        {
            std::cout << ".. ";
        }

        string line;
        while (line.size() == 0)
        {
            std::getline(std::cin, line);
        }
        PrintAst(line);
    }
}

int main(int argc, char const *argv[])
{
    std::cout << "cclox 0.0.0d" << std::endl;
    std::cout << "------------" << std::endl;

    RunRepl();

    return 0;
}
