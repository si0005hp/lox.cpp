
#include "Scanner.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace Cclox;
using namespace std;
using testing::AllOfArray;

class ScannerTestFixture : public ::testing::Test
{
  public:
    ScannerTestFixture()
    {
    }
    void SetUp()
    {
    }
    void TearDown()
    {
    }
};

TEST_F(ScannerTestFixture, basic_tokens)
{
    Scanner s("(){},.-+;*!!=<<=>>===");
    const vector<shared_ptr<Token>> &tokens = s.ScanTokens();

    vector<TokenType> expecteds{
        TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN, TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,   TOKEN_COMMA,       TOKEN_DOT,
        TOKEN_MINUS,      TOKEN_PLUS,        TOKEN_SEMICOLON,  TOKEN_STAR,          TOKEN_BANG,        TOKEN_BANG_EQUAL,
        TOKEN_LESS,       TOKEN_LESS_EQUAL,  TOKEN_GREATER,    TOKEN_GREATER_EQUAL, TOKEN_EQUAL_EQUAL, TOKEN_EOF};

    for (size_t i = 0; i < expecteds.size(); i++)
    {
        ASSERT_EQ(expecteds.at(i), tokens.at(i)->Type());
    }
}

TEST_F(ScannerTestFixture, whitespace)
{
    Scanner s("a b \t\r e\nd");
    const vector<shared_ptr<Token>> &tokens = s.ScanTokens();

    ASSERT_EQ(5, tokens.size());
    for (size_t i = 0; i < tokens.size() - 1; i++)
    {
        ASSERT_EQ(TOKEN_IDENTIFIER, tokens.at(i)->Type());
    }
}

TEST_F(ScannerTestFixture, number)
{
    Scanner s("12 4 78.9");
    const vector<shared_ptr<Token>> &tokens = s.ScanTokens();

    vector<string> expecteds{"12", "4", "78.9"};

    ASSERT_EQ(4, tokens.size());
    for (size_t i = 0; i < tokens.size() - 1; i++)
    {
        ASSERT_EQ(TOKEN_NUMBER, tokens.at(i)->Type());
        ASSERT_EQ(stod(expecteds.at(i)), tokens.at(i)->Literal().Number());
        ASSERT_EQ(expecteds.at(i), tokens.at(i)->Lexeme());
    }
}

TEST_F(ScannerTestFixture, string)
{
    Scanner s("\"foo\""
              "\"bar\""
              "\"hoge\nfuga\"");
    const vector<shared_ptr<Token>> &tokens = s.ScanTokens();

    vector<string> expecteds{"foo", "bar", "hoge\nfuga"};

    ASSERT_EQ(4, tokens.size());
    for (size_t i = 0; i < tokens.size() - 1; i++)
    {
        ASSERT_EQ(TOKEN_STRING, tokens.at(i)->Type());
        ASSERT_EQ(expecteds.at(i), tokens.at(i)->Literal().Text());
        ASSERT_EQ("\"" + expecteds.at(i) + "\"", tokens.at(i)->Lexeme());
    }

    int i = 0;
    ASSERT_EQ(1, tokens.at(i++)->Line());
    ASSERT_EQ(1, tokens.at(i++)->Line());
    ASSERT_EQ(2, tokens.at(i++)->Line());
}

TEST_F(ScannerTestFixture, line)
{
    Scanner s("1\n2\n3");
    const vector<shared_ptr<Token>> &tokens = s.ScanTokens();

    vector<int> expecteds{1, 2, 3};

    ASSERT_EQ(4, tokens.size());
    for (size_t i = 0; i < tokens.size() - 1; i++)
    {
        ASSERT_EQ(expecteds.at(i), tokens.at(i)->Line());
    }
}

TEST_F(ScannerTestFixture, keywords)
{
    Scanner s("if else for");
    const vector<shared_ptr<Token>> &tokens = s.ScanTokens();

    vector<int> expecteds{TOKEN_IF, TOKEN_ELSE, TOKEN_FOR};

    ASSERT_EQ(4, tokens.size());
    for (size_t i = 0; i < tokens.size() - 1; i++)
    {
        ASSERT_EQ(expecteds.at(i), tokens.at(i)->Type());
    }
}
