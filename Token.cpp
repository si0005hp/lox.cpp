#include "Token.h"

namespace lox
{

std::ostream &operator<<(std::ostream &cout, const Token &token)
{
    switch (token.Type())
    {
    case TOKEN_LEFT_PAREN:
        cout << "LEFT_PAREN";
        break;
    case TOKEN_RIGHT_PAREN:
        cout << "RIGHT_PAREN";
        break;
    case TOKEN_LEFT_BRACE:
        cout << "LEFT_BRACE";
        break;
    case TOKEN_RIGHT_BRACE:
        cout << "RIGHT_BRACE";
        break;
    case TOKEN_COMMA:
        cout << "COMMA";
        break;
    case TOKEN_DOT:
        cout << "DOT";
        break;
    case TOKEN_MINUS:
        cout << "MINUS";
        break;
    case TOKEN_PLUS:
        cout << "PLUS";
        break;
    case TOKEN_SEMICOLON:
        cout << "SEMICOLON";
        break;
    case TOKEN_SLASH:
        cout << "SLASH";
        break;
    case TOKEN_STAR:
        cout << "STAR";
        break;
    case TOKEN_BANG:
        cout << "BANG";
        break;
    case TOKEN_BANG_EQUAL:
        cout << "BANG_EQUAL";
        break;
    case TOKEN_EQUAL:
        cout << "EQUAL";
        break;
    case TOKEN_EQUAL_EQUAL:
        cout << "EQUAL_EQUAL";
        break;
    case TOKEN_GREATER:
        cout << "GREATER";
        break;
    case TOKEN_GREATER_EQUAL:
        cout << "GREATER_EQUAL";
        break;
    case TOKEN_LESS:
        cout << "LESS";
        break;
    case TOKEN_LESS_EQUAL:
        cout << "LESS_EQUAL";
        break;
    case TOKEN_IDENTIFIER:
        cout << "IDENTIFIER";
        break;
    case TOKEN_STRING:
        cout << "STRING";
        break;
    case TOKEN_NUMBER:
        cout << "NUMBER";
        break;
    case TOKEN_AND:
        cout << "AND";
        break;
    case TOKEN_CLASS:
        cout << "CLASS";
        break;
    case TOKEN_ELSE:
        cout << "ELSE";
        break;
    case TOKEN_FALSE:
        cout << "FALSE";
        break;
    case TOKEN_FUN:
        cout << "FUN";
        break;
    case TOKEN_FOR:
        cout << "FOR";
        break;
    case TOKEN_IF:
        cout << "IF";
        break;
    case TOKEN_NIL:
        cout << "NIL";
        break;
    case TOKEN_OR:
        cout << "OR";
        break;
    case TOKEN_PRINT:
        cout << "PRINT";
        break;
    case TOKEN_RETURN:
        cout << "RETURN";
        break;
    case TOKEN_SUPER:
        cout << "SUPER";
        break;
    case TOKEN_THIS:
        cout << "THIS";
        break;
    case TOKEN_TRUE:
        cout << "TRUE";
        break;
    case TOKEN_VAR:
        cout << "VAR";
        break;
    case TOKEN_WHILE:
        cout << "WHILE";
        break;
    case TOKEN_EOF:
        cout << "EOF";
        break;
    default:
        break;
    }
    cout << " ";

    cout << token.Lexeme() << " ";

    if (token.Type() == TOKEN_STRING)
    {
        cout << token.Literal().Text();
    }
    else if (token.Type() == TOKEN_NUMBER)
    {
        cout << token.Literal().Number();
    }
    else
    {
        cout << "unknown literal";
    }
    cout << " ";

    cout << token.Line();

    return cout;
}

} // namespace lox
