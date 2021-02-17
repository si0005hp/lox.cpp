
#pragma once

#include "Expr.h"
#include "Value.h"
#include <memory>
#include <vector>

#define V_STMT_ACCEPT_METHODS                                                                                          \
    virtual string Accept(Visitor<string> &visitor) const = 0;                                                         \
    virtual void Accept(Visitor<void> &visitor) const = 0;

#define STMT_ACCEPT_METHODS                                                                                            \
    string Accept(Visitor<string> &visitor) const override                                                             \
    {                                                                                                                  \
        return visitor.Visit(*this);                                                                                   \
    }                                                                                                                  \
    void Accept(Visitor<void> &visitor) const override                                                                 \
    {                                                                                                                  \
        return visitor.Visit(*this);                                                                                   \
    }

namespace Cclox
{

using std::shared_ptr;
using std::vector;

class Block;
class Expression;
class Print;
class Var;

class Stmt
{
  public:
    template <class R> class Visitor
    {
      public:
        virtual R Visit(const Block &stmt) = 0;
        virtual R Visit(const Expression &stmt) = 0;
        virtual R Visit(const Print &stmt) = 0;
        virtual R Visit(const Var &stmt) = 0;
    };

    V_STMT_ACCEPT_METHODS
};

class Block : public Stmt
{
  public:
    Block(vector<shared_ptr<Stmt>> statements) : mStatements(statements)
    {
    }

    vector<shared_ptr<Stmt>> mStatements;

    STMT_ACCEPT_METHODS
};

class Expression : public Stmt
{
  public:
    Expression(shared_ptr<Expr> expression) : mExpression(expression)
    {
    }

    shared_ptr<Expr> mExpression;

    STMT_ACCEPT_METHODS
};

class Print : public Stmt
{
  public:
    Print(shared_ptr<Expr> expression) : mExpression(expression)
    {
    }

    shared_ptr<Expr> mExpression;

    STMT_ACCEPT_METHODS
};

class Var : public Stmt
{
  public:
    Var(shared_ptr<Token> name, shared_ptr<Expr> initializer) : mName(name), mInitializer(initializer)
    {
    }

    shared_ptr<Token> mName;
    shared_ptr<Expr> mInitializer;

    STMT_ACCEPT_METHODS
};

}; // namespace Cclox
