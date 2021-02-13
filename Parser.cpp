#include "Parser.h"

namespace Cclox
{

using std::shared_ptr;
using std::vector;

// Syntax Grammar
// http://www.craftinginterpreters.com/appendix-i.html

shared_ptr<Expr> Parser::Parse()
{
    try
    {
        return ParseExpression();
    }
    catch (const ParseError &error)
    {
        return nullptr;
    }
}

shared_ptr<Expr> Parser::ParseExpression()
{
    return ParseEquality();
}

// equality       → comparison ( ( "!=" | "==" ) comparison )* ;
shared_ptr<Expr> Parser::ParseEquality()
{
    auto expr = ParseComparison();

    while (Match(vector<TokenType>{TOKEN_BANG_EQUAL, TOKEN_EQUAL_EQUAL}))
    {
        auto op = Previous();
        auto right = ParseComparison();
        expr = make_shared<Binary>(expr, op, right);
    }
    return expr;
}

// comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
shared_ptr<Expr> Parser::ParseComparison()
{
    auto expr = ParseTerm();

    while (Match(vector<TokenType>{TOKEN_GREATER, TOKEN_GREATER_EQUAL, TOKEN_LESS, TOKEN_LESS_EQUAL}))
    {
        auto op = Previous();
        auto right = ParseTerm();
        expr = make_shared<Binary>(expr, op, right);
    }

    return expr;
}

// term           → factor ( ( "-" | "+" ) factor )* ;
shared_ptr<Expr> Parser::ParseTerm()
{
    auto expr = ParseFactor();

    while (Match(vector<TokenType>{TOKEN_MINUS, TOKEN_PLUS}))
    {
        auto op = Previous();
        auto right = ParseFactor();
        expr = make_shared<Binary>(expr, op, right);
    }
    return expr;
}

// factor         → unary ( ( "/" | "*" ) unary )* ;
shared_ptr<Expr> Parser::ParseFactor()
{
    auto expr = ParseUnary();

    while (Match(vector<TokenType>{TOKEN_SLASH, TOKEN_STAR}))
    {
        auto op = Previous();
        auto right = ParseUnary();
        expr = make_shared<Binary>(expr, op, right);
    }
    return expr;
}

// unary          → ( "!" | "-" ) unary
//                | primary ;
shared_ptr<Expr> Parser::ParseUnary()
{
    if (Match(vector<TokenType>{TOKEN_BANG, TOKEN_MINUS}))
    {
        auto op = Previous();
        auto right = ParseUnary();
        return make_shared<Unary>(op, right);
    }
    return ParsePrimary();
}

// primary        → NUMBER | STRING | "true" | "false" | "nil"
//                | "(" expression ")" ;
shared_ptr<Expr> Parser::ParsePrimary()
{
    if (Match(TOKEN_FALSE))
        return make_shared<Literal>(make_shared<Object>(OBJ_BOOL_FALSE));
    if (Match(TOKEN_TRUE))
        return make_shared<Literal>(make_shared<Object>(OBJ_BOOL_TRUE));
    if (Match(TOKEN_NIL))
        return make_shared<Literal>(make_shared<Object>());

    if (Match(vector<TokenType>{TOKEN_NUMBER, TOKEN_STRING}))
        return make_shared<Literal>(make_shared<Object>(Previous()->Literal()));

    if (Match(TOKEN_LEFT_PAREN))
    {
        auto expr = ParseExpression();
        Consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
        return make_shared<Grouping>(expr);
    }

    throw Error(*Peek(), "Expect expression.");
}

bool Parser::Match(const TokenType &type)
{
    return Match(vector<TokenType>{type});
}

bool Parser::Match(const vector<TokenType> &types)
{
    for (auto type : types)
    {
        if (Check(type))
        {
            Advance();
            return true;
        }
    }
    return false;
}

bool Parser::Check(const TokenType &type)
{
    if (IsAtEnd())
    {
        return false;
    }
    return Peek()->Type() == type;
}

shared_ptr<Token> Parser::Advance()
{
    if (!IsAtEnd())
    {
        mCurrent++;
    }
    return Previous();
}

bool Parser::IsAtEnd()
{
    return Peek()->Type() == TOKEN_EOF;
}

shared_ptr<Token> Parser::Peek()
{
    return mTokens.at(mCurrent);
}

shared_ptr<Token> Parser::Previous()
{
    return mTokens.at(mCurrent - 1);
}

shared_ptr<Token> Parser::Consume(const TokenType &type, const string &message)
{
    if (Check(type))
        return Advance();

    throw(Error(*Peek(), message));
}

ParseError Parser::Error(const Token &token, const string &message)
{
    Lox::Error(token, message);
    return ParseError();
}

} // namespace Cclox
