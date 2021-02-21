#include "Environment.h"
#include "TestUtil.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace cclox;
using namespace std;

class EnvironmentTestFixture : public CcloxTestFixtureBase
{
  public:
    EnvironmentTestFixture()
    {
    }
    void SetUp()
    {
    }
    void TearDown()
    {
    }
};

TEST_F(EnvironmentTestFixture, Define)
{
    Environment e;
    e.Define("num1", make_shared<NumberValue>(11));
    e.Define("str1", make_shared<StringValue>("hoge"));
    e.Define("str2", make_shared<StringValue>("foo"));
    e.Define("str2", make_shared<StringValue>("bar"));
    e.Define("empty1", nullptr);

    ASSERT_EQ(11, e.Get(token("num1"))->AsNumber());
    ASSERT_EQ("hoge", e.Get(token("str1"))->AsString());
    ASSERT_EQ("bar", e.Get(token("str2"))->AsString());
    ASSERT_FALSE(e.Get(token("empty1")));
    ASSERT_THROW(e.Get(token("nokey")), RuntimeError);
}

TEST_F(EnvironmentTestFixture, Assign)
{
    Environment e;
    e.Define("num1", nullptr);
    e.Define("num2", make_shared<NumberValue>(10));
    e.Define("str1", nullptr);

    e.Assign(token("num1"), make_shared<NumberValue>(80));
    e.Assign(token("num2"), make_shared<NumberValue>(90));
    e.Assign(token("str1"), make_shared<StringValue>("bar"));
    ASSERT_THROW(e.Assign(token("nokey"), make_shared<NumberValue>(0)), RuntimeError);

    ASSERT_EQ(80, e.Get(token("num1"))->AsNumber());
    ASSERT_EQ(90, e.Get(token("num2"))->AsNumber());
    ASSERT_EQ("bar", e.Get(token("str1"))->AsString());
}

TEST_F(EnvironmentTestFixture, Get)
{
    auto eRoot = make_shared<Environment>();
    auto e1 = make_shared<Environment>(eRoot);
    auto e2 = make_shared<Environment>(e1);

    eRoot->Define("num1", make_shared<NumberValue>(1));
    eRoot->Define("num2", make_shared<NumberValue>(2));
    e1->Define("num1", make_shared<NumberValue>(11));

    ASSERT_EQ(11, e1->Get(token("num1"))->AsNumber());
    ASSERT_EQ(2, e1->Get(token("num2"))->AsNumber());
    ASSERT_THROW(e1->Get(token("num3")), RuntimeError);

    ASSERT_EQ(11, e2->Get(token("num1"))->AsNumber());
    ASSERT_EQ(2, e2->Get(token("num2"))->AsNumber());
    ASSERT_THROW(e2->Get(token("num3")), RuntimeError);
}
