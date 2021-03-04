#include "Scanner.h"

#include "Lox.h"

namespace lox
{

const vector<shared_ptr<Token>> &Scanner::ScanTokens()
{
    while (!IsAtEnd())
    {
        mStart = mCurrent;
        ScanToken();
    }

    mTokens.push_back(make_shared<Token>(TOKEN_EOF, "", "", mLine));
    return mTokens;
}

void Scanner::ScanToken()
{
    char c = Advance();
    switch (c)
    {
    case '(':
        AddToken(TOKEN_LEFT_PAREN);
        break;
    case ')':
        AddToken(TOKEN_RIGHT_PAREN);
        break;
    case '{':
        AddToken(TOKEN_LEFT_BRACE);
        break;
    case '}':
        AddToken(TOKEN_RIGHT_BRACE);
        break;
    case ',':
        AddToken(TOKEN_COMMA);
        break;
    case '.':
        AddToken(TOKEN_DOT);
        break;
    case '-':
        AddToken(TOKEN_MINUS);
        break;
    case '+':
        AddToken(TOKEN_PLUS);
        break;
    case ';':
        AddToken(TOKEN_SEMICOLON);
        break;
    case '*':
        AddToken(TOKEN_STAR);
        break;
        /* two-char-tokens */
    case '!':
        AddToken(Match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        break;
    case '=':
        AddToken(Match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        break;
    case '<':
        AddToken(Match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        break;
    case '>':
        AddToken(Match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        break;

    case '/':
        if (Match('/'))
        {
            // A comment goes until the end of the line.
            while (Peek() != '\n' && !IsAtEnd())
                Advance();
        }
        else
        {
            AddToken(TOKEN_SLASH);
        }
        break;

        /* whitespace (ignored) */
    case ' ':
    case '\r':
    case '\t':
        break;

    case '\n':
        mLine++;
        break;

    case '"':
        String();
        break;

    default:
        if (IsDigit(c))
        {
            Number();
        }
        else if (IsAlpha(c))
        {
            Identifier();
        }
        else
        {
            Lox::Error(mLine, "Unexpected character.");
        }
        break;
    }
}

bool Scanner::IsAtEnd() const
{
    return mCurrent >= mSource.length();
}

char Scanner::Advance()
{
    mCurrent++;
    return mSource.at(mCurrent - 1);
}

void Scanner::AddToken(TokenType type)
{
    AddToken(type, "");
}

template <typename L> void Scanner::AddToken(TokenType type, const L &literal)
{
    string text = mSource.substr(mStart, mCurrent - mStart);
    mTokens.push_back(make_shared<Token>(type, text, literal, mLine));
}

bool Scanner::Match(const char &expected)
{
    if (IsAtEnd())
        return false;
    if (mSource.at(mCurrent) != expected)
        return false;

    mCurrent++;
    return true;
}

char Scanner::Peek() const
{
    if (IsAtEnd())
        return '\0';
    return mSource.at(mCurrent);
}

char Scanner::PeekNext() const
{
    if (mCurrent + 1 >= mSource.length())
        return '\0';
    return mSource.at(mCurrent + 1);
}

void Scanner::String()
{
    while (Peek() != '"' && !IsAtEnd())
    {
        if (Peek() == '\n')
            mLine++;
        Advance();
    }

    if (IsAtEnd())
    {
        Lox::Error(mLine, "Unterminated string.");
        return;
    }

    Advance(); // The closing ".

    string value = mSource.substr(mStart + 1, mCurrent - mStart - 2);
    AddToken(TOKEN_STRING, value);
}

bool Scanner::IsDigit(const char &c) const
{
    return c >= '0' && c <= '9';
}

bool Scanner::IsAlpha(const char &c) const
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Scanner::IsAlphaNumeric(const char &c) const
{
    return IsAlpha(c) || IsDigit(c);
}

void Scanner::Number()
{
    while (IsDigit(Peek()))
        Advance();

    if (Peek() == '.' && IsDigit(PeekNext()))
    {
        Advance(); // Consume the "."
        while (IsDigit(Peek()))
            Advance();
    }

    AddToken(TOKEN_NUMBER, stod(mSource.substr(mStart, mCurrent - mStart)));
}

void Scanner::Identifier()
{
    while (IsAlphaNumeric(Peek()))
        Advance();

    string text = mSource.substr(mStart, mCurrent - mStart);
    TokenType type = sKeywords.contains(text) ? sKeywords.at(text) : TOKEN_IDENTIFIER;

    AddToken(type);
}

} // namespace lox
