#include <any>
#include <iostream>
#include <sstream>

#include "AstPrinter.h"
#include "Interpreter.h"
#include "Parser.h"
#include "Scanner.h"

using namespace Cclox;

static Interpreter interpreter;

void PrintAst(const string &source)
{
    Scanner s(source);
    Parser p(s.ScanTokens());
    std::cout << AstPrinter().Ast(*p.ParseExpr()) << std::endl;
}

void InterpretExpr(const string &source)
{
    Scanner s(source);
    Parser p(s.ScanTokens());

    auto value = interpreter.InterpretExpr(*p.ParseExpr());
    auto str = value ? value->Str() : "nil";
    std::cout << str << std::endl;
}

void Interpret(const string &source)
{
    Scanner s(source);
    Parser p(s.ScanTokens());

    interpreter.Interpret(p.Parse());
}

void RunRepl(const bool &isDebug)
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

        if (isDebug)
            PrintAst(line);
        else
            // InterpretExpr(line);
            Interpret(line);
    }
}

bool IsAstDebug(int argc, char const *argv[])
{
    return argc > 1 && string(argv[1]) == "s";
}

int main(int argc, char const *argv[])
{
    std::cout << "cclox 0.0.0d" << std::endl;
    std::cout << "------------" << std::endl;

    RunRepl(IsAstDebug(argc, argv));

    return 0;
}
