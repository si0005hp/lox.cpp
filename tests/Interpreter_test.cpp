#include "Interpreter.h"
#include "TestUtil.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace cclox;
using namespace std;

class InterpreterTestFixture : public CcloxTestFixtureBase
{
  public:
    std::ostringstream testOs;
    Interpreter i;
    const Environment &iEnv;

    InterpreterTestFixture() : i(Interpreter(testOs)), iEnv(i.CEnvironment())
    {
    }
    void SetUp()
    {
    }
    void TearDown()
    {
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

TEST_F(InterpreterTestFixture, Print)
{
    auto p = GenerateParserFromSource("print 1; print \"jack\"; print true; print nil;");
    i.Interpret(p->Parse());

    ASSERT_EQ("1\njack\ntrue\nnil\n", testOs.str());
}

TEST_F(InterpreterTestFixture, Var)
{
    auto p = GenerateParserFromSource("var a; var b = 10; var s = \"hi\";");
    i.Interpret(p->Parse());

    ASSERT_FALSE(iEnv.Get(token("a")));
    ASSERT_EQ(10, iEnv.Get(token("b"))->AsNumber());
    ASSERT_EQ("hi", iEnv.Get(token("s"))->AsString());
}

TEST_F(InterpreterTestFixture, Assign)
{
    auto p = GenerateParserFromSource("var a; var b; var c; a = 2; b = c = 3;");
    i.Interpret(p->Parse());

    ASSERT_EQ(2, iEnv.Get(token("a"))->AsNumber());
    ASSERT_EQ(3, iEnv.Get(token("b"))->AsNumber());
    ASSERT_EQ(3, iEnv.Get(token("c"))->AsNumber());
}

TEST_F(InterpreterTestFixture, Block)
{
    stringstream ss;
    ss << "var a = 1; var b = 2; print a; print b; ";
    ss << "{ var a = 9; print a; print b; } ";
    ss << "print a; print b; ";
    auto p = GenerateParserFromSource(ss.str());
    i.Interpret(p->Parse());

    ASSERT_EQ("1\n2\n9\n2\n1\n2\n", testOs.str());
}

TEST_F(InterpreterTestFixture, If)
{
    stringstream ss;
    ss << "if (true) print 1; ";
    ss << "if (false) print 2; ";
    ss << "if (true) print 3; else print 4; ";
    ss << "if (false) print 3; else print 4; ";
    ss << "if (false) print 5; else if (false) print 6; else print 7; ";
    auto p = GenerateParserFromSource(ss.str());
    i.Interpret(p->Parse());

    ASSERT_EQ("1\n3\n4\n7\n", testOs.str());
}

TEST_F(InterpreterTestFixture, Logical)
{
    stringstream ss;
    ss << "print false or 1; ";
    ss << "print nil or 2; ";
    ss << "print true or 3; ";
    ss << "print 0 or 4; ";
    ss << "print \"s\" or 5; ";
    auto p = GenerateParserFromSource(ss.str());
    i.Interpret(p->Parse());

    ASSERT_EQ("1\n2\ntrue\n0\ns\n", testOs.str());
}

TEST_F(InterpreterTestFixture, While)
{
    stringstream ss;
    ss << "var c = 0; while (c < 3) print c = c + 1;";
    ss << "var a = 0; while (a < 3) { print a; a = a + 1; }";
    auto p = GenerateParserFromSource(ss.str());
    i.Interpret(p->Parse());

    ASSERT_EQ("1\n2\n3\n0\n1\n2\n", testOs.str());
}

TEST_F(InterpreterTestFixture, For)
{
    stringstream ss;
    ss << "for (var c = 0; c < 3;) print c = c + 1;";
    ss << "for (var a = 0; a < 3; a = a + 1) { print a; }";
    auto p = GenerateParserFromSource(ss.str());
    i.Interpret(p->Parse());

    ASSERT_EQ("1\n2\n3\n0\n1\n2\n", testOs.str());
}
