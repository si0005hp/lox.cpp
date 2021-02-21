#pragma once

#include "Stmt.h"
#include "Value.h"

namespace cclox
{

class Interpreter;

class LoxCallable
{
  public:
    virtual size_t Arity() const = 0;
    virtual shared_ptr<Value> Call(Interpreter &interpreter, const vector<shared_ptr<Value>> &arguments) = 0;
};

} // namespace cclox
