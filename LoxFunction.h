#pragma once

#include "Environment.h"
#include "LoxCallable.h"

namespace cclox
{

struct FunctionReturn : public exception
{
    FunctionReturn(const shared_ptr<Value> value) : mValue(value)
    {
    }
    const shared_ptr<Value> mValue;
};

class LoxFunction : public LoxCallable, public Value
{
  public:
    LoxFunction(const Function &declaration, const shared_ptr<Environment> &closure)
        : mDeclaration(declaration), mClosure(closure)
    {
    }

    size_t Arity() const;
    shared_ptr<Value> Call(Interpreter &interpreter, const vector<shared_ptr<Value>> &arguments);

    const LoxFunction &AsFunction() const override
    {
        return *this;
    }

    virtual const bool IsFunction() const override
    {
        return true;
    }

    virtual const bool Equals(const Value &other) const override
    {
        throw std::runtime_error("[ERROR on FunctionValue#Equals] not implemented."); // TODO
    }

    virtual const string Str() const override
    {
        return "<fn " + mDeclaration.mName->Lexeme() + ">";
    }

  private:
    Function mDeclaration;
    shared_ptr<Environment> mClosure;
};

} // namespace cclox
