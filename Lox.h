#pragma once

#include <iostream>

#include "Interpreter.h"
#include "Token.h"

namespace cclox
{

class Lox
{
  public:
    static void RunFile(const string &fileName);
    static void RunFile(const string &fileName, std::ostream &os);
    static void RunRepl();

    static void Error(const int &line, const string &message);
    static void Error(const Token &token, const string &message);

    static void ErrorRuntimeError(const RuntimeError &error);

  private:
    static void Report(const int &line, const string &where, const string &message);

    static bool sHadError;
};

} // namespace cclox
