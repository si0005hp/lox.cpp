#pragma once

#include "Expr.h"
#include "Stmt.h"
#include <any>
#include <functional>
#include <string>
#include <type_traits>

namespace Cclox
{

using std::any;
using std::any_cast;
using std::bad_any_cast;
using std::reference_wrapper;
using std::string;
using std::stringstream;
using std::vector;

class AstPrinter : public Expr::Visitor<string>, public Stmt::Visitor<string>
{
  public:
    string Ast(const Expr &expr)
    {
        return expr.Accept(*this);
    }
    string Ast(const Stmt &stmt)
    {
        return stmt.Accept(*this);
    }

    virtual string Visit(const Expression &stmt) override
    {
        return Parenthesize(";", vector<shared_ptr<Expr>>{stmt.mExpression});
    }
    virtual string Visit(const Print &stmt) override
    {
        return Parenthesize("print", vector<shared_ptr<Expr>>{stmt.mExpression});
    }
    virtual string Visit(const Var &stmt) override
    {
        if (stmt.mInitializer)
            return Parenthesize2("var", vector<any>{stmt.mName, "=", stmt.mInitializer});
        return Parenthesize2("var", vector<any>{stmt.mName});
    }
    virtual string Visit(const Block &stmt) override
    {
        stringstream ss;
        ss << "(block ";

        for (auto s : stmt.mStatements)
            ss << s->Accept(*this);

        ss << ")";
        return ss.str();
    }

    virtual string Visit(const Assign &expr) override
    {
        return Parenthesize2("=", vector<any>{expr.mName->Lexeme(), expr.mValue});
    }

    virtual string Visit(const Binary &expr) override
    {
        return Parenthesize(expr.mOp->Lexeme(), vector<shared_ptr<Expr>>{expr.mLeft, expr.mRight});
    }

    virtual string Visit(const Call &expr) override
    {
        return Parenthesize2("call", vector<any>{expr.mCallee, expr.mArguments});
    }

    virtual string Visit(const Get &expr) override
    {
        return Parenthesize2(".", vector<any>{expr.mObject, expr.mName->Lexeme()});
    }

    virtual string Visit(const Grouping &expr) override
    {
        return Parenthesize("group", vector<shared_ptr<Expr>>{expr.mExpression});
    }

    virtual string Visit(const Literal &expr) override
    {
        return expr.mValue->Str();
    }

    virtual string Visit(const Logical &expr) override
    {
        return Parenthesize(expr.mOp->Lexeme(), vector<shared_ptr<Expr>>{expr.mLeft, expr.mRight});
    }

    virtual string Visit(const Set &expr) override
    {
        return Parenthesize2("=", vector<any>{expr.mObject, expr.mName->Lexeme(), expr.mValue});
    }

    virtual string Visit(const Super &expr) override
    {
        return Parenthesize2("super", vector<any>{expr.mMethod});
    }

    virtual string Visit(const This &expr) override
    {
        return "this";
    }

    virtual string Visit(const Unary &expr) override
    {
        return Parenthesize(expr.mOp->Lexeme(), vector<shared_ptr<Expr>>{expr.mRight});
    }

    virtual string Visit(const Variable &expr) override
    {
        return expr.mName->Lexeme();
    }

  private:
    string Parenthesize(const string &name, const vector<shared_ptr<Expr>> &exprs)
    {
        stringstream ss;

        ss << "(" << name;
        for (auto expr : exprs)
        {
            ss << " ";
            ss << expr->Accept(*this);
        }
        ss << ")";

        return ss.str();
    }

    string Parenthesize2(const string &name, const vector<any> &parts)
    {
        stringstream ss;

        ss << "(" << name;
        Transform(ss, parts);
        ss << ")";

        return ss.str();
    }

    void Transform(stringstream &ss, const vector<any> &parts)
    {
        for (auto part : parts)
        {
            ss << " ";
            if (IsType<string>(part))
            {
                ss << any_cast<string>(part);
            }
            else if (IsType<shared_ptr<Expr>>(part))
            {
                ss << any_cast<shared_ptr<Expr>>(part)->Accept(*this);
            }
            else if (IsType<shared_ptr<Token>>(part))
            {
                ss << any_cast<shared_ptr<Token>>(part)->Lexeme();
            }
            else if (IsType<vector<shared_ptr<Expr>>>(part))
            {
                Transform(ss, ToAnyVector(any_cast<vector<shared_ptr<Expr>>>(part)));
            }
            else
            {
                ss << any_cast<string>(part); // treat as string
            }
        }
    }

    template <typename T> bool IsType(any a)
    {
        return a.type().name() == typeid(T).name();
    }

    template <typename T> vector<any> ToAnyVector(const vector<T> &src)
    {
        vector<any> dst;
        for (auto e : src)
            dst.push_back(e);
        return dst;
    }
};

}; // namespace Cclox
