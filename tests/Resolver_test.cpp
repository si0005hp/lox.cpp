#include "Environment.h"
#include "TestUtil.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace cclox;
using namespace std;

class ResolverTestFixture : public CcloxTestFixtureBase
{
  public:
    std::ostringstream testOs;
    Interpreter i;
    const Environment &iEnv;

    ResolverTestFixture() : i(Interpreter(testOs)), iEnv(i.CEnvironment())
    {
    }
    void SetUp()
    {
    }
    void TearDown()
    {
    }
};

TEST_F(ResolverTestFixture, DuplicatedDeclare)
{
    ParseAndResolve(i, "var a; var a;");
    ASSERT_FALSE(Lox::HadError());

    ParseAndResolve(i, "{ var a; var a; }");
    ASSERT_TRUE(Lox::HadError());
    Lox::ResetError(); // TODO fix
}

TEST_F(ResolverTestFixture, SelfReference)
{
    ParseAndResolve(i, "var a = 1; { var a = a; }");
    ASSERT_TRUE(Lox::HadError());
    Lox::ResetError();
}

TEST_F(ResolverTestFixture, InvalidReturn)
{
    ParseAndResolve(i, "return 1;");
    ASSERT_TRUE(Lox::HadError());
    Lox::ResetError();
}

TEST_F(ResolverTestFixture, InvalidThis)
{
    ParseAndResolve(i, "print this;");
    ASSERT_TRUE(Lox::HadError());
    Lox::ResetError();

    ParseAndResolve(i, "fun notAMethod() { print this; }");
    ASSERT_TRUE(Lox::HadError());
    Lox::ResetError();
}
