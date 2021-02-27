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

    /* for test */
    static bool HadError()
    {
        return sHadError;
    }
    static void ResetError()
    {
        sHadError = false;
    }

  private:
    static void Report(const int &line, const string &where, const string &message);
    static void DoInterpret(Interpreter &interpreter, const string &source);

    inline static bool sHadError = false;
};

} // namespace cclox
