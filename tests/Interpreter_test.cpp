#include "Interpreter.h"
#include "TestUtil.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace Cclox;
using namespace std;

class InterpreterTestFixture : public CcloxTestFixtureBase
{
  public:
    Interpreter i;

    InterpreterTestFixture()
    {
    }
    void SetUp()
    {
    }
    void TearDown()
    {
    }

    template <typename T> T NextPrimaryAs(shared_ptr<Parser> &p)
    {
        return As<T>(p->ParsePrimary());
    }
    Unary NextUnary(shared_ptr<Parser> &p)
    {
        return As<Unary>(p->ParseUnary());
    }
    Binary NextFactor(shared_ptr<Parser> &p)
    {
        return As<Binary>(p->ParseFactor());
    }
    Binary NextTerm(shared_ptr<Parser> &p)
    {
        return As<Binary>(p->ParseTerm());
    }
    Binary NextComparison(shared_ptr<Parser> &p)
    {
        return As<Binary>(p->ParseComparison());
    }
    Binary NextEquality(shared_ptr<Parser> &p)
    {
        return As<Binary>(p->ParseEquality());
    }
};

TEST_F(InterpreterTestFixture, Literal)
{
    auto p = GenerateParserFromSource("nil \"foobar\" 12.5 true");

    ASSERT_EQ(nullptr, i.Visit(NextPrimaryAs<Literal>(p)));

    auto strVal = i.Visit(NextPrimaryAs<Literal>(p));
    ASSERT_EQ("foobar", strVal->AsString());

    auto numVal = i.Visit(NextPrimaryAs<Literal>(p));
    ASSERT_EQ(12.5, numVal->AsNumber());

    auto boolVal = i.Visit(NextPrimaryAs<Literal>(p));
    ASSERT_TRUE(boolVal->AsBoolean());
}

TEST_F(InterpreterTestFixture, Unary)
{
    auto p = GenerateParserFromSource("-3 !false !!true !1 !\"aaa\"");

    ASSERT_EQ(-3, i.Visit(NextUnary(p))->AsNumber());
    ASSERT_EQ(true, i.Visit(NextUnary(p))->AsBoolean());
    ASSERT_EQ(true, i.Visit(NextUnary(p))->AsBoolean());
    ASSERT_EQ(false, i.Visit(NextUnary(p))->AsBoolean());
    ASSERT_EQ(false, i.Visit(NextUnary(p))->AsBoolean());
}

TEST_F(InterpreterTestFixture, Binary)
{
    auto p = GenerateParserFromSource("2*3 4/2 5+7 12-8 \"abc\"+\"de\" 1>2 3>=3 1<6 2<=1 3!=5 8==9 1+2*3-5");

    /* arithmetic */
    ASSERT_EQ(6, i.Visit(NextFactor(p))->AsNumber());
    ASSERT_EQ(2, i.Visit(NextFactor(p))->AsNumber());
    ASSERT_EQ(12, i.Visit(NextTerm(p))->AsNumber());
    ASSERT_EQ(4, i.Visit(NextTerm(p))->AsNumber());
    /* string append */
    ASSERT_EQ("abcde", i.Visit(NextTerm(p))->AsString());
    /* comparison */
    ASSERT_EQ(false, i.Visit(NextComparison(p))->AsBoolean());
    ASSERT_EQ(true, i.Visit(NextComparison(p))->AsBoolean());
    ASSERT_EQ(true, i.Visit(NextComparison(p))->AsBoolean());
    ASSERT_EQ(false, i.Visit(NextComparison(p))->AsBoolean());
    /* equality */
    ASSERT_EQ(true, i.Visit(NextEquality(p))->AsBoolean());
    ASSERT_EQ(false, i.Visit(NextEquality(p))->AsBoolean());

    /* some compound case */
    ASSERT_EQ(2, i.Visit(NextTerm(p))->AsNumber());
}
