#pragma once

#include "Expr.h"
#include <functional>
#include <string>

namespace Cclox
{

using std::reference_wrapper;
using std::string;
using std::stringstream;
using std::vector;

class AstPrinter : public Expr::Visitor<string>
{
  public:
    string Print(const Expr &expr)
    {
        return expr.Accept(*this);
    }

    virtual string Visit(const Assign &expr) override
    {
        return "";
    }

    virtual string Visit(const Binary &expr) override
    {
        return "";
    }

    virtual string Visit(const Call &expr) override
    {
        return "";
    }

    virtual string Visit(const Get &expr) override
    {
        return "";
    }

    virtual string Visit(const Grouping &expr) override
    {
        return "";
    }

    virtual string Visit(const Literal &expr) override
    {
        return expr.mValue.Str();
    }

    virtual string Visit(const Logical &expr) override
    {
        return "";
    }

    virtual string Visit(const Set &expr) override
    {
        return "";
    }

    virtual string Visit(const Super &expr) override
    {
        return "";
    }

    virtual string Visit(const This &expr) override
    {
        return "";
    }

    virtual string Visit(const Unary &expr) override
    {
        return "";
    }

    virtual string Visit(const Variable &expr) override
    {
        return "";
    }

    //   private:
    string Parenthesize(const string &name, vector<reference_wrapper<const Expr>> exprs)
    {
        stringstream ss;

        ss << "(" << name;
        for (const Expr &expr : exprs)
        {
            ss << " ";
            ss << expr.Accept(*this);
        }
        ss << ")";

        return ss.str();
    }
};

}; // namespace Cclox
