#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "Token.h"

namespace Cclox
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

    vector<shared_ptr<Token>> ScanTokens();

  private:
    static const unordered_map<string, TokenType> sKeywords;

    void ScanToken();
    bool IsAtEnd();
    char Advance();
    void AddToken(TokenType type);
    template <typename L> void AddToken(TokenType type, const L &literal);
    bool Match(const char &expected);
    char Peek();
    char PeekNext();
    void String();
    bool IsDigit(const char &c);
    bool IsAlpha(const char &c);
    bool IsAlphaNumeric(const char &c);
    void Number();
    void Identifier();

    string mSource;
    vector<shared_ptr<Token>> mTokens;

    size_t mStart = 0;
    size_t mCurrent = 0;
    size_t mLine = 1;
};

} // namespace Cclox
