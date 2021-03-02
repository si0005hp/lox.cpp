#pragma once

#include "Expr.h"
#include "Interpreter.h"
#include "Stmt.h"
#include <deque>
#include <unordered_map>

namespace lox
{

using std::deque;
using std::string;
using std::unordered_map;

enum FunctionType
{
    FUNCTION_NONE,
    FUNCTION_FUNCTION,
    FUNCTION_INITIALIZER,
    FUNCTION_METHOD,
};

enum ClassType
{
    CLASS_NONE,
    CLASS_CLASS,
    CLASS_SUBCLASS,
};

class Resolver : public Expr::Visitor<void>, public Stmt::Visitor<void>
{
  public:
    Resolver(Interpreter &interpreter) : mInterpreter(interpreter)
    {
    }

    void Resolve(const vector<shared_ptr<Stmt>> &statements);

    void Visit(const Expression &stmt);
    void Visit(const Print &stmt);
    void Visit(const Var &stmt);
    void Visit(const Block &stmt);
    void Visit(const If &stmt);
    void Visit(const While &stmt);
    void Visit(const Function &stmt);
    void Visit(const Return &stmt);
    void Visit(const Class &stmt);

    void Visit(const Assign &expr);
    void Visit(const Binary &expr);
    void Visit(const Call &expr);
    void Visit(const Get &expr);
    void Visit(const Grouping &expr);
    void Visit(const Literal &expr);
    void Visit(const Logical &expr);
    void Visit(const Set &expr);
    void Visit(const Super &expr);
    void Visit(const This &expr);
    void Visit(const Unary &expr);
    void Visit(const Variable &expr);

  private:
    void Resolve(const Stmt &stmt);
    void Resolve(const Expr &expr);
    void BeginScope();
    void EndScope();
    void Declare(const Token &name);
    void Define(const Token &name);
    void ResolveLocal(const Expr &expr, const Token &name);
    void ResolveFunction(const Function &func, FunctionType type);

    Interpreter &mInterpreter;
    deque<unordered_map<string, bool>> mScopes;

    FunctionType mCurrentFunction = FUNCTION_NONE;
    ClassType mCurrentClass = CLASS_NONE;
};

} // namespace lox
