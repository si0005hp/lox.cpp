#include "TestUtil.h"
#include "Value.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace cclox;
using namespace std;

class ValueTestFixture : public CcloxTestFixtureBase
{
  public:
    ValueTestFixture()
    {
    }
    void SetUp()
    {
    }
    void TearDown()
    {
    }
};

TEST_F(ValueTestFixture, StringValue)
{
    shared_ptr<Value> v = make_shared<StringValue>("someStr");
    ASSERT_THROW(v->AsNumber(), UnsupposedValueOperationError);
    ASSERT_THROW(v->AsBoolean(), UnsupposedValueOperationError);
    ASSERT_EQ(v->AsString(), "someStr");

    ASSERT_FALSE(v->IsBoolean());
    ASSERT_FALSE(v->IsNumber());
    ASSERT_TRUE(v->IsString());

    ASSERT_FALSE(v->Equals(NumberValue(1)));
    ASSERT_FALSE(v->Equals(BooleanValue(true)));
    ASSERT_FALSE(v->Equals(StringValue("someOtherStr")));
    ASSERT_TRUE(v->Equals(StringValue("someStr")));

    ASSERT_EQ(v->Str(), "someStr");
}

TEST_F(ValueTestFixture, NumberValue)
{
    shared_ptr<Value> v = make_shared<NumberValue>(98.4);
    ASSERT_THROW(v->AsString(), UnsupposedValueOperationError);
    ASSERT_THROW(v->AsBoolean(), UnsupposedValueOperationError);
    ASSERT_EQ(v->AsNumber(), 98.4);

    ASSERT_FALSE(v->IsString());
    ASSERT_FALSE(v->IsBoolean());
    ASSERT_TRUE(v->IsNumber());

    ASSERT_FALSE(v->Equals(BooleanValue(true)));
    ASSERT_FALSE(v->Equals(StringValue("someStr")));
    ASSERT_FALSE(v->Equals(NumberValue(1)));
    ASSERT_TRUE(v->Equals(NumberValue(98.4)));

    ASSERT_EQ(v->Str(), "98.4");
}

TEST_F(ValueTestFixture, BooleanValue)
{
    shared_ptr<Value> v = make_shared<BooleanValue>(true);
    ASSERT_THROW(v->AsString(), UnsupposedValueOperationError);
    ASSERT_THROW(v->AsNumber(), UnsupposedValueOperationError);
    ASSERT_EQ(v->AsBoolean(), true);

    ASSERT_FALSE(v->IsString());
    ASSERT_FALSE(v->IsNumber());
    ASSERT_TRUE(v->IsBoolean());

    ASSERT_FALSE(v->Equals(StringValue("someStr")));
    ASSERT_FALSE(v->Equals(NumberValue(1)));
    ASSERT_FALSE(v->Equals(BooleanValue(false)));
    ASSERT_TRUE(v->Equals(BooleanValue(true)));

    ASSERT_EQ(v->Str(), "true");
}
