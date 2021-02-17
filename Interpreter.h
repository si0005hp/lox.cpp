#pragma once

#include "Environment.h"
#include "Expr.h"
#include "Stmt.h"
#include "Value.h"
#include <iostream>
#include <memory>

namespace Cclox
{

using std::exception;
using std::make_shared;
using std::shared_ptr;

class RuntimeError : public exception
{
  public:
    RuntimeError(const shared_ptr<Token> token, const string &msg) : mToken(token), mMsg(msg)
    {
    }
    virtual const char *what() const throw()
    {
        return mMsg.c_str();
    }

    const shared_ptr<Token> mToken;
    const string mMsg;
};

class Interpreter : public Expr::Visitor<shared_ptr<Value>>, public Stmt::Visitor<void>
{
  public:
    Interpreter() : mEnvironment(make_shared<Environment>()), mOs(std::cout)
    {
    }
    Interpreter(std::ostream &os) : mEnvironment(make_shared<Environment>()), mOs(os)
    {
    }

    void Interpret(const vector<shared_ptr<Stmt>> &stmts);

    void Visit(const Expression &stmt);
    void Visit(const Print &stmt);
    void Visit(const Var &stmt);
    void Visit(const Block &stmt);

    shared_ptr<Value> Visit(const Assign &expr);
    shared_ptr<Value> Visit(const Binary &expr);
    shared_ptr<Value> Visit(const Call &expr);
    shared_ptr<Value> Visit(const Get &expr);
    shared_ptr<Value> Visit(const Grouping &expr);
    shared_ptr<Value> Visit(const Literal &expr);
    shared_ptr<Value> Visit(const Logical &expr);
    shared_ptr<Value> Visit(const Set &expr);
    shared_ptr<Value> Visit(const Super &expr);
    shared_ptr<Value> Visit(const This &expr);
    shared_ptr<Value> Visit(const Unary &expr);
    shared_ptr<Value> Visit(const Variable &expr);

    // for test
    const Environment &GetEnvironment() const
    {
        return *mEnvironment;
    }

  private:
    void Execute(const Stmt &stmt);
    void ExecuteBlock(const vector<shared_ptr<Stmt>> &stmts, const shared_ptr<Environment> &environment);
    shared_ptr<Value> Evaluate(const Expr &expr);
    bool IsTruthy(const shared_ptr<Value> &value) const;
    bool IsEqual(const shared_ptr<Value> &left, const shared_ptr<Value> &right) const;
    void CheckNumberOperand(const shared_ptr<Token> op, const shared_ptr<Value> &operand) const;
    void CheckNumberOperands(const shared_ptr<Token> op, const shared_ptr<Value> &left,
                             const shared_ptr<Value> &right) const;

    shared_ptr<Value> InterpretObject(const shared_ptr<Object> &object) const;
    shared_ptr<Value> InterpretObject(const Object &object) const;
    void Println(const string &str) const;

    shared_ptr<Environment> mEnvironment;

    std::ostream &mOs;
};

}; // namespace Cclox
