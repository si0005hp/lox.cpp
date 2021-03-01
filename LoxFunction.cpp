#include "LoxFunction.h"

#include "Interpreter.h"
#include "LoxClass.h"

namespace cclox
{

shared_ptr<Value> LoxFunction::Call(Interpreter &interpreter, const vector<shared_ptr<Value>> &arguments)
{
    auto environment = make_shared<Environment>(mClosure);

    // define function arguments as local variables
    for (size_t i = 0; i < mDeclaration.mParams.size(); i++)
        environment->Define(mDeclaration.mParams.at(i)->Lexeme(), arguments.at(i));

    try
    {
        interpreter.ExecuteBlock(mDeclaration.mBody, environment);
    }
    catch (const FunctionReturn &returnValue)
    {
        return mIsInitializer ? mClosure->GetAt(0, "this") : returnValue.mValue;
    }

    return mIsInitializer ? mClosure->GetAt(0, "this") : nullptr;
}

size_t LoxFunction::Arity() const
{
    return mDeclaration.mParams.size();
}

shared_ptr<LoxFunction> LoxFunction::Bind(const shared_ptr<LoxInstance> &instance)
{
    auto environment = make_shared<Environment>(mClosure);
    environment->Define("this", instance);
    return make_shared<LoxFunction>(mDeclaration, environment, mIsInitializer);
}

} // namespace cclox
