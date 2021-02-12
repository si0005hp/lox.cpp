#pragma once

#include "Object.h"
#include "Token.h"
#include <memory>
#include <vector>

#define V_ACCEPT_METHODS virtual string Accept(Visitor<string> &visitor) const = 0;

#define ACCEPT_METHODS                                                                                                 \
    string Accept(Visitor<string> &visitor) const override                                                             \
    {                                                                                                                  \
        return visitor.Visit(*this);                                                                                   \
    }

namespace Cclox
{

using std::shared_ptr;
using std::vector;

class Assign;
class Binary;
class Call;
class Get;
class Grouping;
class Literal;
class Logical;
class Set;
class Super;
class This;
class Unary;
class Variable;

class Expr
{
  public:
    template <class R> class Visitor
    {
      public:
        virtual R Visit(const Assign &expr) = 0;
        virtual R Visit(const Binary &expr) = 0;
        virtual R Visit(const Call &expr) = 0;
        virtual R Visit(const Get &expr) = 0;
        virtual R Visit(const Grouping &expr) = 0;
        virtual R Visit(const Literal &expr) = 0;
        virtual R Visit(const Logical &expr) = 0;
        virtual R Visit(const Set &expr) = 0;
        virtual R Visit(const Super &expr) = 0;
        virtual R Visit(const This &expr) = 0;
        virtual R Visit(const Unary &expr) = 0;
        virtual R Visit(const Variable &expr) = 0;
    };

    V_ACCEPT_METHODS
};

class Assign : public Expr
{
  public:
    Assign(shared_ptr<Token> name, shared_ptr<Expr> value) : mName(name), mValue(value)
    {
    }

    shared_ptr<Token> mName;
    shared_ptr<Expr> mValue;

    ACCEPT_METHODS
};

class Binary : public Expr
{
  public:
    Binary(shared_ptr<Expr> left, shared_ptr<Token> op, shared_ptr<Expr> right) : mLeft(left), mOp(op), mRight(right)
    {
    }

    shared_ptr<Expr> mLeft;
    shared_ptr<Token> mOp;
    shared_ptr<Expr> mRight;

    ACCEPT_METHODS
};

class Call : public Expr
{
  public:
    Call(shared_ptr<Expr> callee, shared_ptr<Token> paren, vector<shared_ptr<Expr>> arguments)
        : mCallee(callee), mParen(paren), mArguments(arguments)
    {
    }

    shared_ptr<Expr> mCallee;
    shared_ptr<Token> mParen;
    vector<shared_ptr<Expr>> mArguments;

    ACCEPT_METHODS
};

class Get : public Expr
{
  public:
    Get(shared_ptr<Expr> object, shared_ptr<Token> name) : mObject(object), mName(name)
    {
    }

    shared_ptr<Expr> mObject;
    shared_ptr<Token> mName;

    ACCEPT_METHODS
};

class Grouping : public Expr
{
  public:
    Grouping(shared_ptr<Expr> expression) : mExpression(expression)
    {
    }

    shared_ptr<Expr> mExpression;

    ACCEPT_METHODS
};

class Literal : public Expr
{
  public:
    Literal(shared_ptr<Object> value) : mValue(value)
    {
    }

    shared_ptr<Object> mValue;

    ACCEPT_METHODS
};

class Logical : public Expr
{
  public:
    Logical(shared_ptr<Expr> left, shared_ptr<Token> op, shared_ptr<Expr> right) : mLeft(left), mOp(op), mRight(right)
    {
    }

    shared_ptr<Expr> mLeft;
    shared_ptr<Token> mOp;
    shared_ptr<Expr> mRight;

    ACCEPT_METHODS
};

class Set : public Expr
{
  public:
    Set(shared_ptr<Expr> object, shared_ptr<Token> name, shared_ptr<Expr> value)
        : mObject(object), mName(name), mValue(value)
    {
    }

    shared_ptr<Expr> mObject;
    shared_ptr<Token> mName;
    shared_ptr<Expr> mValue;

    ACCEPT_METHODS
};

class Super : public Expr
{
  public:
    Super(shared_ptr<Token> keyword, shared_ptr<Token> method) : mKeyword(keyword), mMethod(method)
    {
    }

    shared_ptr<Token> mKeyword;
    shared_ptr<Token> mMethod;

    ACCEPT_METHODS
};

class This : public Expr
{
  public:
    This(shared_ptr<Token> keyword) : mKeyword(keyword)
    {
    }

    shared_ptr<Token> mKeyword;

    ACCEPT_METHODS
};

class Unary : public Expr
{
  public:
    Unary(shared_ptr<Token> op, shared_ptr<Expr> right) : mOp(op), mRight(right)
    {
    }

    shared_ptr<Token> mOp;
    shared_ptr<Expr> mRight;

    ACCEPT_METHODS
};

class Variable : public Expr
{
  public:
    Variable(shared_ptr<Token> name) : mName(name)
    {
    }

    shared_ptr<Token> mName;

    ACCEPT_METHODS
};

}; // namespace Cclox
