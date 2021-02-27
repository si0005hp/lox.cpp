#include "Environment.h"
#include "Interpreter.h"

#define ANCESTOR_IMPL                                                                                                  \
    auto environment = shared_from_this();                                                                             \
    for (int i = 0; i < distance; i++)                                                                                 \
        environment = environment->mEnclosing;                                                                         \
    return environment;

namespace cclox
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

    if (mEnclosing)
    {
        mEnclosing->Assign(name, value);
        return;
    }

    throw RuntimeError(name, "Undefined variable a'" + name->Lexeme() + "'.");
}

void Environment::AssignAt(int distance, const shared_ptr<Token> &name, const shared_ptr<Value> &value)
{
    Ancestor(distance)->mValues[name->Lexeme()] = value;
}

shared_ptr<Value> Environment::Get(const shared_ptr<Token> &name) const
{
    if (mValues.contains(name->Lexeme()))
        return mValues.at(name->Lexeme());

    if (mEnclosing)
        return mEnclosing->Get(name);

    throw RuntimeError(name, "Undefined variable aaa'" + name->Lexeme() + "'.");
}

shared_ptr<Value> Environment::GetAt(int distance, const string &name) const
{
    return Ancestor(distance)->mValues.at(name);
}

shared_ptr<Environment> Environment::Ancestor(int distance){ANCESTOR_IMPL}

shared_ptr<const Environment> Environment::Ancestor(int distance) const
{
    ANCESTOR_IMPL
}

} // namespace cclox
