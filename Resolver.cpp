#include "Resolver.h"
#include "Lox.h"

namespace cclox
{

void Resolver::Visit(const Expression &stmt)
{
    Resolve(stmt);
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

    ResolveFunction(stmt);
}

void Resolver::Visit(const Return &stmt)
{
    if (stmt.mValue)
        Resolve(*stmt.mValue);
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
}

void Resolver::Visit(const Super &expr)
{
}

void Resolver::Visit(const This &expr)
{
}

void Resolver::Visit(const Unary &expr)
{
    Resolve(*expr.mRight);
}

void Resolver::Visit(const Variable &expr)
{
    if (!mScopes.empty() && mScopes.front()[expr.mName->Lexeme()] == false)
        Lox::Error(*expr.mName, "Can't read local variable in its own initializer.");

    ResolveLocal(expr, *expr.mName);
}

void Resolver::Resolve(const vector<shared_ptr<Stmt>> statements)
{
    for (auto stmt : statements)
        Resolve(*stmt);
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
    auto scope = mScopes.front();
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
        if (mScopes.at(0).contains(name.Lexeme()))
        {
            mInterpreter.Resolve(expr, i);
            return;
        }
    }
}

void Resolver::ResolveFunction(const Function &func)
{
    BeginScope();
    for (auto param : func.mParams)
    {
        Declare(*param);
        Define(*param);
    }
    Resolve(func.mBody);
    EndScope();
}

} // namespace cclox
