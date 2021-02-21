#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "Token.h"

namespace cclox
{

using std::make_shared;
using std::shared_ptr;
using std::stod;
using std::unordered_map;
using std::vector;

class Scanner
{
  public:
    Scanner(const string &source) : mSource(source)
    {
    }

    const vector<shared_ptr<Token>> &ScanTokens();

  private:
    inline static const unordered_map<string, TokenType> sKeywords = {
        {"and", TOKEN_AND},   {"class", TOKEN_CLASS}, {"else", TOKEN_ELSE},     {"false", TOKEN_FALSE},
        {"for", TOKEN_FOR},   {"fun", TOKEN_FUN},     {"if", TOKEN_IF},         {"nil", TOKEN_NIL},
        {"or", TOKEN_OR},     {"print", TOKEN_PRINT}, {"return", TOKEN_RETURN}, {"super", TOKEN_SUPER},
        {"this", TOKEN_THIS}, {"true", TOKEN_TRUE},   {"var", TOKEN_VAR},       {"while", TOKEN_WHILE},
    };

    void ScanToken();
    bool IsAtEnd() const;
    char Advance();
    void AddToken(TokenType type);
    template <typename L> void AddToken(TokenType type, const L &literal);
    bool Match(const char &expected);
    char Peek() const;
    char PeekNext() const;
    void String();
    bool IsDigit(const char &c) const;
    bool IsAlpha(const char &c) const;
    bool IsAlphaNumeric(const char &c) const;
    void Number();
    void Identifier();

    string mSource;
    vector<shared_ptr<Token>> mTokens;

    size_t mStart = 0;
    size_t mCurrent = 0;
    size_t mLine = 1;
};

} // namespace cclox
