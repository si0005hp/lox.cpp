#include "Lox.h"
#include "Interpreter.h"
#include "Parser.h"
#include "Resolver.h"
#include "Scanner.h"

#include <fstream>
#include <iostream>
#include <string>

namespace lox
{

using std::ifstream;
using std::string;

string ReadFile(const string &fileName)
{
    ifstream file;
    file.open(fileName);
    if (file.fail())
    {
        // TODO error
    }

    stringstream ss;
    string line;
    while (getline(file, line))
        ss << line << std::endl;

    return ss.str();
}

void Lox::DoInterpret(Interpreter &interpreter, const string &source)
{
    Scanner s(source);
    Parser p(s.ScanTokens());

    auto stmts = p.Parse();

    if (sHadError)
        return;

    Resolver r(interpreter);
    r.Resolve(stmts);

    if (sHadError)
        return;

    interpreter.Interpret(stmts);
}

void Lox::RunFile(const string &fileName)
{
    Interpreter interpreter;

    DoInterpret(interpreter, ReadFile(fileName));
}

void Lox::RunFile(const string &fileName, std::ostream &os)
{
    Interpreter interpreter(os);

    DoInterpret(interpreter, ReadFile(fileName));
}

void Lox::RunRepl()
{
    Interpreter interpreter;

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
            std::getline(std::cin, line);

        DoInterpret(interpreter, line);
    }
}

void Lox::Error(const int &line, const string &message)
{
    Report(line, "", message);
}

void Lox::Error(const Token &token, const string &message)
{
    if (token.Type() == TOKEN_EOF)
    {
        Report(token.Line(), " at end", message);
    }
    else
    {
        Report(token.Line(), " at '" + token.Lexeme() + "'", message);
    }
}

void Lox::ErrorRuntimeError(const RuntimeError &error)
{
    std::cerr << error.mMsg << std::endl;
    std::cerr << "[line " << error.mToken.Line() << "]" << std::endl;
    sHadError = true;
}

void Lox::Report(const int &line, const string &where, const string &message)
{
    std::cerr << "[line " << line << "]" << where << ":" << message << std::endl;
    sHadError = true;
}

} // namespace lox
