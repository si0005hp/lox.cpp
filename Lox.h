#pragma once

#include <iostream>

#include "Interpreter.h"
#include "Token.h"

namespace Cclox
{

class Lox
{
  public:
    static void Error(const int &line, const string &message);
    static void Error(const Token &token, const string &message);

    static void ErrorRuntimeError(const RuntimeError &error);

  private:
    static void Report(const int &line, const string &where, const string &message);

    static bool sHadError;
};

} // namespace Cclox
