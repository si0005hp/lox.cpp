#pragma once

#include <memory>
#include <vector>

#include "Expr.h"
#include "Lox.h"
#include "Stmt.h"
#include "Token.h"

namespace Cclox
{

using std::exception;
using std::make_shared;
using std::shared_ptr;
using std::vector;

class ParseError : public exception
{
  public:
    ParseError()
    {
    }
    ParseError(const string &msg) : mMsg(msg)
    {
    }
    virtual const char *what() const throw()
    {
        return mMsg.c_str();
    }

    const string mMsg;
};

class Parser
{
  public:
    Parser(const vector<shared_ptr<Token>> &tokens) : mTokens(tokens)
    {
    }
    vector<shared_ptr<Stmt>> Parse();
    // TODO: delete as it's temporal
    shared_ptr<Expr> ParseExpr();

    /* public scope for test */
    shared_ptr<Stmt> ParseDeclaration();
    shared_ptr<Stmt> ParseVarDeclaration();

    shared_ptr<Stmt> ParseStatement();
    shared_ptr<Stmt> ParsePrintStatement();
    shared_ptr<Stmt> ParseExpressionStatement();

    shared_ptr<Expr> ParseExpression();
    shared_ptr<Expr> ParseEquality();
    shared_ptr<Expr> ParseComparison();
    shared_ptr<Expr> ParseTerm();
    shared_ptr<Expr> ParseFactor();
    shared_ptr<Expr> ParseUnary();
    shared_ptr<Expr> ParsePrimary();

  private:
    bool Match(const TokenType &type);
    bool Match(const vector<TokenType> &types);

    bool Check(const TokenType &type);
    shared_ptr<Token> Advance();
    bool IsAtEnd();
    shared_ptr<Token> Peek();
    shared_ptr<Token> Previous();
    shared_ptr<Token> Consume(const TokenType &type, const string &message);
    ParseError Error(const Token &token, const string &message);
    void Synchronize();

    const vector<shared_ptr<Token>> mTokens;
    int mCurrent = 0;
};

} // namespace Cclox
