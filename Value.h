#pragma once

#include <iomanip>
#include <string>

#define UNSUPPOSED_OPERATION_ERROR(op) throw(UnsupposedValueOperationError("Unsupposed call: " + string(op)));

namespace Cclox
{

using std::exception;
using std::string;

class UnsupposedValueOperationError : public exception
{
  public:
    UnsupposedValueOperationError(const string &msg) : mMsg(msg)
    {
    }
    virtual const char *what() const throw()
    {
        return mMsg.c_str();
    }
    const string mMsg;
};

class Value
{
  public:
    virtual const string &AsString() const
    {
        UNSUPPOSED_OPERATION_ERROR("AsString")
    }
    virtual const double AsNumber() const
    {
        UNSUPPOSED_OPERATION_ERROR("AsNumber")
    }
    virtual const bool AsBoolean() const
    {
        UNSUPPOSED_OPERATION_ERROR("AsBoolean")
    }

    virtual const bool IsString() const = 0;
    virtual const bool IsNumber() const = 0;
    virtual const bool IsBoolean() const = 0;
    virtual const bool Equals(const Value &other) const = 0;

    virtual const string Str() const = 0;
};

class StringValue : public Value
{
  public:
    StringValue(const string &value) : mValue(value)
    {
    }
    const string &AsString() const override
    {
        return mValue;
    }

    virtual const bool IsString() const override
    {
        return true;
    }
    virtual const bool IsNumber() const override
    {
        return false;
    }
    virtual const bool IsBoolean() const override
    {
        return false;
    }

    virtual const bool Equals(const Value &other) const override
    {
        return other.IsString() && mValue == other.AsString();
    }

    virtual const string Str() const override
    {
        return mValue;
    }

  private:
    string mValue;
};

class NumberValue : public Value
{
  public:
    NumberValue(const double &value) : mValue(value)
    {
    }
    const double AsNumber() const override
    {
        return mValue;
    }

    virtual const bool IsString() const override
    {
        return false;
    }
    virtual const bool IsNumber() const override
    {
        return true;
    }
    virtual const bool IsBoolean() const override
    {
        return false;
    }

    virtual const bool Equals(const Value &other) const override
    {
        return other.IsNumber() && mValue == other.AsNumber();
    }

    virtual const string Str() const override
    {
        std::ostringstream oss;
        oss << std::noshowpoint << mValue;
        return oss.str();
    }

  private:
    double mValue;
};

class BooleanValue : public Value
{
  public:
    BooleanValue(const bool &value) : mValue(value)
    {
    }
    const bool AsBoolean() const override
    {
        return mValue;
    }

    virtual const bool IsString() const override
    {
        return false;
    }
    virtual const bool IsNumber() const override
    {
        return false;
    }
    virtual const bool IsBoolean() const override
    {
        return true;
    }

    virtual const bool Equals(const Value &other) const override
    {
        return other.IsBoolean() && mValue == other.AsBoolean();
    }

    virtual const string Str() const override
    {
        return mValue ? "true" : "false";
    }

  private:
    bool mValue;
};

}; // namespace Cclox
