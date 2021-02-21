
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

namespace cclox
{

using std::shared_ptr;
using std::vector;

class Block;
class Expression;
class Function;
class If;
class Print;
class Return;
class Var;
class While;

class Stmt
{
  public:
    template <class R> class Visitor
    {
      public:
        virtual R Visit(const Block &stmt) = 0;
        virtual R Visit(const Expression &stmt) = 0;
        virtual R Visit(const Function &stmt) = 0;
        virtual R Visit(const If &stmt) = 0;
        virtual R Visit(const Print &stmt) = 0;
        virtual R Visit(const Return &stmt) = 0;
        virtual R Visit(const Var &stmt) = 0;
        virtual R Visit(const While &stmt) = 0;
    };

    V_STMT_ACCEPT_METHODS
};

class Block : public Stmt
{
  public:
    Block(const vector<shared_ptr<Stmt>> &statements) : mStatements(statements)
    {
    }

    vector<shared_ptr<Stmt>> mStatements;

    STMT_ACCEPT_METHODS
};

class Expression : public Stmt
{
  public:
    Expression(const shared_ptr<Expr> &expression) : mExpression(expression)
    {
    }

    shared_ptr<Expr> mExpression;

    STMT_ACCEPT_METHODS
};

class Function : public Stmt
{
  public:
    Function(const shared_ptr<Token> &name, const vector<shared_ptr<Token>> &params,
             const vector<shared_ptr<Stmt>> &body)
        : mName(name), mParams(params), mBody(body)
    {
    }

    shared_ptr<Token> mName;
    vector<shared_ptr<Token>> mParams;
    vector<shared_ptr<Stmt>> mBody;

    STMT_ACCEPT_METHODS
};

class If : public Stmt
{
  public:
    If(const shared_ptr<Expr> &condition, const shared_ptr<Stmt> &thenBranch, const shared_ptr<Stmt> &elseBranch)
        : mCondition(condition), mThenBranch(thenBranch), mElseBranch(elseBranch)
    {
    }

    shared_ptr<Expr> mCondition;
    shared_ptr<Stmt> mThenBranch;
    shared_ptr<Stmt> mElseBranch;

    STMT_ACCEPT_METHODS
};

class Print : public Stmt
{
  public:
    Print(const shared_ptr<Expr> &expression) : mExpression(expression)
    {
    }

    shared_ptr<Expr> mExpression;

    STMT_ACCEPT_METHODS
};

class Return : public Stmt
{
  public:
    Return(const shared_ptr<Token> &keyword, const shared_ptr<Expr> &value) : mKeyword(keyword), mValue(value)
    {
    }

    shared_ptr<Token> mKeyword;
    shared_ptr<Expr> mValue;

    STMT_ACCEPT_METHODS
};

class Var : public Stmt
{
  public:
    Var(const shared_ptr<Token> &name, const shared_ptr<Expr> &initializer) : mName(name), mInitializer(initializer)
    {
    }

    shared_ptr<Token> mName;
    shared_ptr<Expr> mInitializer;

    STMT_ACCEPT_METHODS
};

class While : public Stmt
{
  public:
    While(const shared_ptr<Expr> &condition, const shared_ptr<Stmt> &body) : mCondition(condition), mBody(body)
    {
    }

    shared_ptr<Expr> mCondition;
    shared_ptr<Stmt> mBody;

    STMT_ACCEPT_METHODS
};

}; // namespace cclox
