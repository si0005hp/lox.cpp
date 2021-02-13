#include "Object.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace Cclox;
using namespace std;

class ObjectTestFixture : public ::testing::Test
{
  public:
    ObjectTestFixture()
    {
    }
    void SetUp()
    {
    }
    void TearDown()
    {
    }
};

TEST_F(ObjectTestFixture, base)
{
    Object n1 = Object(12);
    ASSERT_EQ(12, n1.Number());
    ASSERT_EQ("12", n1.Str());

    Object n2 = Object(5.78);
    ASSERT_EQ(5.78, n2.Number());
    ASSERT_EQ("5.78", n2.Str());

    Object t1 = Object("foobar");
    ASSERT_EQ("foobar", t1.Text());
    ASSERT_EQ("foobar", t1.Str());

    Object t = Object(OBJ_BOOL_TRUE);
    Object f = Object(OBJ_BOOL_FALSE);
    ASSERT_EQ(true, t.Bool());
    ASSERT_EQ("true", t.Str());
    ASSERT_EQ(false, f.Bool());
    ASSERT_EQ("false", f.Str());

    Object nil = Object();
    ASSERT_EQ("nil", nil.Str());
}
