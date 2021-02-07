#pragma once

#include <iomanip>
#include <string>

namespace Cclox
{

using std::string;
using std::to_string;

enum ObjectType
{
    OBJ_NUMBER,
    OBJ_TEXT
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

    double Number() const
    {
        return mNumber;
    }
    const string &Text() const
    {
        return mText;
    }

    const string Str() const
    {
        if (mType == OBJ_NUMBER)
        {
            std::ostringstream oss;
            oss << std::noshowpoint << mNumber;
            return oss.str();
        }
        return mText;
    }

  private:
    ObjectType mType;
    string mText;
    double mNumber;
};
} // namespace Cclox
