#pragma once

#include "Environment.h"
#include "LoxCallable.h"
#include "LoxFunction.h"
#include <unordered_map>

namespace lox
{

class LoxInstance;

using std::enable_shared_from_this;
using std::string;
using std::unordered_map;

class LoxClass : public LoxCallable
{
    friend class LoxInstance;

  public:
    LoxClass(const string &name, LoxClass *superclass, unordered_map<string, shared_ptr<LoxFunction>> &&methods)
        : mName(name), mSuperclass(superclass), mMethods(std::move(methods))
    {
    }

    size_t Arity() const;
    shared_ptr<Value> Call(Interpreter &interpreter, const vector<shared_ptr<Value>> &arguments);

    shared_ptr<LoxFunction> FindMethod(const string &name) const;

    /* Value */
    LoxClass &AsClass() override
    {
        return *this;
    }
    virtual const bool IsClass() const override
    {
        return true;
    }

    virtual const bool Equals(const Value &other) const override
    {
        throw std::runtime_error("[ERROR on LoxClass#Equals] not implemented."); // TODO
    }

    virtual const string Str() const override
    {
        return mName;
    }

  private:
    string mName;
    LoxClass *mSuperclass;
    unordered_map<string, shared_ptr<LoxFunction>> mMethods;
};

class LoxInstance : public Value, public enable_shared_from_this<LoxInstance>
{
  public:
    LoxInstance(const LoxClass &klass) : mKlass(klass)
    {
    }

    shared_ptr<Value> Get(const Token &name);
    void Set(const Token &name, const shared_ptr<Value> &value);

    /* Value */
    LoxInstance &AsInstance() override
    {
        return *this;
    }

    virtual const bool IsInstance() const override
    {
        return true;
    }

    virtual const bool Equals(const Value &other) const override
    {
        throw std::runtime_error("[ERROR on LoxInstance#Equals] not implemented."); // TODO
    }

    virtual const string Str() const override
    {
        return mKlass.mName + " instance";
    }

  private:
    const LoxClass &mKlass;
    unordered_map<string, shared_ptr<Value>> mFields;
};

} // namespace lox
