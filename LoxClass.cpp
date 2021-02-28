#include "LoxClass.h"
#include "Interpreter.h"

namespace cclox
{

shared_ptr<Value> LoxClass::Call(Interpreter &interpreter, const vector<shared_ptr<Value>> &arguments)
{
    auto instance = make_shared<LoxInstance>(*this);
    return instance;
}

size_t LoxClass::Arity() const
{
    return 0;
}

shared_ptr<LoxFunction> LoxClass::FindMethod(const string &name) const
{
    return mMethods.contains(name) ? mMethods.at(name) : nullptr;
}

/* LoxInstance */
shared_ptr<Value> LoxInstance::Get(const Token &name)
{
    if (mFields.contains(name.Lexeme()))
        return mFields.at(name.Lexeme());

    auto method = mKlass.FindMethod(name.Lexeme());
    if (method)
        return method->Bind(shared_from_this());

    throw RuntimeError(name, "Undefined property '" + name.Lexeme() + "'.");
}

void LoxInstance::Set(const Token &name, const shared_ptr<Value> &value)
{
    mFields[name.Lexeme()] = value;
}

} // namespace cclox
