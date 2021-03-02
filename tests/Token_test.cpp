#include "TestUtil.h"
#include "Token.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace lox;
using namespace std;

class TokenTestFixture : public CcloxTestFixtureBase
{
  public:
    TokenTestFixture()
    {
    }
    void SetUp()
    {
    }
    void TearDown()
    {
    }

  protected:
    string TokenAsString(Token t)
    {
        stringstream ss;
        ss << t;
        return ss.str();
    }
};

TEST_F(TokenTestFixture, base)
{
    ASSERT_EQ("NUMBER 12 12 1", TokenAsString(Token(TokenType::TOKEN_NUMBER, "12", 12, 1)));
    ASSERT_EQ("STRING Hello Hello 13", TokenAsString(Token(TokenType::TOKEN_STRING, "Hello", "Hello", 13)));
}
