#pragma once

#include "Parser.h"
#include "Scanner.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace Cclox;
using namespace std;

class CcloxTestFixtureBase : public ::testing::Test
{
  public:
    CcloxTestFixtureBase()
    {
    }
    void SetUp()
    {
    }
    void TearDown()
    {
    }

    shared_ptr<Parser> GenerateParserFromSource(const string &source)
    {
        Scanner scanner(source);
        return make_shared<Parser>(scanner.ScanTokens());
    }

    template <typename T> T As(shared_ptr<Expr> expr)
    {
        shared_ptr<T> tp = static_pointer_cast<T>(expr);
        return *tp;
    }
};
