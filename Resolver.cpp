#include "Resolver.h"
#include "Lox.h"
#include <iostream>

namespace cclox
{

void Resolver::Resolve(const vector<shared_ptr<Stmt>> &statements)
{
    for (auto stmt : statements)
        Resolve(*stmt);
}
void Resolver::Visit(const Expression &stmt)
{
    Resolve(*stmt.mExpression);
}

void Resolver::Visit(const Print &stmt)
{
    Resolve(*stmt.mExpression);
}

void Resolver::Visit(const Var &stmt)
{
    Declare(*stmt.mName);
    if (stmt.mInitializer)
        Resolve(*stmt.mInitializer);
    Define(*stmt.mName);
}

void Resolver::Visit(const Block &stmt)
{
    BeginScope();
    Resolve(stmt.mStatements);
    EndScope();
}

void Resolver::Visit(const If &stmt)
{
    Resolve(*stmt.mCondition);
    Resolve(*stmt.mThenBranch);
    if (stmt.mElseBranch)
        Resolve(*stmt.mElseBranch);
}

void Resolver::Visit(const While &stmt)
{
    Resolve(*stmt.mCondition);
    Resolve(*stmt.mBody);
}

void Resolver::Visit(const Function &stmt)
{
    Declare(*stmt.mName);
    Define(*stmt.mName);

    ResolveFunction(stmt, FUNCTION_FUNCTION);
}

void Resolver::Visit(const Return &stmt)
{
    if (mCurrentFunction == FUNCTION_NONE)
        Lox::Error(*stmt.mKeyword, "Can't return from top-level code.");

    if (stmt.mValue)
    {
        if (mCurrentFunction == FUNCTION_INITIALIZER)
            Lox::Error(*stmt.mKeyword, "Can't return a value from an initializer.");

        Resolve(*stmt.mValue);
    }
}

void Resolver::Visit(const Class &stmt)
{
    auto enclosingClass = mCurrentClass;
    mCurrentClass = CLASS_CLASS;

    Declare(*stmt.mName);
    Define(*stmt.mName);

    BeginScope();
    mScopes.front()["this"] = true;

    for (auto method : stmt.mMethods)
        ResolveFunction(*method, method->mName->Lexeme() == "init" ? FUNCTION_INITIALIZER : FUNCTION_METHOD);

    EndScope();

    mCurrentClass = enclosingClass;
}

void Resolver::Visit(const Assign &expr)
{
    Resolve(*expr.mValue);
    ResolveLocal(expr, *expr.mName);
}

void Resolver::Visit(const Binary &expr)
{
    Resolve(*expr.mLeft);
    Resolve(*expr.mRight);
}

void Resolver::Visit(const Call &expr)
{
    Resolve(*expr.mCallee);
    for (auto arg : expr.mArguments)
        Resolve(*arg);
}

void Resolver::Visit(const Get &expr)
{
    Resolve(*expr.mObject);
}

void Resolver::Visit(const Grouping &expr)
{
    Resolve(*expr.mExpression);
}

void Resolver::Visit(const Literal &expr)
{
    // no work to do for Literal
}

void Resolver::Visit(const Logical &expr)
{
    Resolve(*expr.mLeft);
    Resolve(*expr.mRight);
}

void Resolver::Visit(const Set &expr)
{
    Resolve(*expr.mValue);
    Resolve(*expr.mObject);
}

void Resolver::Visit(const Super &expr)
{
}

void Resolver::Visit(const This &expr)
{
    if (mCurrentClass == CLASS_NONE)
    {
        Lox::Error(*expr.mKeyword, "Can't use 'this' outside of a class.");
        return;
    }

    ResolveLocal(expr, *expr.mKeyword);
}

void Resolver::Visit(const Unary &expr)
{
    Resolve(*expr.mRight);
}

void Resolver::Visit(const Variable &expr)
{
    if (!mScopes.empty())
    {
        auto &topScope = mScopes.front();
        if (topScope.contains(expr.mName->Lexeme()) && topScope[expr.mName->Lexeme()] == false)
            Lox::Error(*expr.mName, "Can't read local variable in its own initializer.");
    }

    ResolveLocal(expr, *expr.mName);
}

void Resolver::Resolve(const Stmt &stmt)
{
    stmt.Accept(*this);
}

void Resolver::Resolve(const Expr &expr)
{
    expr.Accept(*this);
}

void Resolver::BeginScope()
{
    mScopes.push_front(unordered_map<string, bool>());
}

void Resolver::EndScope()
{
    mScopes.pop_front();
}

void Resolver::Declare(const Token &name)
{
    if (mScopes.empty())
        return;
    auto &scope = mScopes.front();

    if (scope.contains(name.Lexeme()))
        Lox::Error(name, "Already variable with this name in this scope.");

    scope[name.Lexeme()] = false;
}

void Resolver::Define(const Token &name)
{
    if (mScopes.empty())
        return;
    mScopes.front()[name.Lexeme()] = true;
}

void Resolver::ResolveLocal(const Expr &expr, const Token &name)
{
    for (size_t i = 0; i < mScopes.size(); i++)
    {
        if (mScopes.at(i).contains(name.Lexeme()))
        {
            mInterpreter.Resolve(expr, i);
            return;
        }
    }
}

void Resolver::ResolveFunction(const Function &func, FunctionType type)
{
    auto enclosingFunction = mCurrentFunction;
    mCurrentFunction = type;

    BeginScope();
    for (auto param : func.mParams)
    {
        Declare(*param);
        Define(*param);
    }
    Resolve(func.mBody);
    EndScope();

    mCurrentFunction = enclosingFunction;
}

} // namespace cclox
