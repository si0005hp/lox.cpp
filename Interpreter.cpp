#include "Interpreter.h"
#include "Lox.h"
#include "LoxFunction.h"

namespace cclox
{

using std::make_shared;

Interpreter::Interpreter() : Interpreter(std::cout)
{
}

Interpreter::Interpreter(std::ostream &os) : mGlobals(make_shared<Environment>()), mEnvironment(mGlobals), mOs(os)
{
}

template <typename T, typename V> shared_ptr<Value> LoxValue(const V &value)
{
    return make_shared<T>(value);
}

void Interpreter::Interpret(const vector<shared_ptr<Stmt>> &stmts)
{
    try
    {
        for (auto &stmt : stmts)
            Execute(*stmt);
    }
    catch (const RuntimeError &e)
    {
        Lox::ErrorRuntimeError(e);
    }
}

void Interpreter::Visit(const Expression &stmt)
{
    Evaluate(*stmt.mExpression);
}

void Interpreter::Visit(const Print &stmt)
{
    auto value = Evaluate(*stmt.mExpression);
    Println(value ? value->Str() : "nil");
}

void Interpreter::Visit(const Var &stmt)
{
    auto value = stmt.mInitializer ? Evaluate(*stmt.mInitializer) : nullptr;
    mEnvironment->Define(stmt.mName->Lexeme(), value);
}

void Interpreter::Visit(const Block &stmt)
{
    auto newEnvironment = make_shared<Environment>(mEnvironment);
    ExecuteBlock(stmt.mStatements, newEnvironment);
}

void Interpreter::Visit(const If &stmt)
{
    if (IsTruthy(Evaluate(*stmt.mCondition)))
        Execute(*stmt.mThenBranch);
    else if (stmt.mElseBranch)
        Execute(*stmt.mElseBranch);
}

void Interpreter::Visit(const While &stmt)
{
    while (IsTruthy(Evaluate(*stmt.mCondition)))
        Execute(*stmt.mBody);
}

void Interpreter::Visit(const Function &stmt)
{
    auto function = make_shared<LoxFunction>(stmt, mEnvironment);
    mEnvironment->Define(stmt.mName->Lexeme(), function);
}

void Interpreter::Visit(const Return &stmt)
{
    auto value = stmt.mValue ? Evaluate(*stmt.mValue) : nullptr;

    throw FunctionReturn(value);
}

shared_ptr<Value> Interpreter::Visit(const Assign &expr)
{
    auto value = Evaluate(*expr.mValue);
    mEnvironment->Assign(expr.mName, value);
    return value;
}

shared_ptr<Value> Interpreter::Visit(const Binary &expr)
{
    auto left = Evaluate(*expr.mLeft);
    auto right = Evaluate(*expr.mRight);

    switch (expr.mOp->Type())
    {
        /* equality */
    case TOKEN_BANG_EQUAL:
        return LoxValue<BooleanValue>(!IsEqual(left, right));
    case TOKEN_EQUAL_EQUAL:
        return LoxValue<BooleanValue>(IsEqual(left, right));
        /* comparison */
    case TOKEN_GREATER:
        CheckNumberOperands(expr.mOp, left, right);
        return LoxValue<BooleanValue>(left->AsNumber() > right->AsNumber());
    case TOKEN_GREATER_EQUAL:
        CheckNumberOperands(expr.mOp, left, right);
        return LoxValue<BooleanValue>(left->AsNumber() >= right->AsNumber());
    case TOKEN_LESS:
        CheckNumberOperands(expr.mOp, left, right);
        return LoxValue<BooleanValue>(left->AsNumber() < right->AsNumber());
    case TOKEN_LESS_EQUAL:
        CheckNumberOperands(expr.mOp, left, right);
        return LoxValue<BooleanValue>(left->AsNumber() <= right->AsNumber());
        /* arithmetic (& string) */
    case TOKEN_MINUS:
        CheckNumberOperands(expr.mOp, left, right);
        return LoxValue<NumberValue>(left->AsNumber() - right->AsNumber());
    case TOKEN_PLUS:
        if (left->IsNumber() && right->IsNumber())
        {
            return LoxValue<NumberValue>(left->AsNumber() + right->AsNumber());
        }
        else if (left->IsString() && right->IsString())
        {
            return LoxValue<StringValue>(left->AsString() + right->AsString());
        }
        throw RuntimeError(expr.mOp, "Operands must be two numbers or two strings.");
    case TOKEN_SLASH:
        CheckNumberOperands(expr.mOp, left, right);
        return LoxValue<NumberValue>(left->AsNumber() / right->AsNumber());
    case TOKEN_STAR:
        CheckNumberOperands(expr.mOp, left, right);
        return LoxValue<NumberValue>(left->AsNumber() * right->AsNumber());
    default:
        return nullptr;
    }
    return nullptr;
}

shared_ptr<Value> Interpreter::Visit(const Call &expr)
{
    auto callee = Evaluate(*expr.mCallee);

    vector<shared_ptr<Value>> arguments;
    for (auto argument : expr.mArguments)
        arguments.push_back(Evaluate(*argument));

    if (!callee->IsFunction())
        throw RuntimeError(expr.mParen, "Can only call functions and classes.");

    auto function = callee->AsFunction();

    if (arguments.size() != function.Arity())
        throw RuntimeError(expr.mParen, "Expected " + to_string(function.Arity()) + " arguments but got " +
                                            to_string(arguments.size()) + ".");

    return function.Call(*this, arguments);
}

shared_ptr<Value> Interpreter::Visit(const Get &expr)
{
    return nullptr;
}

shared_ptr<Value> Interpreter::Visit(const Grouping &expr)
{
    return Evaluate(*expr.mExpression);
}

shared_ptr<Value> Interpreter::Visit(const Literal &expr)
{
    return InterpretObject(expr.mValue);
}

shared_ptr<Value> Interpreter::Visit(const Logical &expr)
{
    auto left = Evaluate(*expr.mLeft);

    if (expr.mOp->Type() == TOKEN_OR)
    {
        if (IsTruthy(left))
            return left;
    }
    else
    {
        if (!IsTruthy(left))
            return left;
    }

    return Evaluate(*expr.mRight);
}

shared_ptr<Value> Interpreter::Visit(const Set &expr)
{
    return nullptr;
}

shared_ptr<Value> Interpreter::Visit(const Super &expr)
{
    return nullptr;
}

shared_ptr<Value> Interpreter::Visit(const This &expr)
{
    return nullptr;
}

shared_ptr<Value> Interpreter::Visit(const Unary &expr)
{
    auto right = Evaluate(*expr.mRight);

    switch (expr.mOp->Type())
    {
    case TOKEN_BANG:
        return LoxValue<BooleanValue>(!IsTruthy(right));
    case TOKEN_MINUS:
        CheckNumberOperand(expr.mOp, right);
        return LoxValue<NumberValue>(-1 * right->AsNumber());
    default:
        throw std::runtime_error("[ERROR on Visit(Unary)] Illegal op type: " + to_string(expr.mOp->Type()));
    }
    return nullptr; // Unreachable.
}

shared_ptr<Value> Interpreter::Visit(const Variable &expr)
{
    return mEnvironment->Get(expr.mName);
}

void Interpreter::Execute(const Stmt &stmt)
{
    stmt.Accept(*this);
}

void Interpreter::ExecuteBlock(const vector<shared_ptr<Stmt>> &stmts, const shared_ptr<Environment> &environment)
{
    auto previous = mEnvironment;
    try
    {
        mEnvironment = environment;
        for (auto stmt : stmts)
            Execute(*stmt);
    }
    catch (const FunctionReturn &returnValue)
    {
        // TODO fix
        mEnvironment = previous;
        throw returnValue;
    }
    mEnvironment = previous;
}

shared_ptr<Value> Interpreter::Evaluate(const Expr &expr)
{
    return expr.Accept(*this);
}

// false and nil are falsey and everything else is truthy
bool Interpreter::IsTruthy(const shared_ptr<Value> &value) const
{
    if (!value)
        return false;
    if (value->IsBoolean())
        return value->AsBoolean();
    return true;
}

bool Interpreter::IsEqual(const shared_ptr<Value> &left, const shared_ptr<Value> &right) const
{
    if (left == nullptr && right == nullptr)
        return true;
    if (left == nullptr)
        return false;
    return left->Equals(*right);
}

void Interpreter::CheckNumberOperand(const shared_ptr<Token> op, const shared_ptr<Value> &operand) const
{
    if (operand->IsNumber())
        return;
    throw RuntimeError(op, "Operand must be a number.");
}

void Interpreter::CheckNumberOperands(const shared_ptr<Token> op, const shared_ptr<Value> &left,
                                      const shared_ptr<Value> &right) const
{
    if (left->IsNumber() && right->IsNumber())
        return;
    throw RuntimeError(op, "Operands must be numbers.");
}

shared_ptr<Value> Interpreter::InterpretObject(const Object &object) const
{
    switch (object.Type())
    {
    case OBJ_NIL:
        return nullptr;
    case OBJ_TEXT:
        return LoxValue<StringValue>(object.Text());
    case OBJ_NUMBER:
        return LoxValue<NumberValue>(object.Number());
    case OBJ_BOOL:
        return LoxValue<BooleanValue>(object.Bool());
    default:
        throw std::runtime_error("[ERROR on InterpretObject] Illegal object type: " + to_string(object.Type()));
    }
}

shared_ptr<Value> Interpreter::InterpretObject(const shared_ptr<Object> &object) const
{
    return InterpretObject(*object);
}

void Interpreter::Println(const string &str) const
{
    mOs << str << std::endl;
}

}; // namespace cclox
