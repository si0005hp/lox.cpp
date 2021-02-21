#include "LoxFunction.h"
#include "Interpreter.h"

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
        return returnValue.mValue;
    }
    return nullptr;
}

size_t LoxFunction::Arity() const
{
    return mDeclaration.mParams.size();
}

} // namespace cclox
