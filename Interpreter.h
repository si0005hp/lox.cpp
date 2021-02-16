#pragma once

#include "Expr.h"
#include "Stmt.h"
#include "Value.h"
#include <memory>

namespace Cclox
{

using std::exception;
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
    void Interpret(const vector<shared_ptr<Stmt>> &stmts);
    // TODO: delete as it's temporal
    shared_ptr<Value> InterpretExpr(const Expr &expr);

    void Execute(const Stmt &stmt);
    void Visit(const Expression &stmt);
    void Visit(const Print &stmt);
    void Visit(const Var &stmt);

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

  private:
    shared_ptr<Value> Evaluate(const Expr &expr);
    bool IsTruthy(const shared_ptr<Value> &value);
    bool IsEqual(const shared_ptr<Value> &left, const shared_ptr<Value> &right);
    void CheckNumberOperand(const shared_ptr<Token> op, const shared_ptr<Value> &operand);
    void CheckNumberOperands(const shared_ptr<Token> op, const shared_ptr<Value> &left, const shared_ptr<Value> &right);

    shared_ptr<Value> InterpretObject(const shared_ptr<Object> &object);
    shared_ptr<Value> InterpretObject(const Object &object);
    void Println(const string &str);
};

}; // namespace Cclox
