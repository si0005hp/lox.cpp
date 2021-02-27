#pragma once

#include "Token.h"
#include "Value.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace cclox
{

using std::enable_shared_from_this;
using std::shared_ptr;
using std::string;
using std::unordered_map;

class Environment : public enable_shared_from_this<Environment>
{
  public:
    Environment() : Environment(nullptr)
    {
    }
    Environment(const shared_ptr<Environment> &enclosing) : mEnclosing(enclosing)
    {
    }

    void Define(const string &name, const shared_ptr<Value> &value);
    void Assign(const shared_ptr<Token> &name, const shared_ptr<Value> &value);
    void AssignAt(int distance, const shared_ptr<Token> &name, const shared_ptr<Value> &value);
    shared_ptr<Value> Get(const shared_ptr<Token> &name) const;
    shared_ptr<Value> GetAt(int distance, const string &name) const;
    shared_ptr<Environment> Ancestor(int distance);
    shared_ptr<const Environment> Ancestor(int distance) const;

  private:
    shared_ptr<Environment> mEnclosing;

    unordered_map<string, shared_ptr<Value>> mValues;
};

} // namespace cclox
