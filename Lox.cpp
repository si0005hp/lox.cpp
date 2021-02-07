#include "Lox.h"

#include <iostream>
#include <string>

namespace Cclox
{

using std::string;

bool Lox::sHadError = false;

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

void Lox::Report(const int &line, const string &where, const string &message)
{
    std::cerr << "[line " << line << "]" << where << ":" << message << std::endl;
    sHadError = true;
}

} // namespace Cclox
