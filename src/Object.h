#pragma once

#include <iomanip>
#include <string>

namespace lox
{

using std::string;
using std::to_string;

enum ObjectType
{
    OBJ_NUMBER,
    OBJ_TEXT,
    OBJ_BOOL,
    OBJ_NIL
};

enum ObjectBool
{
    OBJ_BOOL_TRUE,
    OBJ_BOOL_FALSE,
};

class Object
{
  public:
    Object(double number) : mType(OBJ_NUMBER), mNumber(number)
    {
    }
    Object(const string &text) : mType(OBJ_TEXT), mText(text)
    {
    }
    Object(const ObjectBool &b) : mType(OBJ_BOOL), mBool(b)
    {
    }
    Object() : mType(OBJ_NIL)
    {
    }

    ObjectType Type() const
    {
        return mType;
    }
    double Number() const
    {
        return mNumber;
    }
    const string &Text() const
    {
        return mText;
    }
    const bool Bool() const
    {
        return mBool == OBJ_BOOL_TRUE ? true : false;
    }
    const bool IsNil() const
    {
        return mType == OBJ_NIL;
    }

    const string Str() const
    {
        if (mType == OBJ_NUMBER)
        {
            std::ostringstream oss;
            oss << std::noshowpoint << mNumber;
            return oss.str();
        }
        else if (mType == OBJ_BOOL)
        {
            return mBool == OBJ_BOOL_TRUE ? "true" : "false";
        }
        else if (mType == OBJ_NIL)
        {
            return "nil";
        }
        else
        {
            return mText; // text
        }
    }

  private:
    ObjectType mType;
    string mText;
    double mNumber;
    ObjectBool mBool;
};
} // namespace lox
