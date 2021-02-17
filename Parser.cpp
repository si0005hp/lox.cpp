#include "Parser.h"

namespace Cclox
{

using std::shared_ptr;
using std::vector;

// Syntax Grammar
// http://www.craftinginterpreters.com/appendix-i.html

vector<shared_ptr<Stmt>> Parser::Parse()
{
    vector<shared_ptr<Stmt>> stmts;
    while (!IsAtEnd())
        stmts.push_back(ParseDeclaration());
    return stmts;
}

// declaration    → classDecl
//                | funDecl
//                | varDecl
//                | statement ;
shared_ptr<Stmt> Parser::ParseDeclaration()
{
    try
    {
        if (Match(TOKEN_VAR))
            return ParseVarDeclaration();
        return ParseStatement();
    }
    catch (const ParseError &error)
    {
        Synchronize();
        return nullptr;
    }
}

// varDecl        → "var" IDENTIFIER ( "=" expression )? ";" ;
shared_ptr<Stmt> Parser::ParseVarDeclaration()
{
    auto name = Consume(TOKEN_IDENTIFIER, "Expect variable name.");

    auto initializer = Match(TOKEN_EQUAL) ? ParseExpression() : nullptr;

    Consume(TOKEN_SEMICOLON, "Expect ';' after declaration.");
    return make_shared<Var>(name, initializer);
}

// statement      → exprStmt
//                | printStmt
//                | block ;
shared_ptr<Stmt> Parser::ParseStatement()
{
    if (Match(TOKEN_PRINT))
        return ParsePrintStatement();
    if (Match(TOKEN_LEFT_BRACE))
        return ParseBlock();
    return ParseExpressionStatement();
}

// printStmt      → "print" expression ";" ;
shared_ptr<Stmt> Parser::ParsePrintStatement()
{
    shared_ptr<Expr> value = ParseExpression();
    Consume(TOKEN_SEMICOLON, "Expect ';' after value.");
    return make_shared<Print>(value);
}

// exprStmt       → expression ";" ;
shared_ptr<Stmt> Parser::ParseExpressionStatement()
{
    shared_ptr<Expr> value = ParseExpression();
    Consume(TOKEN_SEMICOLON, "Expect ';' after value.");
    return make_shared<Expression>(value);
}

// block          → "{" declaration* "}" ;
shared_ptr<Stmt> Parser::ParseBlock()
{
    vector<shared_ptr<Stmt>> stmts;

    while (!Check(TOKEN_RIGHT_BRACE) && !IsAtEnd())
        stmts.push_back(ParseDeclaration());

    Consume(TOKEN_RIGHT_BRACE, "Expect '}' after block.");
    return make_shared<Block>(stmts);
}

shared_ptr<Expr> Parser::ParseExpression()
{
    return ParseAssignment();
}

// assignment     → IDENTIFIER "=" assignment
//                | equality ;
shared_ptr<Expr> Parser::ParseAssignment()
{
    auto expr = ParseEquality();

    if (Match(TOKEN_EQUAL))
    {
        auto equals = Previous();
        auto value = ParseAssignment();
        if (typeid(*expr) == typeid(Variable))
        {
            auto name = static_pointer_cast<Variable>(expr)->mName;
            return make_shared<Assign>(name, value);
        }
        Error(*equals, "Invalid assignment target.");
    }

    return expr;
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

// primary        → "true" | "false" | "nil" | "this"
//                | NUMBER | STRING | IDENTIFIER | "(" expression ")"
//                | "super" "." IDENTIFIER ;
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

    if (Match(TOKEN_IDENTIFIER))
        return make_shared<Variable>(Previous());

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

bool Parser::Check(const TokenType &type) const
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

bool Parser::IsAtEnd() const
{
    return Peek()->Type() == TOKEN_EOF;
}

shared_ptr<Token> Parser::Peek() const
{
    return mTokens.at(mCurrent);
}

shared_ptr<Token> Parser::Previous() const
{
    return mTokens.at(mCurrent - 1);
}

shared_ptr<Token> Parser::Consume(const TokenType &type, const string &message)
{
    if (Check(type))
        return Advance();

    throw(Error(*Peek(), message));
}

ParseError Parser::Error(const Token &token, const string &message) const
{
    Lox::Error(token, message);
    return ParseError();
}

void Parser::Synchronize()
{
    Advance();
    while (!IsAtEnd())
    {
        if (Previous()->Type() == TOKEN_SEMICOLON)
            return;

        switch (Peek()->Type())
        {
        case TOKEN_CLASS:
        case TOKEN_FUN:
        case TOKEN_VAR:
        case TOKEN_FOR:
        case TOKEN_IF:
        case TOKEN_WHILE:
        case TOKEN_PRINT:
        case TOKEN_RETURN:
            return;
        default:
            break;
        }

        Advance();
    }
}

} // namespace Cclox
