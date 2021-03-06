#pragma once

#include <sstream>
#include <string>

#include "Object.h"

namespace lox
{

using std::string;
using std::stringstream;

enum TokenType
{
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_MINUS,
    TOKEN_PLUS,
    TOKEN_SEMICOLON,
    TOKEN_SLASH,
    TOKEN_STAR,

    // One or two character tokens.
    TOKEN_BANG,
    TOKEN_BANG_EQUAL,
    TOKEN_EQUAL,
    TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER,
    TOKEN_GREATER_EQUAL,
    TOKEN_LESS,
    TOKEN_LESS_EQUAL,

    // Literals.
    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_NUMBER,

    // Keywords.
    TOKEN_AND,
    TOKEN_CLASS,
    TOKEN_ELSE,
    TOKEN_FALSE,
    TOKEN_FUN,
    TOKEN_FOR,
    TOKEN_IF,
    TOKEN_NIL,
    TOKEN_OR,
    TOKEN_PRINT,
    TOKEN_RETURN,
    TOKEN_SUPER,
    TOKEN_THIS,
    TOKEN_TRUE,
    TOKEN_VAR,
    TOKEN_WHILE,

    TOKEN_EOF
};

class Token
{
  public:
    Token(TokenType type, const string &lexeme, const string &literal, int line)
        : mType(type), mLexeme(lexeme), mLiteral(Object(literal)), mLine(line)
    {
    }

    Token(TokenType type, const string &lexeme, double literal, int line)
        : mType(type), mLexeme(lexeme), mLiteral(Object(literal)), mLine(line)
    {
    }

    TokenType Type() const
    {
        return mType;
    }
    const string &Lexeme() const
    {
        return mLexeme;
    }
    const Object &Literal() const
    {
        return mLiteral;
    }
    int Line() const
    {
        return mLine;
    }

  private:
    TokenType mType;
    string mLexeme;
    Object mLiteral;
    int mLine;
}; // namespace lox

std::ostream &operator<<(std::ostream &cout, const Token &token);

} // namespace lox
