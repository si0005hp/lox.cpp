#include "Environment.h"
#include "Interpreter.h"

namespace Cclox
{

void Environment::Define(const string &name, const shared_ptr<Value> &value)
{
    mValues[name] = value;
}

void Environment::Assign(const shared_ptr<Token> &name, const shared_ptr<Value> &value)
{
    if (mValues.contains(name->Lexeme()))
    {
        mValues[name->Lexeme()] = value;
        return;
    }
    throw RuntimeError(name, "Undefined variable '" + name->Lexeme() + "'.");
}

shared_ptr<Value> Environment::Get(const shared_ptr<Token> &name) const
{
    if (mValues.contains(name->Lexeme()))
        return mValues.at(name->Lexeme());

    if (mEnclosing)
        return mEnclosing->Get(name);

    throw RuntimeError(name, "Undefined variable '" + name->Lexeme() + "'.");
}

} // namespace Cclox