#pragma once

#include "Object.h"
#include "Token.h"
#include "Value.h"
#include <memory>
#include <vector>

#define V_EXPR_ACCEPT_METHODS                                                                                          \
    virtual string Accept(Visitor<string> &visitor) const = 0;                                                         \
    virtual shared_ptr<Value> Accept(Visitor<shared_ptr<Value>> &visitor) const = 0;                                   \
    virtual void Accept(Visitor<void> &visitor) const = 0;

#define EXPR_ACCEPT_METHODS                                                                                            \
    string Accept(Visitor<string> &visitor) const override                                                             \
    {                                                                                                                  \
        return visitor.Visit(*this);                                                                                   \
    }                                                                                                                  \
    shared_ptr<Value> Accept(Visitor<shared_ptr<Value>> &visitor) const override                                       \
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

    V_EXPR_ACCEPT_METHODS
};

class Assign : public Expr
{
  public:
    Assign(const shared_ptr<Token> &name, const shared_ptr<Expr> &value) : mName(name), mValue(value)
    {
    }

    shared_ptr<Token> mName;
    shared_ptr<Expr> mValue;

    EXPR_ACCEPT_METHODS
};

class Binary : public Expr
{
  public:
    Binary(const shared_ptr<Expr> &left, const shared_ptr<Token> &op, const shared_ptr<Expr> &right)
        : mLeft(left), mOp(op), mRight(right)
    {
    }

    shared_ptr<Expr> mLeft;
    shared_ptr<Token> mOp;
    shared_ptr<Expr> mRight;

    EXPR_ACCEPT_METHODS
};

class Call : public Expr
{
  public:
    Call(const shared_ptr<Expr> &callee, const shared_ptr<Token> &paren, const vector<shared_ptr<Expr>> &arguments)
        : mCallee(callee), mParen(paren), mArguments(arguments)
    {
    }

    shared_ptr<Expr> mCallee;
    shared_ptr<Token> mParen;
    vector<shared_ptr<Expr>> mArguments;

    EXPR_ACCEPT_METHODS
};

class Get : public Expr
{
  public:
    Get(const shared_ptr<Expr> &object, const shared_ptr<Token> &name) : mObject(object), mName(name)
    {
    }

    shared_ptr<Expr> mObject;
    shared_ptr<Token> mName;

    EXPR_ACCEPT_METHODS
};

class Grouping : public Expr
{
  public:
    Grouping(const shared_ptr<Expr> &expression) : mExpression(expression)
    {
    }

    shared_ptr<Expr> mExpression;

    EXPR_ACCEPT_METHODS
};

class Literal : public Expr
{
  public:
    Literal(const shared_ptr<Object> &value) : mValue(value)
    {
    }

    shared_ptr<Object> mValue;

    EXPR_ACCEPT_METHODS
};

class Logical : public Expr
{
  public:
    Logical(const shared_ptr<Expr> &left, const shared_ptr<Token> &op, const shared_ptr<Expr> &right)
        : mLeft(left), mOp(op), mRight(right)
    {
    }

    shared_ptr<Expr> mLeft;
    shared_ptr<Token> mOp;
    shared_ptr<Expr> mRight;

    EXPR_ACCEPT_METHODS
};

class Set : public Expr
{
  public:
    Set(const shared_ptr<Expr> &object, const shared_ptr<Token> &name, const shared_ptr<Expr> &value)
        : mObject(object), mName(name), mValue(value)
    {
    }

    shared_ptr<Expr> mObject;
    shared_ptr<Token> mName;
    shared_ptr<Expr> mValue;

    EXPR_ACCEPT_METHODS
};

class Super : public Expr
{
  public:
    Super(const shared_ptr<Token> &keyword, const shared_ptr<Token> &method) : mKeyword(keyword), mMethod(method)
    {
    }

    shared_ptr<Token> mKeyword;
    shared_ptr<Token> mMethod;

    EXPR_ACCEPT_METHODS
};

class This : public Expr
{
  public:
    This(const shared_ptr<Token> &keyword) : mKeyword(keyword)
    {
    }

    shared_ptr<Token> mKeyword;

    EXPR_ACCEPT_METHODS
};

class Unary : public Expr
{
  public:
    Unary(const shared_ptr<Token> &op, const shared_ptr<Expr> &right) : mOp(op), mRight(right)
    {
    }

    shared_ptr<Token> mOp;
    shared_ptr<Expr> mRight;

    EXPR_ACCEPT_METHODS
};

class Variable : public Expr
{
  public:
    Variable(const shared_ptr<Token> &name) : mName(name)
    {
    }

    shared_ptr<Token> mName;

    EXPR_ACCEPT_METHODS
};

}; // namespace cclox
