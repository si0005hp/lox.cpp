#include <any>
#include <iostream>
#include <sstream>

#include "Token.h"

using namespace Cclox;

int main(int argc, char const *argv[])
{
    // Token t(TokenType::ELSE, "aaa", "a", 0);
    Token t(TokenType::TOKEN_NUMBER, "axx", 12, 0);
    Token t2(TokenType::TOKEN_STRING, "asfsdg", "hello", 0);

    std::cout << t << std::endl;

    return 0;
}
