#include "Parser.h"

namespace lox
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
        if (Match(TOKEN_CLASS))
            return ParseClassDeclaration();
        if (Match(TOKEN_FUN))
            return ParseFunction("function");
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

// classDecl      → "class" IDENTIFIER ( "<" IDENTIFIER )?
//                  "{" function* "}" ;
shared_ptr<Stmt> Parser::ParseClassDeclaration()
{
    auto name = Consume(TOKEN_IDENTIFIER, "Expect class name.");

    shared_ptr<Variable> superclass = nullptr;
    if (Match(TOKEN_LESS))
    {
        Consume(TOKEN_IDENTIFIER, "Expect superclass name.");
        superclass = make_shared<Variable>(Previous());
    }

    Consume(TOKEN_LEFT_BRACE, "Expect '{' before class body.");

    vector<shared_ptr<Function>> methods;
    while (!Check(TOKEN_RIGHT_BRACE) && !IsAtEnd())
        methods.push_back(static_pointer_cast<Function>(ParseFunction("method")));

    Consume(TOKEN_RIGHT_BRACE, "Expect '}' after class body.");
    return make_shared<Class>(name, superclass, methods);
}

// funDecl        → "fun" function ;
// function       → IDENTIFIER "(" parameters? ")" block ;
shared_ptr<Stmt> Parser::ParseFunction(const string &kind)
{
    auto name = Consume(TOKEN_IDENTIFIER, "Expect " + kind + " name.");

    Consume(TOKEN_LEFT_PAREN, "Expect '(' after " + kind + " name.");
    vector<shared_ptr<Token>> params;
    if (!Check(TOKEN_RIGHT_PAREN))
    {
        do
        {
            if (params.size() >= 255)
                Error(*Peek(), "Can't have more than 255 parameters.");

            params.push_back(Consume(TOKEN_IDENTIFIER, "Expect parameter name."));
        } while (Match(TOKEN_COMMA));
    }
    Consume(TOKEN_RIGHT_PAREN, "Expect ')' after parameters.");

    Consume(TOKEN_LEFT_BRACE, "Expect '{' before " + kind + " body.");
    auto body = static_pointer_cast<Block>(ParseBlock())->mStatements;
    return make_shared<Function>(name, params, body);
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
//                | forStmt
//                | ifStmt
//                | printStmt
//                | returnStmt
//                | whileStmt
//                | block ;
shared_ptr<Stmt> Parser::ParseStatement()
{
    if (Match(TOKEN_FOR))
        return ParseForStatement();
    if (Match(TOKEN_IF))
        return ParseIfStatement();
    if (Match(TOKEN_PRINT))
        return ParsePrintStatement();
    if (Match(TOKEN_RETURN))
        return ParseReturnStatement();
    if (Match(TOKEN_WHILE))
        return ParseWhileStatement();
    if (Match(TOKEN_LEFT_BRACE))
        return ParseBlock();
    return ParseExpressionStatement();
}

// forStmt        → "for" "(" ( varDecl | exprStmt | ";" )
//                            expression? ";"
//                            expression? ")" statement ;
shared_ptr<Stmt> Parser::ParseForStatement()
{
    Consume(TOKEN_LEFT_PAREN, "Expect '(' after 'if'.");

    /* parse for syntax parts */
    shared_ptr<Stmt> initializer;
    if (Match(TOKEN_SEMICOLON))
        initializer = nullptr;
    else if (Match(TOKEN_VAR))
        initializer = ParseVarDeclaration();
    else
        initializer = ParseExpressionStatement();

    auto condition = Check(TOKEN_SEMICOLON) ? nullptr : ParseExpression();
    Consume(TOKEN_SEMICOLON, "Expect ';' after for loop condition.");

    auto iteration = Check(TOKEN_RIGHT_PAREN) ? nullptr : ParseExpression();
    Consume(TOKEN_RIGHT_PAREN, "Expect ')' after for clauses.");

    auto body = ParseStatement();

    /* construct to while node */
    if (iteration)
        body = make_shared<Block>(vector<shared_ptr<Stmt>>{body, make_shared<Expression>(iteration)});

    if (!condition)
        condition = make_shared<Literal>(make_shared<Object>(OBJ_BOOL_TRUE));
    body = make_shared<While>(condition, body);

    if (initializer)
        body = make_shared<Block>(vector<shared_ptr<Stmt>>{initializer, body});

    return body;
}

// ifStmt         → "if" "(" expression ")" statement
//                ( "else" statement )? ;
shared_ptr<Stmt> Parser::ParseIfStatement()
{
    Consume(TOKEN_LEFT_PAREN, "Expect '(' after 'if'.");
    auto condition = ParseExpression();
    Consume(TOKEN_RIGHT_PAREN, "Expect ')' after if condition.");

    auto thenBranch = ParseStatement();
    auto elseBranch = Match(TOKEN_ELSE) ? ParseStatement() : nullptr;

    return make_shared<If>(condition, thenBranch, elseBranch);
}

// printStmt      → "print" expression ";" ;
shared_ptr<Stmt> Parser::ParsePrintStatement()
{
    shared_ptr<Expr> value = ParseExpression();
    Consume(TOKEN_SEMICOLON, "Expect ';' after value.");
    return make_shared<Print>(value);
}

// returnStmt     → "return" expression? ";" ;
shared_ptr<Stmt> Parser::ParseReturnStatement()
{
    auto keyword = Previous();
    auto value = Check(TOKEN_SEMICOLON) ? nullptr : ParseExpression();

    Consume(TOKEN_SEMICOLON, "Expect ';' after return value.");
    return make_shared<Return>(keyword, value);
}

// whileStmt      → "while" "(" expression ")" statement ;
shared_ptr<Stmt> Parser::ParseWhileStatement()
{
    Consume(TOKEN_LEFT_PAREN, "Expect '(' after 'while'.");
    auto condition = ParseExpression();
    Consume(TOKEN_RIGHT_PAREN, "Expect ')' after 'while'.");
    auto body = ParseStatement();

    return make_shared<While>(condition, body);
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

// expression     → assignment ;
shared_ptr<Expr> Parser::ParseExpression()
{
    return ParseAssignment();
}

// assignment     → ( call "." )? IDENTIFIER "=" assignment
//                | logic_or ;
shared_ptr<Expr> Parser::ParseAssignment()
{
    auto expr = ParseOr(); // TODO change here

    if (Match(TOKEN_EQUAL))
    {
        auto equals = Previous();
        auto value = ParseAssignment();
        if (typeid(*expr) == typeid(Variable))
        {
            auto name = static_pointer_cast<Variable>(expr)->mName;
            return make_shared<Assign>(name, value);
        }
        else if (typeid(*expr) == typeid(Get))
        {
            auto get = static_pointer_cast<Get>(expr);
            return make_shared<Set>(get->mObject, get->mName, value);
        }
        Error(*equals, "Invalid assignment target.");
    }

    return expr;
}

// logic_or       → logic_and ( "or" logic_and )* ;
shared_ptr<Expr> Parser::ParseOr()
{
    auto expr = ParseAnd();

    while (Match(TOKEN_OR))
    {
        auto op = Previous();
        auto right = ParseAnd();
        expr = make_shared<Logical>(expr, op, right);
    }

    return expr;
}

// logic_and      → equality ( "and" equality )* ;
shared_ptr<Expr> Parser::ParseAnd()
{
    auto expr = ParseEquality();

    while (Match(TOKEN_AND))
    {
        auto op = Previous();
        auto right = ParseEquality();
        expr = make_shared<Logical>(expr, op, right);
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

// unary          → ( "!" | "-" ) unary | call ;
shared_ptr<Expr> Parser::ParseUnary()
{
    if (Match(vector<TokenType>{TOKEN_BANG, TOKEN_MINUS}))
    {
        auto op = Previous();
        auto right = ParseUnary();
        return make_shared<Unary>(op, right);
    }
    return ParseCall();
}

// call           → primary ( "(" arguments? ")" | "." IDENTIFIER )* ;
shared_ptr<Expr> Parser::ParseCall()
{
    auto expr = ParsePrimary();

    while (true)
        if (Match(TOKEN_LEFT_PAREN))
            expr = FinishCall(expr);
        else if (Match(TOKEN_DOT))
        {
            auto name = Consume(TOKEN_IDENTIFIER, "Expect property name after '.'.");
            expr = make_shared<Get>(expr, name);
        }
        else
            break;

    return expr;
}

shared_ptr<Expr> Parser::FinishCall(const shared_ptr<Expr> &callee)
{
    vector<shared_ptr<Expr>> arguments;
    if (!Check(TOKEN_RIGHT_PAREN))
    {
        do
        {
            if (arguments.size() >= 255)
                Error(*Peek(), "Can't have more than 255 arguments.");

            arguments.push_back(ParseExpression());
        } while (Match(TOKEN_COMMA));
    }

    auto paren = Consume(TOKEN_RIGHT_PAREN, "Expect ')' after arguments.");
    return make_shared<Call>(callee, paren, arguments);
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

    if (Match(TOKEN_SUPER))
    {
        auto keyword = Previous();
        Consume(TOKEN_DOT, "Expect '.' after 'super'.");
        auto method = Consume(TOKEN_IDENTIFIER, "Expect superclass method name.");
        return make_shared<Super>(keyword, method);
    }

    if (Match(TOKEN_IDENTIFIER))
        return make_shared<Variable>(Previous());

    if (Match(TOKEN_THIS))
        return make_shared<This>(Previous());

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

} // namespace lox
